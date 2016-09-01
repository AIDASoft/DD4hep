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
#ifndef DDCOND_CONDITIONSINTERNA_H
#define DDCOND_CONDITIONSINTERNA_H

// Framework include files%
#include "DD4hep/Mutex.h"
#include "DD4hep/Memory.h"
#include "DD4hep/Callback.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/ObjectExtensions.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsDataLoader.h"

// C/C++ include files
#include <set>
#include <list>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    class Entry;
    class ConditionsPool;
    class ConditionsIOVPool;
    class ConditionsListener;
    class ConditionsDataLoader;

    /// Conditions internal namespace declaration
    /** Internally defined datastructures are not presented to the
     *  user directly, but are used by dedicated views.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    //namespace Interna {

    /// The data class behind a conditions manager handle
    /**
     *  For convenience, the class definition is here.
     *  See ConditionsManager.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsManagerObject : public NamedObject, 
                                    public PropertyConfigurable,
                                    public ObjectExtensions
    {
      friend class DD4hep::Conditions::ConditionsPool;
      friend class DD4hep::Conditions::ConditionsDataLoader;

    public:
      typedef dd4hep_ptr<ConditionsDataLoader>     Loader;
      typedef std::vector<IOVType>                 IOVTypes;
      typedef std::vector<ConditionsIOVPool*>      TypedConditionPool;
      typedef std::pair<ConditionsListener*,void*> Listener;
      typedef std::set<Listener>                   Listeners;
      typedef ConditionsManager::Dependencies      Dependencies;
      typedef Condition::key_type                  key_type;
      typedef Condition::iov_type                  iov_type;

    public:
      /// Property: maximal number of IOV types to be handled
      int                    m_maxIOVTypes;
      /// Property: ConditionsPool constructor type (default: empty. MUST BE SET!)
      std::string            m_poolType;
      /// Property: UpdatePool constructor type (default: DD4hep_ConditionsLinearUpdatePool)
      std::string            m_updateType;
      /// Property: UserPool constructor type (default: DD4hep_ConditionsLinearUserPool)
      std::string            m_userType;
      /// Property: Conditions loader type (default: "multi" -> DD4hep_Conditions_multi_Loader)
      std::string            m_loaderType;

      /// Reference to main detector description object
      LCDD&                  m_lcdd;
      /// Collection of IOV types managed
      IOVTypes               m_iovTypes;
      /// Managed pool of typed conditions indexed by IOV-type and IOV key
      TypedConditionPool     m_rawPool;
      /// Conditions listeners on registration of new conditions
      Listeners              m_onRegister;
      /// Conditions listeners on de-registration of new conditions
      Listeners              m_onRemove;
      /// Lock to protect the update/delayed conditions pool
      dd4hep_mutex_t         m_updateLock;
      /// Lock to protect the pool of all known conditions
      dd4hep_mutex_t         m_poolLock;
      /// Reference to data loader
      Loader                 m_loader;
      /// Reference to update conditions pool
      dd4hep_ptr<UpdatePool> m_updatePool;

      /// Public access: if locked, DetElements stay intact and are not altered
      int                    locked;

    protected:
      /// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
      bool select(key_type key, const iov_type& req_validity, RangeConditions& conditions);

      /// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
      bool select_range(key_type key, const iov_type& req_validity, RangeConditions& conditions);

      /// Register a set of new managed condition for an IOV range. Called by __load_immediate
      // void __register_immediate(RangeConditions& c);

      void registerCallee(Listeners& listeners, const Listener& callee, bool add);

      /// Listener invocation when a condition is registered to the cache
      void onRegister(Condition condition);

      /// Listener invocation when a condition is deregistered from the cache
      void onRemove(Condition condition);

    public:
      /// Set a single conditions value to be managed.
      /// Requires EXTERNALLY held lock on update pool!
      Condition __queue_update(Conditions::Entry* data);

    public:
      /// Standard constructor
      ConditionsManagerObject(LCDD& lcdd);

      /// Default destructor
      virtual ~ConditionsManagerObject();

      /// Access to the detector description instance
      LCDD& lcdd() const  {   return m_lcdd; }

      /// Access to managed pool of typed conditions indexed by IOV-type and IOV key
      const TypedConditionPool& conditionsPool() const  {  return m_rawPool; }
	  
      ConditionsDataLoader* loader()  const  {  return m_loader.get();  }

      void initialize();

      /// (Un)Registration of conditions listeners with callback when a new condition is registered
      void callOnRegister(const Listener& callee, bool add);
      /// (Un)Registration of conditions listeners with callback when a condition is unregistered
      void callOnRemove(const Listener& callee, bool add);

      /// Register new IOV type if it does not (yet) exist.
      /** Returns (false,pointer) if IOV existed and
       *  (true,pointer) if new IOV was registered to the manager.
       */
      std::pair<bool, const IOVType*> registerIOVType(size_t iov_type, const std::string& iov_name);
      
      /// Access IOV by its type
      const IOVTypes& iovTypes () const   {   return  m_iovTypes;  }

      /// Access IOV by its type
      const IOVType* iovType (size_t iov_type) const;

      /// Access IOV by its name
      const IOVType* iovType (const std::string& iov_name) const;

      /// Create IOV from string
      void fromString(const std::string& iov_str, IOV& iov);

      /// Register IOV using new string data
      ConditionsPool* registerIOV(const std::string& data);

      /// Register IOV with type and key
      ConditionsPool* registerIOV(const IOVType& typ, IOV::Key key);

      /// Register new condition with the conditions store. Unlocked version, not multi-threaded
      bool registerUnlocked(ConditionsPool* pool, Condition cond);

      /// Prepare all updates to the clients with the defined IOV
      long prepare(const IOV& required_validity, dd4hep_ptr<UserPool>& user_pool);

      /// Prepare all updates to the clients with the defined IOV
      long prepare(const IOV&            required_validity,
                   dd4hep_ptr<UserPool>& user_pool,
                   const Dependencies&   dependencies,
                   bool                  verify_dependencies=true);

      /// Clean conditions, which are above the age limit.
      /** @return Number of conditions cleaned up and removed  */
      int clean(const IOVType* typ, int max_age);

      /// Full cleanup of all managed conditions.
      /** @return pair<Number of pools cleared, Number of conditions cleaned up and removed> */
      std::pair<int,int> clear();

      /// Push all pending updates to the conditions store. 
      /** Note:
       *  This does not yet make the new conditions availible to the clients
       */
      void pushUpdates();
 
      /// Register a new managed condition.
      /** The condition is created externally by the user.
       *  Lengthy and tedious procedure.
       */
      void registerCondition(Condition c);

      /// Retrieve a condition set given a Detector Element and the conditions name according to their validity
      Condition get(key_type key, const iov_type& req_validity);

      /// Retrieve a condition given a Detector Element and the conditions name
      RangeConditions getRange(key_type key, const iov_type& req_validity);
    };
    //    } /* End namespace Interna              */
  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSINTERNA_H    */
