//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Errors.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/World.h"
#include "DD4hep/Handle.inl"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/objects/AlignmentsInterna.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsEntry.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsListener.h"
#include "DDCond/ConditionsLoaderImp.h"
#include "DDCond/AlignmentsLoaderImp.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;

typedef Alignments::AlignmentsLoader AlignmentsLoader;
typedef UpdatePool::UpdateEntries Updates;
typedef RangeConditions RC;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsManagerObject);

#define NO_AGE 0

namespace {
  struct Range {};
  struct Discrete {};

  int s_debug = INFO;

  /// Helper: IOV Check function declaration
  template <typename T> const IOVType* check_iov_type(const ConditionsManagerObject* o, const IOV* iov);

  /// Helper: Specialized IOV check
  template <> const IOVType* check_iov_type<void>(const ConditionsManagerObject* o, const IOV* iov)   {
    if ( iov )  {
      const IOVType* typ = iov->iovType ? iov->iovType : o->iovType(iov->type);
      if ( typ )  {
        if ( iov->type == typ->type )  {
          if ( typ->type < o->m_rawPool.size() )  {
            if ( o->m_rawPool[typ->type] != 0 )  {
              return typ;
            }
          }
        }
      }
    }
    return 0;
  }

  /// Helper: Specialized IOV check for discrete IOV values
  template <> const IOVType* check_iov_type<Discrete>(const ConditionsManagerObject* o, const IOV* iov)   {
    const IOVType* typ = check_iov_type<void>(o,iov);
    if ( typ && !iov->has_range() ) return typ;
    return 0;
  }
  /// Helper: Specialized IOV check for range IOV values
  template <> const IOVType* check_iov_type<Range>(const ConditionsManagerObject* o, const IOV* iov)   {
    const IOVType* typ = check_iov_type<void>(o,iov);
    if ( typ && iov->has_range() ) return typ;
    return 0;
  }

  /// Helper: Check conditions result for consistency
  template <typename T> void __check_values__(const ConditionsManagerObject* o, Condition::key_type key, const IOV* iov)  
  {
    if ( !iov )  {
      except("ConditionsManager","+++ Invalid IOV to access condition: %08X. [Null-reference]",key);
    }
    const IOVType* typ = check_iov_type<T>(o,iov);
    if ( !typ )  {
      // Severe: We have an unknown IOV type. This is not allowed, 
      // because we do not known hot to handle it.....
      except("ConditionsManager","+++ Invalid IOV type [%d] to access condition: %08X.",
             iov->type, key);
    }
  }

  /// Helper: Check if the conditions range covers the entire IOV span
  bool is_range_complete(const IOV& iov, const RC& conditions)  {
    if ( !conditions.empty() )  {
      // We need to check if the entire range is covered.
      // For every key.second we must find a key.first, which is at least as big
      IOV::Key test=iov.keyData;
      // The range may be returned unordered. Hence, 
      // we have to try to match at most conditions.size() times until we really know
      for(size_t j = 0; j < conditions.size(); ++j )  {
        for(RC::const_iterator i=conditions.begin(); i!=conditions.end(); ++i)  {
          const IOV::Key& k = (*i)->iov->key();
          if ( k.first   <= test.first+1 && k.second >= test.first  ) test.first = k.second;
          if ( k.first+1 <= test.second  && k.second >= test.second ) test.second = k.first;
          //printout(INFO,"Test","IOV: %ld,%ld --> %ld,%ld",k.first,k.second, test.first, test.second);
          if ( test.first >= test.second ) return true;
        }
        if ( test.first <= iov.keyData.first && test.second >= iov.keyData.second ) return false;
      }
    }
    return false;
  }

  template <typename PMF>
  void __callListeners(const ConditionsManagerObject::Listeners& listeners, PMF pmf, Condition& cond)  {
    for(ConditionsManagerObject::Listeners::const_iterator i=listeners.begin(); i!=listeners.end(); ++i)  {
      const ConditionsManagerObject::Listener& listener = *i;
      (listener.first->*pmf)(cond, listener.second);
    }
  }
}

/// Standard constructor
ConditionsManagerObject::ConditionsManagerObject(LCDD& lcdd_instance)
  : NamedObject(), ObjectExtensions(typeid(ConditionsManagerObject)), m_lcdd(lcdd_instance), m_iovTypes(), m_rawPool(), 
    m_updateLock(), m_poolLock(), m_loader(),
    m_updatePool(), 
    locked(0)
{
  InstanceCount::increment(this);
  declareProperty("MaxIOVTypes",         m_maxIOVTypes=32);
  declareProperty("PoolType",            m_poolType   = "");
  declareProperty("UpdatePoolType",      m_updateType = "DD4hep_ConditionsLinearUpdatePool");
  declareProperty("UserPoolType",        m_userType   = "DD4hep_ConditionsMapUserPool");
  declareProperty("LoaderType",          m_loaderType = "multi");
  m_iovTypes.resize(m_maxIOVTypes,IOVType());
  m_rawPool.resize(m_maxIOVTypes,0);
}

/// Default destructor
ConditionsManagerObject::~ConditionsManagerObject()   {
  Geometry::World world(m_lcdd.world());
  for_each(m_rawPool.begin(), m_rawPool.end(), DestroyObject<ConditionsIOVPool*>());
  if ( world.isValid() )  {
    ConditionsLoader* cld = world->conditionsLoader;
    AlignmentsLoader* ald = world->alignmentsLoader;
    world->conditionsLoader = 0;
    world->alignmentsLoader = 0;
    if ( cld ) cld->release();
    if ( ald ) ald->release();
  }
  InstanceCount::decrement(this);
}

void ConditionsManagerObject::initialize()  {
  if ( !m_updatePool.get() )  {
    string typ = "DD4hep_Conditions_"+m_loaderType+"_Loader";
    const void* argv_loader[] = {"ConditionsDataLoader", this, 0};
    const void* argv_pool[] = {this, 0};
    m_loader.adopt(createPlugin<ConditionsDataLoader>(typ,m_lcdd,2,argv_loader));
    m_updatePool.adopt(createPlugin<UpdatePool>(m_updateType,m_lcdd,1,argv_pool));
    if ( !m_updatePool.get() )  {
      except("ConditionsManager","+++ The update pool of type %s cannot be created. [%s]",
             m_updateType.c_str(),Errors::noSys().c_str());
    }
    Ref_t ref(m_updatePool.get());
    ref->SetName("updates");
    ref->SetTitle("updates");
    Geometry::World world(m_lcdd.world());
    world->conditionsLoader = new ConditionsLoaderImp(this);
    world->alignmentsLoader = new AlignmentsLoaderImp(this);
  }
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
  __callListeners(m_onRegister, &ConditionsListener::onRegisterCondition, condition);
}

/// Call this when a condition is deregistered from the cache
void ConditionsManagerObject::onRemove(Condition condition)   {
  __callListeners(m_onRemove, &ConditionsListener::onRemoveCondition, condition);
}

/// Register new IOV type if it does not (yet) exist.
pair<bool, const IOVType*> ConditionsManagerObject::registerIOVType(size_t iov_type, const string& iov_name)   {
  if ( iov_type<m_iovTypes.size() )  {
    IOVType& t = m_iovTypes[iov_type];
    bool eq_type = t.type == iov_type;
    bool eq_name = t.name == iov_name;
    if ( eq_type && eq_name )  {
      return make_pair(false,&t);
    }
    else if ( t.type != 0 && eq_type && !eq_name )  {
      except("ConditionsManager","Cannot register IOV %s. Type %d already in use!",
             iov_name.c_str(), iov_type);
    }
    t.name = iov_name;
    t.type = iov_type;
    m_rawPool[t.type] = new ConditionsIOVPool();
    return make_pair(true,&t);    
  }
  except("ConditionsManager","Cannot register IOV section %d of type %d. Value out of bounds: [%d,%d]",
         iov_name.c_str(), iov_type, 0, int(m_iovTypes.size()));
  return make_pair(false,(IOVType*)0);
}

/// Access IOV by its type
const IOVType* ConditionsManagerObject::iovType (size_t iov_type) const  {
  if ( iov_type<m_iovTypes.size() )  {
    const IOVType& t = m_iovTypes[iov_type];
    if ( t.type == iov_type ) return &t;
  }
  except("ConditionsManager","Request to access an unregistered IOV type: %d.", iov_type);
  return 0;
}

/// Access IOV by its name
const IOVType* ConditionsManagerObject::iovType (const string& iov_name) const   {
  for( IOVTypes::const_iterator i=m_iovTypes.begin(); i != m_iovTypes.end(); ++i)
    if ( (*i).name == iov_name ) return &(*i);
  except("ConditionsManager","Request to access an unregistered IOV type: %s.", iov_name.c_str());
  return 0;
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

/// Register IOV with type and key
ConditionsPool* ConditionsManagerObject::registerIOV(const IOVType& typ, IOV::Key key)   {
  // IOV read and checked. Now register it, but always locked!
  ConditionsIOVPool* pool = m_rawPool[typ.type];
  dd4hep_lock_t lock(m_poolLock);
  if ( !pool )  {
    m_rawPool[typ.type] = pool = new ConditionsIOVPool();
  }
  ConditionsIOVPool::Elements::const_iterator i = pool->elements.find(key);
  if ( i != pool->elements.end() )   {
    return (*i).second;
  }
  const void* argv_pool[] = {this, 0};
  ConditionsPool* cond_pool = createPlugin<ConditionsPool>(m_poolType,m_lcdd,1,argv_pool);
  IOV* iov = new IOV(&typ);
  iov->type      = typ.type;
  iov->keyData   = key;
  cond_pool->iov = iov;
  pool->elements.insert(make_pair(key,cond_pool));
  return cond_pool;
}

/// Register new condition with the conditions store. Unlocked version, not multi-threaded
bool ConditionsManagerObject::registerUnlocked(ConditionsPool* pool, Condition cond)   {
  if ( pool && cond.isValid() )  {
    cond->pool = pool;
    cond->iov  = pool->iov;
    pool->insert(cond);
    __callListeners(m_onRegister, &ConditionsListener::onRegisterCondition, cond);
    return true;
  }
  else if ( !pool )
    except("ConditionsManager","+++ Unknown Conditions pool to register entry. [%s]",
           Errors::invalidArg().c_str());
  else if ( !cond.isValid() )
    except("ConditionsManager","+++ Invalid condition objects may not be registered. [%s]",
           Errors::invalidArg().c_str());
  return false;
}

/// Set a single conditions value to be managed. 
/// Requires external lock on update pool!
Condition ConditionsManagerObject::__queue_update(Conditions::Entry* e)   {
  if ( e )  {
    ConditionsPool*  p = registerIOV(e->validity);
    Condition condition(e->name,e->type);
    Condition::Object* c = condition.ptr();
    //c->name = e->name;
    //  c->type = e->type;
    c->value = e->value;
    c->comment = "----";
    c->address = "----";
    c->validity = e->validity;
    c->iov  = p->iov;
    c->pool = p;
    p->insert(c);
    if ( s_debug > INFO )  {
      printout(INFO,"Conditions","+++ Loaded condition: %s.%s to %s [%s] V: %s",
               e->detector.path().c_str(), c->name.c_str(),
	       c->value.c_str(), c->type.c_str(), c->validity.c_str());
    }
    return c;
  }
  return Condition();
}

/// Clean conditions, which are above the age limit.
int ConditionsManagerObject::clean(const IOVType* typ, int max_age)   {
  int count = 0;
  dd4hep_lock_t lock(m_updateLock);
  ConditionsIOVPool* pool = m_rawPool[typ->type];
  if ( pool )  {
    count += pool->clean(max_age);
  }
  return count;
}

/// Full cleanup of all managed conditions.
pair<int,int> ConditionsManagerObject::clear()   {
  pair<int,int> count(0,0);
  for( TypedConditionPool::iterator i=m_rawPool.begin(); i != m_rawPool.end(); ++i)  {
    ConditionsIOVPool* p = *i;
    if ( p )  {
      ++count.first;
      count.second += p->clean(0);
    }
  }
  return count;
}

/// Push all pending updates to the conditions store
void ConditionsManagerObject::pushUpdates()   {
  Updates entries;  {
    dd4hep_lock_t lock(m_updateLock);
    m_updatePool->popEntries(entries);
  }
  // Lock global pool so that no other updates happen in the meanwhile
  // which could kill the pool's containers
  dd4hep_lock_t lock(m_poolLock);
  for(Updates::const_iterator iov_iter=entries.begin(); iov_iter!=entries.end(); ++iov_iter)  {
    typedef UpdatePool::ConditionEntries _E;
    const _E& ents = (*iov_iter).second;
    if ( !ents.empty() )  {
      for(_E::const_iterator j=ents.begin(); j != ents.end(); ++j)  {
        Condition c = *j;
        c->setFlag(Condition::ACTIVE);
        c->pool->insert(c);
      }
    }
  }
}
#if 0
/// Prepare all updates to the clients with the defined IOV
long ConditionsManagerObject::prepare(const Condition::iov_type& required_validity,
				      dd4hep_ptr<UserPool>& up)
{
  const IOVType* typ = check_iov_type<Discrete>(this, &required_validity);
  if ( typ )  {
    RC valid, expired;
    ConditionsIOVPool* pool = m_rawPool[typ->type];
    if ( 0 == up.get() || up->pool_type != ConditionsPool::USER_POOL_TYPE )  {
      const void* argv_pool[] = {this, 0};
      UserPool* cp = createPlugin<UserPool>(m_userType,m_lcdd,1,argv_pool);
      up.adopt(cp);
    }
    UserPool *user_pool = up.get();
    /// First push any pending updates and register them to pending pools...
    Condition::iov_type pool_iov(typ);
    pool_iov.reset().invert();

    pushUpdates();
    pool->select(required_validity, valid, expired, pool_iov);
    user_pool->clear();
    user_pool->insert(valid);
    long num_expired = (long)expired.size();
    if ( num_expired > 0 )  {
      m_loader->update(required_validity, expired, pool_iov);
      user_pool->insert(expired);
    }
    user_pool->setValidity(pool_iov);
    user_pool->setReqValidity(required_validity);
    return num_expired;
  }
  except("ConditionsManager","+++ Unknown IOV type requested to enable conditions. [%s]",
         Errors::invalidArg().c_str());
  return -1;
}
#endif

/// Helper to check iov and user pool and create user pool if not present
void ConditionsManagerObject::__get_checked_pool(const IOV& req_iov,
                                                 dd4hep_ptr<UserPool>& up)
{
  const IOVType* typ = check_iov_type<Discrete>(this, &req_iov);
  if ( typ )  {
    ConditionsIOVPool* pool = m_rawPool[typ->type];
    if ( 0 == up.get() )  {
      const void* argv[] = {this, pool, 0};
      UserPool* p = createPlugin<UserPool>(m_userType,m_lcdd,2,argv);
      up.adopt(p);
    }
    return;
  }
  // Invalid IOV type. Throw exception
  except("ConditionsManager","+++ Unknown IOV type requested to enable conditions. [%s]",
         Errors::invalidArg().c_str());
}

/// Prepare all updates for the given keys to the clients with the defined IOV
long ConditionsManagerObject::prepare(const IOV&            req_iov,
                                      const ConditionKeys&  keys,
                                      dd4hep_ptr<UserPool>& up)
{
  RC valid, expired;
  __get_checked_pool(req_iov, up);
  /// First push any pending updates and register them to pending pools...
  pushUpdates();
  /// Now update/fill the user pool
  return up->prepare(req_iov,keys);
}


/// Prepare all updates for the given keys to the clients with the defined IOV
long ConditionsManagerObject::prepare(const IOV&            req_iov,
                                      const ConditionKeys&  keys,
                                      dd4hep_ptr<UserPool>& up,
                                      const Dependencies&   dependencies,
                                      bool                  verify_dependencies)
{
  long num_raw_updates = prepare(req_iov, keys, up);
  if ( num_raw_updates > 0 || verify_dependencies )   {
    long num_dep_updates = up->compute(dependencies);
    return num_raw_updates+num_dep_updates;
  }
  return num_raw_updates;
}

/// Prepare all updates to the clients with the defined IOV
long ConditionsManagerObject::prepare(const Condition::iov_type& req_iov, dd4hep_ptr<UserPool>& up)   {
  RC valid, expired;
  __get_checked_pool(req_iov, up);
  /// First push any pending updates and register them to pending pools...
  pushUpdates();
  /// Now update/fill the user pool
  return up->prepare(req_iov);
}

/// Prepare all updates to the clients with the defined IOV
long ConditionsManagerObject::prepare(const Condition::iov_type& req_iov,
                                      dd4hep_ptr<UserPool>& up,
                                      const Dependencies& dependencies,
                                      bool verify_dependencies)
{
  long num_raw_updates = prepare(req_iov, up);
  if ( num_raw_updates > 0 || verify_dependencies )   {
    long num_dep_updates = up->compute(dependencies);
    return num_raw_updates+num_dep_updates;
  }
  return num_raw_updates;
}

/// Register a new managed condition
void ConditionsManagerObject::registerCondition(Condition c)    {
  dd4hep_ptr<Condition::Object> cond(c.ptr());
  if ( !cond.get() )  {
    except("ConditionsManager","+++ Failed to register invalid handle.");
  }
  if ( !locked )  {
    IOV* iov = const_cast<IOV*>(cond->iov);
    // Check arguments. This also checks the existence of the proper pool
    __check_values__<Discrete>(this,cond->hash,iov);
    ConditionsIOVPool* pool = m_rawPool[iov->type];
    { // We are now modifying the pool: need to lock any access
      dd4hep_lock_t lock(m_poolLock);
      ConditionsIOVPool::Elements::iterator it = pool->elements.find(iov->keyData);
      if ( it != pool->elements.end() )  {
        (*it).second->insert(c);
        return;
      }
      const void* argv_pool[] = {this, 0};
      ConditionsPool* cp = createPlugin<ConditionsPool>(m_poolType,m_lcdd,1,argv_pool);
      pool->elements.insert(make_pair(iov->keyData,cp));
      cp->insert(c);
      onRegister(c);
    }
    return;
  }
  except("ConditionsManager","+++ You cannot register new conditions while the conditions manager is locked!");
}

/// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
bool ConditionsManagerObject::select(Condition::key_type key,
				     const Condition::iov_type& req_validity,
                                     RangeConditions& conditions)   {
  {
    ConditionsIOVPool* p = 0;
    dd4hep_lock_t locked_action(m_poolLock);
    p = m_rawPool[req_validity.type]; // Existence already checked by caller!
    p->select(key, req_validity, conditions);
  }
  {
    dd4hep_lock_t locked_action(m_updateLock);
    m_updatePool->select_range(key, req_validity, conditions);
  }
  return !conditions.empty();
}

/// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
bool ConditionsManagerObject::select_range(Condition::key_type key,
					   const Condition::iov_type& req_validity,
					   RangeConditions& conditions)
{
  {
    ConditionsIOVPool* p = 0;
    dd4hep_lock_t locked_action(m_poolLock);
    p = m_rawPool[req_validity.type]; // Existence alread checked by caller!
    p->selectRange(key, req_validity, conditions);
  }
  {
    dd4hep_lock_t locked_action(m_updateLock);
    m_updatePool->select_range(key, req_validity, conditions);
  }
  return is_range_complete(req_validity,conditions);
}

#if 0
/// Register a new managed condition.
void ConditionsManagerObject::__register_immediate(RangeConditions& rc)    {
  // We modify the pool. For this we need a lock, 
  // since this may happen during the event processing in parallel
  dd4hep_lock_t locked_action(m_poolLock);
  for(RC::iterator i=rc.begin(); i != rc.end(); ++i)   {
    Condition c = *i;
    if ( !c->pool )   {
      except("ConditionsManager","+++ __register_immediate: Invalid pool. [Fatal, Internal or Usage Error]");
    }
    if ( s_debug > INFO )  {
      printout(INFO,"ConditionsManager","+++ __register(1): %s [%s]",
               c.name().c_str(), c->iov->str().c_str());
    }
    c->pool->insert(c);
  }
}
#endif

/// Retrieve a condition given a Detector Element and the conditions name
Condition
ConditionsManagerObject::get(Condition::key_type key, const Condition::iov_type& iov)
{
  RC conditions;
  __check_values__<Discrete>(this, key, &iov);
  bool rc = select(key, iov, conditions);
  if ( !rc )  {
    dd4hep_lock_t locked_load(m_updateLock);
    m_loader->load(key, iov, conditions);
  }
  if ( conditions.size() == 1 )   {
    conditions[0]->flags |= Condition::ACTIVE;
    return conditions[0];
  }
  else if ( conditions.empty() )   {
    except("ConditionsManager","+++ Condition %08X for the requested IOV %s do not exist.",
           key, iov.str().c_str());
  }
  else if ( conditions.size() > 1 )  {
    RC::const_iterator start = conditions.begin();
    Condition first = *start;
    printout(ERROR,"ConditionsManager","+++ Condition %s [%08X] is ambiguous for IOV %s:",
             first.name().c_str(), key, iov.str().c_str());
    for(RC::const_iterator i=start; i!=conditions.end(); ++i)  {
      Condition c = *i;
      printout(ERROR,"ConditionsManager","+++ %s [%s] = %s",
               c.name().c_str(), c->iov->str().c_str(), c->value.c_str());
    }
    except("ConditionsManager","+++ Condition %s [%08X] is ambiguous for IOV %s:",
           first.name().c_str(), key, iov.str().c_str());
  }
  return Condition();
}

/// Retrieve a condition given a Detector Element and the conditions name
RangeConditions
ConditionsManagerObject::getRange(Condition::key_type key, const Condition::iov_type& iov)
{
  RC conditions;
  __check_values__<Range>(this, key, &iov);
  bool rc = select_range(key, iov, conditions);
  if ( rc )  {
    return conditions;
  }
  else  {
    dd4hep_lock_t locked_load(m_updateLock);
    m_loader->load_range(key, iov, conditions);
    if ( conditions.empty() )  {
      except("ConditionsManager","+++ Conditions %08X for IOV %s do not exist.",
	     key, iov.str().c_str());
    }
    conditions.clear();
  }
  rc = select_range(key, iov, conditions);
  if ( !rc )  {
    except("ConditionsManager","+++ Conditions %08X for IOV %s do not exist.",
	   key, iov.str().c_str());
  }
  return conditions;
}
