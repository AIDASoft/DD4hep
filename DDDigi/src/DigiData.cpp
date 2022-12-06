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

std::string dd4hep::digi::digiTypeName(const std::type_info& info)    {
  using detail::str_replace;
  std::string typ = typeName(info);
  std::size_t idx = typ.find(", std::allocator<std::");
  if ( idx != std::string::npos ) typ = str_replace(typ, typ.substr(idx), ">");
  typ = str_replace(str_replace(typ,"std::",""),"dd4hep::","");
  typ = str_replace(str_replace(typ,"sim::",""),"digi::","");
  return typ;
}

std::string dd4hep::digi::digiTypeName(const std::any& data)    {
  return digiTypeName(data.type());
}

Key& Key::set(const std::string& name, int mask)    {
  auto& _k = keys();
  if ( name.empty() )   {
    std::lock_guard<std::mutex> lock(_k.lock);
    except("DDDigi::Key", "+++ No key name was specified  --  this is illegal!");
  }
  this->key = 0;
  this->set_mask(Key::mask_type(0xFFFF&mask));
  this->set_item(detail::hash32(name));
  std::lock_guard<std::mutex> lock(_k.lock);
  _k.map[this->item()] = name;
  return *this;
}

/// Set key submask
Key& Key::set_submask(const char* opt_tag)   {
  submask_type sm = detail::hash16(opt_tag);
  this->values.submask = sm;
  return *this;
}

/// Access key name (if registered properly)
std::string Key::key_name(const Key& k)    {
  auto& _k = keys();
  std::lock_guard<std::mutex> lock(_k.lock);
  std::map<unsigned long, std::string>::const_iterator it = _k.map.find(k.item());
  if ( it != _k.map.end() ) return it->second;
  for( const auto& e : _k.map )   {
    if ( e.first == k.values.item )
      return e.second;
  }
  return "UNKNOWN";
}

const Particle& dd4hep::digi::get_history_particle(const DigiEvent& event, Key history_key)   {
  static Key item_key("MCParticles",0x0);
  Key key;
  const auto& segment = event.get_segment(history_key.segment());
  key.set_segment(history_key.segment());
  key.set_mask(history_key.mask());
  key.set_item(item_key.item());
  const auto& particles = segment.get<ParticleMapping>(key);
  return particles.get(history_key);
}

const EnergyDeposit& dd4hep::digi::get_history_deposit(const DigiEvent& event, Key::itemkey_type container_item, Key history_key)   {
  Key key;
  const auto& segment = event.get_segment(history_key.segment());
  key.set_segment(history_key.segment());
  key.set_item(container_item);
  key.set_mask(history_key.mask());
  const auto& hits = segment.get<DepositVector>(key);
  return hits.at(history_key.item());
}

/// Update history
void History::update(const History& upda)   {
  hits.insert(hits.end(), upda.hits.begin(), upda.hits.end());
  particles.insert(particles.end(), upda.particles.begin(), upda.particles.end());
}

/// Drop history information
std::pair<std::size_t,std::size_t> History::drop()   {
  std::pair<std::size_t,std::size_t> ret(hits.size(), particles.size());
  hits.clear();
  particles.clear();
  return ret;
}

const Particle& History::hist_entry_t::get_particle(const DigiEvent& event)  const  {
  static Key item_key("MCParticles",0x0);
  Key key(this->source);
  const auto& segment = event.get_segment(this->source.segment());
  const auto& particles = segment.get<ParticleMapping>(key.set_item(item_key.item()));
  return particles.get(this->source);
}

const EnergyDeposit& History::hist_entry_t::get_deposit(const DigiEvent& event, Key::itemkey_type container_item)  const {
  Key key(this->source);
  const auto& segment = event.get_segment(this->source.segment());
  const auto& hits = segment.get<DepositVector>(key.set_item(container_item));
  return hits.at(this->source.item());
}

/// Retrieve the weighted momentum of all contributing particles
Direction History::average_particle_momentum(const DigiEvent& event)  const   {
  Direction momentum;
  if ( !particles.empty() )    {
    double weight = 0e0;
    for( const auto& p : particles )   {
      const Particle&  par = p.get_particle(event);
      momentum += par.momentum * p.weight;
      weight   += p.weight;
    }
    momentum /= std::max(weight, detail::numeric_epsilon);
  }
  return momentum;
}

/// Update the deposit using deposit weighting
void EnergyDeposit::update_deposit_weighted(const EnergyDeposit& upda)  {
  double    sum = deposit + upda.deposit;
  Position  pos = ((deposit / sum) * position) + ((upda.deposit / sum) * upda.position);
  Direction mom = ((deposit / sum) * momentum) + ((upda.deposit / sum) * upda.momentum);
  position = pos;
  momentum = mom;
  deposit  = sum;
  history.update(upda.history);
}

/// Update the deposit using deposit weighting
void EnergyDeposit::update_deposit_weighted(EnergyDeposit&& upda)  {
  double    sum = deposit + upda.deposit;
  Position  pos = ((deposit / sum) * position) + ((upda.deposit / sum) * upda.position);
  Direction mom = ((deposit / sum) * momentum) + ((upda.deposit / sum) * upda.momentum);
  position = pos;
  momentum = mom;
  deposit  = sum;
  history.update(upda.history);
}

/// Merge new deposit map onto existing map
std::size_t DepositVector::merge(DepositVector&& updates)    {
  std::size_t update_size = updates.size();
  std::size_t newlen = std::max(2*data.size(), data.size()+updates.size());
  data.reserve(newlen);
  for( auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositVector::merge(DepositMapping&& updates)    {
  std::size_t update_size = updates.size();
  std::size_t newlen = std::max(2*data.size(), data.size()+updates.size());
  data.reserve(newlen);
  for( const auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositVector::insert(const DepositVector& updates)    {
  std::size_t update_size = updates.size();
  std::size_t newlen = std::max(2*data.size(), data.size()+updates.size());
  data.reserve(newlen);
  for( const auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositVector::insert(const DepositMapping& updates)    {
  std::size_t update_size = updates.size();
  std::size_t newlen = std::max(2*data.size(), data.size()+updates.size());
  data.reserve(newlen);
  for( const auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Access energy deposit by key
const EnergyDeposit& DepositVector::get(CellID cell)   const    {
  for( const auto& c : data )    {
    if ( c.first == cell )   {
      return c.second;
    }
  }
  except("DepositVector","Failed to access deposit by CellID. UNKNOWN ID: %016X", cell);
  throw std::runtime_error("Attempt to access non-existing CellID");
}

/// Access energy deposit by key
const EnergyDeposit& DepositVector::at(std::size_t cell)   const    {
  return data.at(cell).second;
}

/// Remove entry
void DepositVector::remove(iterator /* position */)   {
  //data.erase(position);
}

/// Merge new deposit map onto existing map
std::size_t DepositMapping::merge(DepositVector&& updates)    {
  std::size_t update_size = updates.size();
  for( auto& dep : updates )    {
    EnergyDeposit& depo = dep.second;
    CellID cell = dep.first;
    auto   iter = data.find(cell);
    if ( iter == data.end() )
      data.emplace(dep.first, std::move(depo));
    else
      iter->second.update_deposit_weighted(std::move(depo));
  }
  return update_size;
}

/// Merge new deposit map onto existing map
std::size_t DepositMapping::merge(DepositMapping&& updates)    {
  std::size_t update_size = updates.size();
  for( auto& dep : updates )    {
    const EnergyDeposit& depo = dep.second;
    CellID cell = dep.first;
    auto   iter = data.find(cell);
    if ( iter == data.end() )
      data.emplace(dep.first, std::move(depo));
    else
      iter->second.update_deposit_weighted(std::move(depo));
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositMapping::insert(const DepositVector& updates)    {
  std::size_t update_size = updates.size();
  for( const auto& c : updates )    {
    data.emplace(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositMapping::insert(const DepositMapping& updates)    {
  std::size_t update_size = updates.size();
  for( const auto& c : updates )    {
    data.emplace(c);
  }
  return update_size;
}

/// Emplace entry
void DepositMapping::emplace(CellID cell, EnergyDeposit&& deposit)    {
  data.emplace(cell, std::move(deposit));
}

/// Access energy deposit by key
const EnergyDeposit& DepositMapping::get(CellID cell)   const    {
  auto   iter = data.find(cell);
  if ( iter != data.end() )
    return iter->second;
  except("DepositMapping","Failed to access deposit by CellID. UNKNOWN ID: %016X", cell);
  throw std::runtime_error("Failed to access deposit by CellID");
}

/// Remove entry
void DepositMapping::remove(iterator position)   {
  data.erase(position);
}

/// Merge new deposit map onto existing map
std::size_t ParticleMapping::insert(const ParticleMapping& updates)    {
  std::size_t update_size = updates.size();
  for( const ParticleMapping::value_type& c : updates )
    this->insert(Key(c.first), c.second);
  return update_size;
}

/// Merge new deposit map onto existing map
std::size_t ParticleMapping::merge(ParticleMapping&& updates)    {
  std::size_t update_size = updates.size();
  for( ParticleMapping::value_type& c : updates )    {
    Particle part(std::move(c.second));
    this->push(Key(c.first), std::move(part));
  }
  return update_size;
}

void ParticleMapping::push(Key particle_key, Particle&& particle_data)  {
  bool ret = data.emplace(particle_key, std::move(particle_data)).second;
  if ( !ret )   {
    except("ParticleMapping","Error in particle map. Duplicate ID: mask:%04X Number:%d History:%s",
	   particle_key.mask(), particle_key.item(), yes_no(1/*particle_data.history.has_value()*/));
  }
}

void ParticleMapping::insert(Key particle_key, const Particle& particle_data)  {
  bool ret = data.emplace(particle_key, particle_data).second;
  if ( !ret )   {
    except("ParticleMapping","Error in particle map. Duplicate ID: mask:%04X Number:%d History:%s",
	   particle_key.mask(), particle_key.item(), yes_no(1/*particle_data.history.has_value()*/));
  }
}

/// Insert new entry
void ParticleMapping::emplace(Key particle_key, Particle&& particle_data)  {
  data.emplace(particle_key, std::move(particle_data)).second;
}

/// Access particle by key
const Particle& ParticleMapping::get(Key particle_key)   const    {
  auto iter = data.find(particle_key);
  if ( iter != data.end() )
    return iter->second;
  except("ParticleMapping","Error in particle map. UNKNOWN ID: %016X segment:%04X mask:%04X Number:%d",
	 particle_key.value(), particle_key.segment(), particle_key.mask(), particle_key.item());
  return iter->second;
}

/// Merge new deposit map onto existing map (not thread safe!)
std::size_t DetectorResponse::merge(DetectorResponse&& updates)   {
  std::size_t len = updates.size();
  data.insert(data.end(), updates.data.begin(), updates.data.end());
  return len;
}

/// Merge new deposit map onto existing map (not thread safe!)
std::size_t DetectorResponse::insert(const DetectorResponse& updates)   {
  std::size_t len = updates.size();
  data.insert(data.end(), updates.data.begin(), updates.data.end());
  return len;
}

/// Merge new deposit map onto existing map (not thread safe!)
std::size_t DetectorHistory::merge(DetectorHistory&& updates)   {
  std::size_t len = updates.size();
  if ( data.empty() )   {
    data = std::move(updates.data);
  }
  else   {
    for(auto& e : updates.data)
      data.emplace(data.end(), std::move(e));
  }
  return len;
}

/// Merge new deposit map onto existing map (not thread safe!)
std::size_t DetectorHistory::insert(const DetectorHistory& updates)   {
  std::size_t len = updates.size();
  data.insert(data.end(), updates.data.begin(), updates.data.end());
  return len;
}

/// Initializing constructor
DataSegment::DataSegment(std::mutex& l, Key::segment_type i)
  : data(), lock(l), id(i)
{
}

/// Remove data item from segment
bool DataSegment::emplace_any(Key key, std::any&& item)    {
  bool has_value = item.has_value();
#if DD4HEP_DDDIGI_DEBUG
  printout(INFO, "DataSegment", "PUT Key No.%4d: %-32s %016lX -> %04X %04X %08Xld Value:%s  %s",
	   data.size(), Key::key_name(key).c_str(), key.value(), key.segment(), key.mask(), key.item(),
	   yes_no(has_value), digiTypeName(item.type()).c_str());
#endif
  std::lock_guard<std::mutex> l(lock);
  bool ret = data.emplace(key, std::move(item)).second;
  if ( !ret )   {
    except("DataSegment","Error in DataSegment map. Duplicate ID: segment:%04X mask:%04X Number:%d Value:%s",
	   key.mask(), key.item(), yes_no(has_value));
  }
  return ret;
}

/// Move data items other than std::any to the data segment
template <typename DATA> bool DataSegment::put(Key key, DATA&& value)   {
  key.set_segment(this->id);
  value.key.set_segment(this->id);
  std::any item = std::make_any<DATA>(std::move(value));
  return this->emplace_any(key, std::move(item));
}

template bool DataSegment::put(Key key, DepositVector&& data);
template bool DataSegment::put(Key key, DepositMapping&& data);
template bool DataSegment::put(Key key, ParticleMapping&& data);
template bool DataSegment::put(Key key, DetectorHistory&& data);
template bool DataSegment::put(Key key, DetectorResponse&& data);

/// Remove data item from segment
bool DataSegment::erase(Key key)    {
  std::lock_guard<std::mutex> l(lock);
  auto iter = data.find(key);
  if ( iter != data.end() )   {
    data.erase(iter);
    return true;
  }
  return false;
}

/// Remove data items from segment (locked)
std::size_t DataSegment::erase(const std::vector<Key>& keys)   {
  std::size_t count = 0;
  std::lock_guard<std::mutex> l(lock);
  for(const auto& key : keys)   {
    auto iter = data.find(key);
    if ( iter != data.end() )   {
      data.erase(iter);
      ++count;
    }
  }
  return count;
}

/// Print segment keys
void DataSegment::print_keys()   const   {
  size_t count = 0;
  for( const auto& e : this->data )   {
    Key key(e.first);
    printout(INFO, "DataSegment", "Key No.%4d: %-32s %016lX -> %04X %04X %08Xld   %s",
	     count, Key::key_name(key).c_str(), key.value(), key.segment(), key.mask(), key.item(),
	     digiTypeName(e.second.type()).c_str());
    ++count;
  }
}

/// Call on failed any-casts during data requests
std::string DataSegment::invalid_cast(Key key, const std::type_info& type)  const   {
  return dd4hep::format(0, "Invalid segment data cast. Key:%-32s %016lX -> %04X %04X %08X type:%s",
			Key::key_name(key).c_str(), key.value(), 
			key.segment(), key.mask(), key.item(),
			typeName(type).c_str());
}

/// Call on failed data requests during data requests
std::string DataSegment::invalid_request(Key key)  const   {
  return dd4hep::format(0, "Invalid segment data requested. Key:%ld",key.value());
}

/// Access data item by key
std::any* DataSegment::get_item(Key key, bool exc)   {
  auto it = this->data.find(key);
  if (it != this->data.end()) return &it->second;
  key.set_segment(0x0);
  it = this->data.find(key);
  if (it != this->data.end()) return &it->second;

  if ( exc ) throw std::runtime_error(invalid_request(key));
  return nullptr;
}

/// Access data item by key  (CONST)
const std::any* DataSegment::get_item(Key key, bool exc)  const   {
  auto it = this->data.find(key);
  if (it != this->data.end()) return &it->second;
  key.set_segment(0x0);
  it = this->data.find(key);
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

DataSegment& DigiEvent::access_segment(std::unique_ptr<DataSegment>& segment, Key::segment_type id)   {
  if ( segment )   {
    return *segment;
  }
  std::lock_guard<std::mutex> guard(m_lock);
  /// Check again after holding the lock:
  if ( !segment )   {
    segment = std::make_unique<DataSegment>(this->m_lock, id);
  }
  return *segment;
}

/// Retrieve data segment from the event structure
DataSegment& DigiEvent::get_segment(const std::string& name)   {
  switch(::toupper(name[0]))   {
  case 'I':
    return access_segment(m_inputs,1);
  case 'C':
    return access_segment(m_counts,2);
  case 'D':
    switch(::toupper(name[1]))   {
    case 'E':
      return access_segment(m_deposits,3);
    default:
      return access_segment(m_data,0xAB);
    }
    break;
  case 'O':
    return access_segment(m_outputs,4);
  default:
    break;
  }
  except("DigiEvent", "Invalid segment name: %s", name.c_str());
  throw std::runtime_error("Invalid segment name");
}

/// Retrieve data segment from the event structure
const DataSegment& DigiEvent::get_segment(const std::string& name)   const  {
  switch(::toupper(name[0]))   {
  case 'I':
    return *m_inputs;
  case 'C':
    return *m_counts;
  case 'D':
    switch(::toupper(name[1]))   {
    case 'E':
      return *m_deposits;
    default:
      return *m_data;
    }
    break;
  case 'O':
    return *m_outputs;
  default:
    break;
  }
  except("DigiEvent", "Invalid segment name: %s", name.c_str());
  throw std::runtime_error("Invalid segment name");
}

/// Retrieve data segment from the event structure by identifier
DataSegment& DigiEvent::get_segment(Key::segment_type id)   {
  switch(id)   {
  case 1:
    return access_segment(m_inputs,1);
  case 2:
    return access_segment(m_counts,2);
  case 3:
    return access_segment(m_deposits,3);
  case 4:
    return access_segment(m_outputs,4);
  case 0xAB:
    return access_segment(m_data,0xAB);
  default:
    break;
  }
  except("DigiEvent", "Invalid segment identifier: %d", id);
  throw std::runtime_error("Invalid segment name");
}

/// Retrieve data segment from the event structure by identifier
const DataSegment& DigiEvent::get_segment(Key::segment_type id)  const  {
  switch(id)   {
  case 1:
    return *m_inputs;
  case 2:
    return *m_counts;
  case 3:
    return *m_deposits;
  case 4:
    return *m_outputs;
  case 0xAB:
    return *m_data;
  default:
    break;
  }
  except("DigiEvent", "Invalid segment identifier: %d", id);
  throw std::runtime_error("Invalid segment name");
}

