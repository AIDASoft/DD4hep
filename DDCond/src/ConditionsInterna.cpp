// $Id$
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

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsEntry.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsListener.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using namespace DD4hep::Conditions::Interna;

typedef UpdatePool::UpdateEntries Updates;

DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionsManagerObject);

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
          if ( typ->type < o->m_pool.size() )  {
            if ( o->m_pool[typ->type] != 0 )  {
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
  template <typename T> void __check_values__(const ConditionsManagerObject* o, 
                                              Geometry::DetElement det,
                                              const std::string& cond,
                                              const IOV* iov)  
  {
    if ( !iov )  {
      except("ConditionsManager","+++ Invalid IOV to access condition: %s.%s. [Null-reference]",
             det.path().c_str(), cond.c_str());
    }
    const IOVType* typ = check_iov_type<T>(o,iov);
    if ( !typ )  {
      // Severe: We have an unknown IOV type. This is not allowed, 
      // because we do not known hot to handle it.....
      except("ConditionsManager","+++ Invalid IOV type [%d] to access condition: %s.%s.",
             iov->type, det.path().c_str(), cond.c_str());
    }
    if ( !det.isValid() )  {
      except("ConditionsManager","+++ Attempt to use the conditions manager %s for "
             "an unknown detector element [Invalid Handle].", cond.c_str());
    }
  }

  /// Helper: Check if the conditions range covers the entire IOV span
  bool is_range_complete(const IOV& iov, const RangeConditions& conditions)  {
    if ( !conditions.empty() )  {
      // We need to check if the entire range is covered.
      // For every key.second we must find a key.first, which is at least as big
      IOV::Key test=iov.keyData;
      // The range may be returned unordered. Hence, 
      // we have to try to match at most conditions.size() times until we really know
      for(size_t j = 0; j < conditions.size(); ++j )  {
        for(RangeConditions::const_iterator i=conditions.begin(); i!=conditions.end(); ++i)  {
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

  void __print(const char* prefix, Condition c)   {
    if ( s_debug > INFO )  {
      printout(INFO,"ConditionsManager","+++ %s %s.%s [%s] = %s",
               prefix, c->detector.path().c_str(), c.name().c_str(), 
               c->iov->str().c_str(), c->value.c_str());
    }
  }

  template <typename PMF>
  void __callListeners(const ConditionsManagerObject::Listeners& listeners, PMF pmf, Condition& cond)  {
    for(ConditionsManagerObject::Listeners::const_iterator i=listeners.begin(); i!=listeners.end(); ++i)  {
      const ConditionsManagerObject::Listener& listener = *i;
      (listener.first->*pmf)(cond, listener.second);
    }
  }

  /// Helper class to be injected into the world object to allow loading from DetElements directly.....
  struct lcdd_cond_loader : public ConditionsLoader  {
    ConditionsManager m_manager;
    lcdd_cond_loader(ConditionsManager m) : m_manager(m) {}
    virtual ~lcdd_cond_loader() {}
    /// Access the conditions loading
    virtual Condition get(Geometry::DetElement element, const std::string& key, const IOV& iov)   {
      return m_manager->get(element, key, iov);
    }
  };
}

/// Standard constructor
ConditionsManagerObject::ConditionsManagerObject(LCDD& lcdd)
  : NamedObject(), m_lcdd(lcdd), m_iovTypes(), m_pool(), 
    m_updateLock(), m_poolLock(), m_loader(),
    m_updatePool(), 
    locked(0)
{
  InstanceCount::increment(this);
  declareProperty("MaxIOVTypes",         m_maxIOVTypes=32);
  declareProperty("PoolType",            m_poolType   = "");
  declareProperty("UpdatePoolType",      m_updateType = "DD4hep_ConditionsLinearUpdatePool");
  declareProperty("UserPoolType",        m_userType   = "DD4hep_ConditionsLinearUserPool");
  declareProperty("LoaderType",          m_loaderType = "multi");
  m_iovTypes.resize(m_maxIOVTypes,IOVType());
  m_pool.resize(m_maxIOVTypes,0);
}

/// Default destructor
ConditionsManagerObject::~ConditionsManagerObject()   {
  for_each(m_pool.begin(), m_pool.end(), DestroyObject<ConditionsIOVPool*>());
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
    world->conditionsLoader = new lcdd_cond_loader(ConditionsManager(this));
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
    m_pool[t.type] = new ConditionsIOVPool();
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
  ConditionsIOVPool* pool = m_pool[typ.type];
  dd4hep_lock_t lock(m_poolLock);
  if ( !pool )  {
    m_pool[typ.type] = pool = new ConditionsIOVPool();
  }
  ConditionsIOVPool::Entries::const_iterator i = pool->entries.find(key);
  if ( i != pool->entries.end() )   {
    return (*i).second;
  }
  const void* argv_pool[] = {this, 0};
  ConditionsPool* cond_pool = createPlugin<ConditionsPool>(m_poolType,m_lcdd,1,argv_pool);
  IOV* iov = new IOV(&typ);
  iov->type      = typ.type;
  iov->keyData   = key;
  cond_pool->iov = iov;
  pool->entries.insert(make_pair(key,cond_pool));
  return cond_pool;
}

/// Register new condition with the conditions store. Unlocked version, not multi-threaded
bool ConditionsManagerObject::registerUnlocked(ConditionsPool* pool, Condition cond)   {
  if ( pool && cond.isValid() )  {
    ConditionsIOVPool* iov_pool = m_pool[pool->iov->type];
    iov_pool->addKey(cond);
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
Condition ConditionsManagerObject::__queue_update(Entry* e)   {
  if ( e )  {
    ConditionsPool*  p = registerIOV(e->validity);
    Condition c = m_updatePool->insertEntry(p, e);
    if ( s_debug > INFO )  {
      printout(INFO,"Conditions","+++ Loaded condition: %s.%s to %s [%s] V: %s",
               c.detector().path().c_str(), c.name().c_str(), c->value.c_str(),
               c->type.c_str(),c->validity.c_str());
    }
    return c;
  }
  return Condition();
}

/// Clean conditions, which are above the age limit.
int ConditionsManagerObject::clean(const IOVType* typ, int max_age)   {
  int count = 0;
  dd4hep_lock_t lock(m_updateLock);
  ConditionsIOVPool* pool = m_pool[typ->type];
  if ( pool )  {
    count += pool->clean(max_age);
  }
  return count;
}

/// Full cleanup of all managed conditions.
pair<int,int> ConditionsManagerObject::clear()   {
  pair<int,int> count(0,0);
  for( TypedConditionPool::iterator i=m_pool.begin(); i != m_pool.end(); ++i)  {
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
      for(_E::const_iterator j=ents.begin(); j != ents.end(); ++j) {
        (*j)->pool->insert(*j);
        __print("__register(2):",*j); 
      }
      //for_each(ents.begin(), ents.end(), [](_E::reference c) { c->pool->insert(c); });
      //for_each(ents.begin(), ents.end(), [](_E::reference c) { __print("__register(2):",c); });
    }
  }
}

/// Prepare all updates to the clients with the defined IOV
long ConditionsManagerObject::prepare(const IOV& required_validity, dd4hep_ptr<ConditionsPool>& up)   {
  const IOVType* typ = check_iov_type<Discrete>(this, &required_validity);
  if ( typ )  {
    RangeConditions valid, expired;
    ConditionsIOVPool* pool = m_pool[typ->type];
    if ( 0 == up.get() || up->pool_type != ConditionsPool::USER_POOL_TYPE )  {
      const void* argv_pool[] = {this, 0};
      ConditionsPool* cp = createPlugin<ConditionsPool>(m_userType,m_lcdd,1,argv_pool);
      up.adopt(cp);
    }
    UserPool *user_pool = (UserPool*)up.get();
    if ( user_pool->pool_type != UserPool::poolType() )   {
      except("ConditionsManager","+++ Unknown User POOL type:%d",
	     user_pool->pool_type, Errors::invalidArg().c_str());
    }
    /// First push any pending updates and register them to pending pools...
    IOV pool_iov(typ);
    pool_iov.reset();
    pool_iov.invert();

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
    return num_expired;
  }
  except("ConditionsManager","+++ Unknown IOV type requested to enable conditions. [%s]",
         Errors::invalidArg().c_str());
  return -1;
}

/// Enable all updates to the clients with the defined IOV
long ConditionsManagerObject::enable(const IOV& required_validity, dd4hep_ptr<ConditionsPool>& user_pool)   {
  if ( !locked )   {
    ConditionsIOVPool* pool = m_pool[required_validity.type];
    if ( pool )  {
#if 0
      UserPool* rep_pool = 0;
      pushUpdates();
      {
        dd4hep_lock_t lock(m_poolLock);
        ConditionsIOVPool::Entries::iterator i = pool->entries.find(required_validity.key());
        if ( i == pool->entries.end() )   {
          except("ConditionsManager","+++ Unknown IOV [%s] requested to enable conditions. [%s]",
                 required_validity.str().c_str(), Errors::invalidArg().c_str());
        }
        rep_pool = (*i).second->updates;
      }
      RangeConditions entries;
      rep_pool->popEntries(entries);
      __push_immediate(entries);
#endif
      return 0;
    }
    except("ConditionsManager","+++ Unknown IOV type requested to enable conditions. [%s]",
           Errors::invalidArg().c_str());
  }
  except("ConditionsManager","+++ Cannot enable new conditions for IOV:%s in locked state! [%s]",
         required_validity.str().c_str(), Errors::invalidArg().c_str());
  return 0;
}

/// Register a new managed condition
void ConditionsManagerObject::registerCondition(Condition c)    {
  dd4hep_ptr<ConditionObject> cond(c.ptr());
  if ( !cond.get() )  {
    except("ConditionsManager","+++ Failed to register invalid handle.");
  }
  if ( !locked )  {
    DetElement det = cond->detector;
    IOV* iov = const_cast<IOV*>(cond->iov);

    // Check arguments. This also checks the existence of the proper pool
    __check_values__<Discrete>(this,det,c.name(),iov);
    ConditionsIOVPool* pool = m_pool[iov->type];
    { // We are now modifying the pool: need to lock any access
      dd4hep_lock_t lock(m_poolLock);
      ConditionsIOVPool::Entries::iterator it = pool->entries.find(iov->keyData);
      if ( it != pool->entries.end() )  {
        (*it).second->insert(c);
        return;
      }
      const void* argv_pool[] = {this, 0};
      ConditionsPool* cp = createPlugin<ConditionsPool>(m_poolType,m_lcdd,1,argv_pool);
      pool->entries.insert(make_pair(iov->keyData,cp));
      pool->addKey(c);
      cp->insert(c);
      onRegister(c);
    }
    return;
  }
  except("ConditionsManager","+++ You cannot register new conditions while the conditions manager is locked!");
}

/// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
bool ConditionsManagerObject::__find(DetElement det, 
                                     const std::string& cond,
                                     const IOV& req_validity,
                                     RangeConditions& conditions)   {
  {
    ConditionsIOVPool* p = 0;
    dd4hep_lock_t locked_action(m_poolLock);
    p = m_pool[req_validity.type]; // Existence already checked by caller!
    p->__find(det, cond, req_validity, conditions);
  }
  {
    dd4hep_lock_t locked_action(m_updateLock);
    m_updatePool->select_range(det, cond, req_validity, conditions);
  }
  return !conditions.empty();
}

/// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
bool ConditionsManagerObject::__find_range(DetElement det, 
                                           const std::string& cond,
                                           const IOV& req_validity,
                                           RangeConditions& conditions)
{
  {
    ConditionsIOVPool* p = 0;
    dd4hep_lock_t locked_action(m_poolLock);
    p = m_pool[req_validity.type]; // Existence alread checked by caller!
    p->__find_range(det, cond, req_validity, conditions);
  }
  {
    dd4hep_lock_t locked_action(m_updateLock);
    m_updatePool->select_range(det, cond, req_validity, conditions);
  }
  return is_range_complete(req_validity,conditions);
}

/// Register a new managed condition.
void ConditionsManagerObject::__register_immediate(RangeConditions& rc)    {
  // We modify the pool. For this we need a lock, 
  // since this may happen during the event processing in parallel
  dd4hep_lock_t locked_action(m_poolLock);
  for(RangeConditions::iterator i=rc.begin(); i != rc.end(); ++i)   {
    Condition c = *i;
    if ( !c->pool )   {
      except("ConditionsManager","+++ __register_immediate: Invalid pool. [Fatal, Internal or Usage Error]");
    }
    if ( s_debug > INFO )  {
      printout(INFO,"ConditionsManager","+++ __register(1): %s.%s [%s]",
               c->detector.path().c_str(), c.name().c_str(), c->iov->str().c_str());
    }
    c->pool->insert(c);
  }
}

/// Push registered set of conditions to the corresponding detector elements
void ConditionsManagerObject::__push_immediate(RangeConditions& rc)    {
  for(RangeConditions::iterator i=rc.begin(); i!=rc.end(); ++i)   {
    Condition::Object* c = (*i).ptr();
    Interna::ConditionContainer* cnt = c->detector.conditions().ptr();
    if ( s_debug > INFO )  {
      printout(INFO,"ConditionsManager","Register condition %s.%s [%s] = %s to Conditions::Container....",
               c->detector.path().c_str(), c->name.c_str(), c->iov->str().c_str(), c->value.c_str());
    }
    cnt->add(c);
  }
}

/// Load  a condition set given a Detector Element and the conditions name according to their validity
bool ConditionsManagerObject::__load_immediate(DetElement det,
                                               const std::string& cond,
                                               const IOV& req_validity,
                                               RangeConditions& conditions)
{
  const IOV& iov = req_validity;
  {
    dd4hep_lock_t locked_action(m_updateLock);
    m_loader->load(det, cond, iov, conditions);
  }
  if ( conditions.size() == 0 )  {
    except("ConditionsManager","+++ Condition %s.%s for the requested IOV %s do not exist.",
           det.path().c_str(), cond.c_str(), iov.str().c_str());
  }
  __push_immediate(conditions);
  return true;
}

/// Load  a condition set given a Detector Element and the conditions name according to their validity
bool ConditionsManagerObject::__load_range_immediate(DetElement det,
                                                     const std::string& cond,
                                                     const IOV& req_validity,
                                                     RangeConditions& conditions)
{
  const IOV& iov = req_validity;
  {
    dd4hep_lock_t locked_action(m_updateLock);
    m_loader->load_range(det, cond, iov, conditions);
  }
  if ( conditions.size() == 0 )  {
    except("ConditionsManager","+++ Condition %s.%s for the requested IOV %s do not exist.",
           det.path().c_str(), cond.c_str(), iov.str().c_str());
  }
  __push_immediate(conditions);
  conditions.clear();
  return __find_range(det, cond, iov, conditions);
}

/// Retrieve a condition given a Detector Element and the conditions name
Condition
ConditionsManagerObject::get(DetElement det, const string& cond, const IOV& req_validity)
{
  RangeConditions conditions;
  const IOV& iov = req_validity;
  __check_values__<Discrete>(this, det, cond, &iov);
  bool rc = __find(det, cond, iov, conditions);
  if ( !rc )  {
    rc = __load_immediate(det, cond, iov, conditions);
  }
  if ( !rc )   {
    except("ConditionsManager","+++ Condition %s.%s for the requested IOV %s do not exist.",
           det.path().c_str(), cond.c_str(), iov.str().c_str());
  }
  else if ( conditions.size() > 1 )  {
    printout(ERROR,"ConditionsManager","+++ Condition %s.%s is ambiguous for IOV %s:",
             det.path().c_str(), cond.c_str(), iov.str().c_str());
    for(RangeConditions::const_iterator i=conditions.begin(); i!=conditions.end(); ++i)  {
      Condition c = *i;
      printout(ERROR,"ConditionsManager","+++ %s.%s [%s] = %s",
               c->detector.path().c_str(), c.name().c_str(), c->iov->str().c_str(), c->value.c_str());
    }
    except("ConditionsManager","+++ Condition %s.%s is ambiguous for IOV %s:",
           det.path().c_str(), cond.c_str(), iov.str().c_str());
  }
  if ( conditions.size() == 1 )   {
    conditions[0]->flags |= ConditionObject::ACTIVE;
    return conditions[0];
  }
  return Condition();
}

/// Retrieve a condition given a Detector Element and the conditions name
RangeConditions
ConditionsManagerObject::getRange(DetElement det,
                                  const std::string& cond,
                                  const IOV& req_range_validity)
{
  RangeConditions conditions;
  const IOV& iov = req_range_validity;
  __check_values__<Range>(this, det, cond, &iov);
  bool rc = __find_range(det, cond, iov, conditions);
  if ( rc )  {
    return conditions;
  }
  rc = __load_range_immediate(det, cond, iov, conditions);
  if ( !rc )  {
    except("ConditionsManager","+++ Conditions %s.%s for IOV %s do not exist.",
           det.path().c_str(), cond.c_str(), iov.str().c_str());
  }
  return conditions;
}
