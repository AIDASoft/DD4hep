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

using namespace dd4hep::digi;

void Key::set(const std::string& name, int mask)    {
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
std::string Key::key_name(const Key& k)    {
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
std::size_t DepositMapping::merge(DepositMapping&& updates)    {
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
std::size_t ParticleMapping::merge(ParticleMapping&& updates)    {
  std::size_t update_size = updates.size();
  for( ParticleMapping::value_type& c : updates )    {
    Particle part(std::move(c.second));
    this->push(c.first, std::move(part));
  }
  return update_size;
}

void ParticleMapping::push(Key::key_type k, Particle&& part)  {
#if defined(__GNUC__) && (__GNUC__ < 10)
  /// Lower compiler version have a bad implementation of std::any
  bool ret = false;
  if ( part.history.has_value() ) {}
#else
  bool ret = this->emplace(k, std::move(part)).second;
#endif
  if ( !ret )   {
    Key key(k);
    except("ParticleMapping","Error in particle map. Duplicate ID: mask:%04X Number:%d",
	   key.values.mask, key.values.item);
  }
}

/// Initializing constructor
DataSegment::DataSegment(std::mutex& l) : lock(l)
{
}

/// Remove data item from segment
bool DataSegment::emplace(key_type key, std::any&& item)    {
  std::lock_guard<std::mutex> l(lock);
#if defined(__GNUC__) && (__GNUC__ < 10)
  /// Lower compiler version have a bad implementation of std::any
  bool ret = false;
  if ( item.has_value() ) {}
#else
  bool ret = data.emplace(key, std::move(item)).second;
#endif
  if ( !ret )   {
    Key k(key);
    except("DataSegment","Error in DataSegment map. Duplicate ID: mask:%04X Number:%d",
	   k.values.mask, k.values.item);
  }
  return ret;
}

/// Remove data item from segment
bool DataSegment::erase(key_type key)    {
  std::lock_guard<std::mutex> l(lock);
  auto iter = data.find(key);
  if ( iter != data.end() )   {
    data.erase(iter);
    return true;
  }
  return false;
}

/// Remove data items from segment (locked)
std::size_t DataSegment::erase(const std::vector<key_type>& keys)   {
  std::size_t count = 0;
  std::lock_guard<std::mutex> l(lock);
  for(auto key : keys)   {
    auto iter = data.find(key);
    if ( iter != data.end() )   {
      data.erase(iter);
      ++count;
    }
  }
  return count;
}

/// Call on failed any-casts during data requests
std::string DataSegment::invalid_cast(key_type key, const std::type_info& type)  const   {
  return dd4hep::format(0, "Invalid segment data cast. Key:%ld type:%s",
			key, typeName(type).c_str());
}

/// Call on failed data requests during data requests
std::string DataSegment::invalid_request(key_type key)  const   {
  return dd4hep::format(0, "Invalid segment data requested. Key:%ld",key);
}

/// Access data item by key
std::any* DataSegment::get_item(key_type key, bool exc)   {
  auto it = this->data.find(key);
  if (it != this->data.end()) return &it->second;
  if ( exc ) throw std::runtime_error(invalid_request(key));
  return nullptr;
}

/// Access data item by key  (CONST)
const std::any* DataSegment::get_item(key_type key, bool exc)  const   {
  auto it = this->data.find(key);
  if (it != this->data.end()) return &it->second;
  if ( exc ) throw std::runtime_error(invalid_request(key));
  return nullptr;
}

/// Intializing constructor
DigiEvent::DigiEvent()
{
  InstanceCount::increment(this);
}

/// Intializing constructor
DigiEvent::DigiEvent(int ev_num) : eventNumber(ev_num)
{
  char text[32];
  ::snprintf(text, sizeof(text), "Ev:%06d ", ev_num);
  m_id = text;
  InstanceCount::increment(this);
}

/// Default destructor
DigiEvent::~DigiEvent()
{
  InstanceCount::decrement(this);
}

/// Retrieve data segment from the event structure
DataSegment& DigiEvent::get_segment(const std::string& name)   {
  switch(::toupper(name[0]))   {
  case 'I':
    return this->m_inputs;
  case 'D':
    switch(::toupper(name[1]))   {
    case 'E':
      return this->m_deposits;
    default:
      return this->m_data;
    }
    break;
  default:
    break;
  }
  throw std::runtime_error("Invalid segment name: "+name);
}
