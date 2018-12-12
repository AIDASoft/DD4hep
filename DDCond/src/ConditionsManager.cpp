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
#include "DD4hep/Detector.h"
#include "DD4hep/Errors.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/PluginCreators.h"

#include "DD4hep/ConditionsListener.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsManagerObject.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsManagerObject);


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  
  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /// Access specialization
    template <> ConditionsManager ConditionsManager::from<Detector>(Detector& host)  {
      Object* obj = host.extension<Object>();
      if ( obj ) return ConditionsManager(obj);
      except("ConditionsManager","+++ Failed to access installed manager from Detector.");
      return ConditionsManager();
    }
  }
}

/// Default constructor
ConditionsManagerObject::ConditionsManagerObject(Detector& ref_description)
  : NamedObject(), m_detDesc(ref_description)
{
  InstanceCount::increment(this);
  declareProperty("LoadConditions",           m_doLoad);
  declareProperty("OutputUnloadedConditions", m_doOutputUnloaded);
}

/// Default destructor
ConditionsManagerObject::~ConditionsManagerObject()   {
  m_onRegister.clear();
  m_onRemove.clear();
  InstanceCount::decrement(this);
}

void ConditionsManagerObject::registerCallee(Listeners& listeners, const Listener& callee, bool add)  {
  if ( add )  {
    listeners.insert(callee);
    return;
  }
  Listeners::iterator i=listeners.find(callee);
  if ( i != listeners.end() ) listeners.erase(i);  
}

/// (Un)Registration of conditions listeners with callback when a new condition is registered
void ConditionsManagerObject::callOnRegister(const Listener& callee, bool add)  {
  registerCallee(m_onRegister, callee, add);
}

/// (Un)Registration of conditions listeners with callback when a condition is de-registered
void ConditionsManagerObject::callOnRemove(const Listener& callee, bool add)  {
  registerCallee(m_onRemove, callee, add);
}

/// Call this when a condition is registered to the cache
void ConditionsManagerObject::onRegister(Condition condition)    {
  for(const auto& listener : m_onRegister )
    listener.first->onRegisterCondition(condition, listener.second);
}

/// Call this when a condition is deregistered from the cache
void ConditionsManagerObject::onRemove(Condition condition)   {
  for(const auto& listener : m_onRemove )
    listener.first->onRegisterCondition(condition, listener.second);
}

/// Access the used/registered IOV types
const vector<const IOVType*> ConditionsManagerObject::iovTypesUsed() const   {
  vector<const IOVType*> result;
  const auto& types = this->iovTypes();
  for ( const auto& i : types )  {
    if ( int(i.type) != IOVType::UNKNOWN_IOV ) result.push_back(&i);
  }
  return result;
}

/// Create IOV from string
void ConditionsManagerObject::fromString(const std::string& data, IOV& iov)   {
  size_t id1 = data.find(',');
  size_t id2 = data.find('#');
  if ( id2 == string::npos )  {
    except("ConditionsManager","+++ Unknown IOV string representation: %s",data.c_str());
  }
  string iov_name = data.substr(id2+1);
  IOV::Key key;
  int nents = 0;
  if ( id1 != string::npos )
    nents = ::sscanf(data.c_str(),"%ld,%ld#",&key.first,&key.second) == 2 ? 2 : 0;
  else  {
    nents = ::sscanf(data.c_str(),"%ld#",&key.first) == 1 ? 1 : 0;
    key.second = key.first;
  }
  if ( nents == 0 )   {
    except("ConditionsManager",
           "+++ Failed to read keys from IOV string representation: %s",data.c_str());
  }

  // Check if this IOV type is known
  const IOVType* typ = iovType(iov_name);
  if ( !typ )  {
    // Severe: We have an unknown IOV type. This is not allowed, 
    // because we do not known hot to handle it.....
    except("ConditionsManager","+++ Unknown IOV type requested from data: %s. [%s]",
           data.c_str(),Errors::invalidArg().c_str());
  }
  iov.type    = typ->type;
  iov.iovType = typ;
  iov.set(key);
}

/// Register IOV using new string data
ConditionsPool* ConditionsManagerObject::registerIOV(const string& data)   {
  IOV iov(0);
  // Convert string to IOV
  fromString(data, iov);
  // IOV read and checked. Now register it.
  // The validity of iov->iovType is already ensured in 'fromString'
  return registerIOV(*iov.iovType, iov.keyData);
}

/// Initializing constructor to create a named manager
ConditionsManager::ConditionsManager(Detector& description, const std::string& factory)   {
  ConditionsManagerObject* obj = createPlugin<ConditionsManagerObject>(factory,description);
  if ( !obj )  {
    except("ConditionsManagerInstaller","Failed to create manager object of type %s",
           factory.c_str());
  }
  assign(obj, "ConditionsManager",factory);
}

/// Default constructor
ConditionsManager::ConditionsManager(Detector& description)  {
  assign(ConditionsManager::from(description).ptr(), "ConditionsManager","");
}

ConditionsManager& ConditionsManager::initialize()   {
  access()->initialize();
  return *this;
}

/// Access the detector description
Detector& ConditionsManager::detectorDescription()  const   {
  return access()->detectorDescription();
}

/// Access to the property manager
PropertyManager& ConditionsManager::properties()  const   {
  return access()->properties();
}

/// Access to properties
Property& ConditionsManager::operator[](const std::string& property_name) const    {
  return access()->properties().property(property_name);
}

/// Access the conditions loader
ConditionsDataLoader& ConditionsManager::loader()  const    {
  return *(access()->loader());
}

/// Register new IOV type if it does not (yet) exist.
pair<bool, const IOVType*> 
ConditionsManager::registerIOVType(size_t iov_type, const string& iov_name)  const  {
  return access()->registerIOVType(iov_type, iov_name);
}

/// Access IOV by its name
const IOVType* ConditionsManager::iovType (const string& iov_name) const   {
  return access()->iovType(iov_name);
}

/// Access conditions multi IOV pool by iov type
ConditionsIOVPool* ConditionsManager::iovPool(const IOVType& iov_type)  const {
  return access()->iovPool(iov_type);
}

/// Access the used/registered IOV types
const vector<const IOVType*> ConditionsManager::iovTypesUsed() const  {
  Object* obj = access();
  vector<const IOVType*> result;
  const auto& types = obj->iovTypes();
  for(const auto& i : types )
    if ( int(i.type) != IOVType::UNKNOWN_IOV ) result.push_back(&i);
  return result;
}

/// Register IOV with type and key
ConditionsPool* ConditionsManager::registerIOV(const string& iov_rep)  const   {
  IOV iov(0);
  Object* o = access();
  o->fromString(iov_rep, iov);
  if ( iov.iovType )
    return o->registerIOV(*iov.iovType, iov.key());
  except("ConditionsManager","Invalid IOV type registration requested by IOV:%s",iov_rep.c_str());
  return 0;
}
      
/// Register IOV with type and key
ConditionsPool* ConditionsManager::registerIOV(const IOV& iov) const   {
  if ( iov.iovType )
    return access()->registerIOV(*iov.iovType, iov.key());
  except("ConditionsManager","+++ Attempt to register invalid IOV [FAILED]");
  return 0;
}

/// Register IOV with type and key
ConditionsPool* ConditionsManager::registerIOV(const IOVType& typ, IOV::Key key)   const {
  return access()->registerIOV(typ, key);
}

/// Create IOV from string
void ConditionsManager::fromString(const string& iov_str, IOV& iov)  const  {
  access()->fromString(iov_str, iov);
}
      
/// Register a whole block of conditions with identical IOV.
size_t ConditionsManager::blockRegister(ConditionsPool& pool, const std::vector<Condition>& cond) const   {
  return access()->blockRegister(pool, cond);
}

/// Register new condition with the conditions store. Unlocked version, not multi-threaded
bool ConditionsManager::registerUnlocked(ConditionsPool& pool, Condition cond)  const  {
  return access()->registerUnlocked(pool, cond);
}

/// Push all pending updates to the conditions store. 
void ConditionsManager::pushUpdates()  const {
}

/// Adopt cleanup handler. If a handler is registered, it is invoked at every "prepare" step
void ConditionsManager::adoptCleanup(ConditionsCleanup* cleaner)  const   {
  access()->adoptCleanup(cleaner);
}

/// Clean conditions, which are above the age limit.
void ConditionsManager::clean(const IOVType* typ, int max_age)  const  {
  access()->clean(typ, max_age);
}

/// Invoke cache cleanup with user defined policy
void ConditionsManager::clean(const ConditionsCleanup& cleaner)  const   {
  access()->clean(cleaner);  
}

/// Full cleanup of all managed conditions.
void ConditionsManager::clear()  const  {
  access()->clear();
}

/// Create empty user pool object
std::unique_ptr<UserPool> ConditionsManager::createUserPool(const IOVType* iovT)  const   {
  return access()->createUserPool(iovT);
}

/// Prepare all updates to the clients with the defined IOV
ConditionsManager::Result
ConditionsManager::prepare(const IOV& req_iov, ConditionsSlice& slice, ConditionUpdateUserContext* ctx)  const  {
  return access()->prepare(req_iov, slice, ctx);
}

/// Load all updates to the clients with the defined IOV (1rst step of prepare)
ConditionsManager::Result
ConditionsManager::load(const IOV& req_iov, ConditionsSlice& slice, ConditionUpdateUserContext* ctx)  const  {
  return access()->load(req_iov, slice, ctx);
}

/// Compute all derived conditions with the defined IOV (2nd step of prepare)
ConditionsManager::Result
ConditionsManager::compute(const IOV& req_iov, ConditionsSlice& slice, ConditionUpdateUserContext* ctx)  const  {
  return access()->compute(req_iov, slice, ctx);
}
