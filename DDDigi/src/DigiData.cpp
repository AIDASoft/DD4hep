//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/InstanceCount.h>
#define  G__ROOT
#include <DDDigi/DigiData.h>

// C/C++ include files
#include <mutex>

namespace   {
  struct digi_keys   {
    std::mutex  lock;
    std::map<unsigned long, std::string> map;
  };
  digi_keys& keys()  {
    static digi_keys k;
    return k;
  }
}

dd4hep::digi::Key::Key()    {
  this->key = 0;
}

dd4hep::digi::Key::Key(const Key& copy)   {
  this->key = copy.key;
}

dd4hep::digi::Key::Key(key_type full_mask)   {
  this->key = full_mask;
}

dd4hep::digi::Key::Key(mask_type mask, const std::string& item)  {
  auto& _k = keys();
  this->key = 0;
  this->values.mask = mask;
  this->values.item = detail::hash32(item);
  std::lock_guard<std::mutex> lock(_k.lock);
  _k.map[key] = item;
}

dd4hep::digi::Key& dd4hep::digi::Key::operator=(const Key& copy)   {
  this->key = copy.key;
  return *this;
}

void dd4hep::digi::Key::set(const std::string& name, int mask)    {
  auto& _k = keys();
  if ( name.empty() )   {
    std::lock_guard<std::mutex> lock(_k.lock);
    except("DDDigi::Key", "+++ No key name was specified  --  this is illegal!");
  }
  this->values.mask = (unsigned char)(0xFF&mask);
  this->values.item = detail::hash32(name);
  std::lock_guard<std::mutex> lock(_k.lock);
  _k.map[this->key] = name;
}

/// Access key name (if registered properly)
std::string dd4hep::digi::Key::key_name(const Key& k)    {
  auto& _k = keys();
  std::lock_guard<std::mutex> lock(_k.lock);
  std::map<unsigned long, std::string>::const_iterator it = _k.map.find(k.key);
  if ( it != _k.map.end() ) return it->second;
  Key kk;
  kk.values.item = ~0x0;
  for( const auto& e : _k.map )   {
    if ( (e.first & kk.key) == k.values.item )
      return e.second;
  }
  return "UNKNOWN";
}

/// Merge new deposit map onto existing map
std::size_t dd4hep::digi::DepositMapping::merge(DepositMapping&& updates)    {
  std::size_t update_size = updates.size();
  for( auto& c : updates )    {
    CellID         cell = c.first;
    EnergyDeposit& depo = c.second;
    auto iter = this->find(cell);
    if ( iter == this->end() )   {
      this->emplace(cell, std::move(depo));
      continue;
    }
    auto& to_update = iter->second;
    to_update.deposit += depo.deposit;
    to_update.history.insert(to_update.history.end(), depo.history.begin(), depo.history.end());
  }
  return update_size;
}

/// Merge new deposit map onto existing map
std::size_t dd4hep::digi::ParticleMapping::merge(ParticleMapping&& updates)    {
  std::size_t update_size = updates.size();
  for( ParticleMapping::value_type& c : updates )    {
    Particle part(std::move(c.second));
    this->push(c.first, std::move(part));
  }
  return update_size;
}

void dd4hep::digi::ParticleMapping::push(Key::key_type k, Particle&& part)  {
  auto ret = this->emplace(k, std::move(part)).second;
  if ( !ret )   {
    Key key(k);
    except("ParticleMapping","Error in particle map. Duplicate ID: mask:%04X Number:%d",
	   key.values.mask, key.values.item);
  }
}

/// Intializing constructor
dd4hep::digi::DigiEvent::DigiEvent()
  : ObjectExtensions(typeid(DigiEvent))
{
  InstanceCount::increment(this);
}

/// Intializing constructor
dd4hep::digi::DigiEvent::DigiEvent(int ev_num)
  : ObjectExtensions(typeid(DigiEvent)), eventNumber(ev_num)
{
  char text[32];
  ::snprintf(text, sizeof(text), "Ev:%06d ", ev_num);
  m_id = text;
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiEvent::~DigiEvent()
{
  InstanceCount::decrement(this);
}

/// Retrieve data segment from the event structure
dd4hep::digi::DigiEvent::container_map_t& 
dd4hep::digi::DigiEvent::get_segment(const std::string& name)   {
  switch(::toupper(name[0]))   {
  case 'I':
    return this->inputs;
  case 'D':
    switch(::toupper(name[1]))   {
    case 'E':
      return this->deposits;
    default:
      return this->data;
    }
    break;
  case 'O':
    return this->data;
  default:
    return this->data;
  }
  throw std::runtime_error("Invalid segment name: "+name);
}

/// Add item by key to the data 
bool dd4hep::digi::DigiEvent::put_data(unsigned long key, std::any&& object)     {
  std::lock_guard<std::mutex> lock(m_lock);
  bool ret = data.emplace(key,object).second;
  if ( ret ) return ret;

  std::string name = Key::key_name(key);
  uint32_t    mask = Key::mask(key);
  except("DigiEvent","%s+++ Invalid requested to store object in data  container [key present]."
	 " Key:%ld [Mask:%04X, name:%s]", this->id(), key, mask, name.c_str());
  throw std::runtime_error("DigiEvent"); // Will never get here!
}

/// Add item by key to the data 
bool dd4hep::digi::DigiEvent::put_input(unsigned long key, std::any&& object)     {
  std::lock_guard<std::mutex> lock(m_lock);
  bool ret = inputs.emplace(key,object).second;
  if ( ret ) return ret;

  std::string name = Key::key_name(key);
  uint32_t    mask = Key::mask(key);
  except("DigiEvent","%s+++ Invalid requested to store object in input container [key present]."
	 " Key:%ld [Mask:%04X, name:%s]", this->id(), key, mask, name.c_str());
  throw std::runtime_error("DigiEvent"); // Will never get here!
}

