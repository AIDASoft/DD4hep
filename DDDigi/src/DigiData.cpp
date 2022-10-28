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
  this->key = 0;
  this->set_mask(Key::mask_type(0xFFFF&mask));
  this->set_item(detail::hash32(name));
  std::lock_guard<std::mutex> lock(_k.lock);
  _k.map[this->key] = name;
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
  std::map<unsigned long, std::string>::const_iterator it = _k.map.find(k.value());
  if ( it != _k.map.end() ) return it->second;
  Key kk;
  kk.set_item(~0x0);
  for( const auto& e : _k.map )   {
    if ( (e.first & kk.value()) == k.values.item )
      return e.second;
  }
  return "UNKNOWN";
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
  data.reserve(data.size()+updates.size());
  for( auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositVector::merge(DepositMapping&& updates)    {
  std::size_t update_size = updates.size();
  data.reserve(data.size()+updates.size());
  for( const auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositVector::insert(const DepositVector& updates)    {
  std::size_t update_size = updates.size();
  data.reserve(data.size()+updates.size());
  for( const auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
}

/// Merge new deposit map onto existing map (keep inputs)
std::size_t DepositVector::insert(const DepositMapping& updates)    {
  std::size_t update_size = updates.size();
  data.reserve(data.size()+updates.size());
  for( const auto& c : updates )    {
    data.emplace_back(c);
  }
  return update_size;
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
#if defined(__GNUC__) && (__GNUC__ >= 10)
  for( ParticleMapping::value_type& c : updates )    {
    Particle part(std::move(c.second));
    this->push(Key(c.first), std::move(part));
  }
#endif
  return update_size;
}

void ParticleMapping::push(Key particle_key, Particle&& particle_data)  {
#if defined(__GNUC__) && (__GNUC__ < 10)
  /// Lower compiler version have a bad implementation of std::any
  bool ret = false;
#else
  bool ret = data.emplace(particle_key, std::move(particle_data)).second;
#endif
  if ( !ret )   {
    except("ParticleMapping","Error in particle map. Duplicate ID: mask:%04X Number:%d History:%s",
	   particle_key.mask(), particle_key.item(), yes_no(particle_data.history.has_value()));
  }
}

void ParticleMapping::insert(Key particle_key, const Particle& particle_data)  {
  bool ret = data.emplace(particle_key, particle_data).second;
  if ( !ret )   {
    except("ParticleMapping","Error in particle map. Duplicate ID: mask:%04X Number:%d History:%s",
	   particle_key.mask(), particle_key.item(), yes_no(particle_data.history.has_value()));
  }
}

/// Insert new entry
void ParticleMapping::emplace(Key particle_key, Particle&& particle_data)  {
#if defined(__GNUC__) && (__GNUC__ < 10)
  //return std::make_pair(false);
#else
  data.emplace(particle_key, std::move(particle_data)).second;
#endif
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
  data.insert(data.end(), updates.data.begin(), updates.data.end());
  return len;
}

/// Merge new deposit map onto existing map (not thread safe!)
std::size_t DetectorHistory::insert(const DetectorHistory& updates)   {
  std::size_t len = updates.size();
  data.insert(data.end(), updates.data.begin(), updates.data.end());
  return len;
}

/// Initializing constructor
DataSegment::DataSegment(std::mutex& l, int i) : lock(l), id(i)
{
}

/// Remove data item from segment
bool DataSegment::emplace(Key key, std::any&& item)    {
  std::lock_guard<std::mutex> l(lock);
#if defined(__GNUC__) && (__GNUC__ < 10)
  /// Lower compiler version have a bad implementation of std::any
  bool ret = false;
#else
  bool ret = data.emplace(key, std::move(item)).second;
#endif
  if ( !ret )   {
    Key k(key);
    except("DataSegment","Error in DataSegment map. Duplicate ID: mask:%04X Number:%d Value:%s",
	   k.mask(), k.item(), yes_no(item.has_value()));
  }
  return ret;
}

/// Move data items other than std::any to the data segment
template <typename DATA> bool DataSegment::put(Key key, DATA&& value)   {
  std::any item = std::make_any<DATA>(std::move(value));
  return this->emplace(key, std::move(item));
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
  for(auto key : keys)   {
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
    Key k(e.first);
    printout(INFO, "DataSegment", "Key No.%4: %16lX <> %16lX -> %04X %10ld",
	     count, e.first, k.value(), k.mask(), k.item(),
	     typeName(e.second.type()).c_str());
    ++count;
  }
}

/// Call on failed any-casts during data requests
std::string DataSegment::invalid_cast(Key key, const std::type_info& type)  const   {
  return dd4hep::format(0, "Invalid segment data cast. Key:%ld type:%s",
			key.value(), typeName(type).c_str());
}

/// Call on failed data requests during data requests
std::string DataSegment::invalid_request(Key key)  const   {
  return dd4hep::format(0, "Invalid segment data requested. Key:%ld",key.value());
}

/// Access data item by key
std::any* DataSegment::get_item(Key key, bool exc)   {
  auto it = this->data.find(key);
  if (it != this->data.end()) return &it->second;
  if ( exc ) throw std::runtime_error(invalid_request(key));
  return nullptr;
}

/// Access data item by key  (CONST)
const std::any* DataSegment::get_item(Key key, bool exc)  const   {
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

DataSegment& DigiEvent::access_segment(std::unique_ptr<DataSegment>& segment, uint32_t id)   {
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
      return access_segment(m_data,0xFEED);
    }
    break;
  case 'O':
    return access_segment(m_outputs,4);
  default:
    break;
  }
  throw std::runtime_error("Invalid segment name: "+name);
}
