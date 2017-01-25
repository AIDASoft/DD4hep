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
#ifndef DDCOND_CONDITIONS_CONDITIONSMANAGEROBJECT_H
#define DDCOND_CONDITIONS_CONDITIONSMANAGEROBJECT_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/NamedObject.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"

// C/C++ include files
#include <vector>
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class IOVType;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class ConditionsIOVPool;
    class ConditionsListener;
    class ConditionsDataLoader;
    
    /// Basic conditions manager implementation
    /**
     *  This class implements the basic functionality of the conditions manager.
     *  To allow for specialization, the important functions are kept virtual.
     *  For performance reasons these are mostly functions, which either
     *  are called elatively rarely or are bulk functions.
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsManagerObject : public NamedObject,
                                    public PropertyConfigurable
    {
    public:
      typedef std::vector<IOVType>                 IOVTypes;
      typedef Condition::key_type                  key_type;
      typedef Condition::iov_type                  iov_type;
      typedef std::pair<ConditionsListener*,void*> Listener;
      typedef std::set<Listener>                   Listeners;
      typedef dd4hep_ptr<ConditionsDataLoader>     Loader;
      typedef ConditionsManager::Result            Result;

    protected:
      /// Reference to main detector description object
      LCDD&                  m_lcdd;
      /// Conditions listeners on registration of new conditions
      Listeners              m_onRegister;
      /// Conditions listeners on de-registration of new conditions
      Listeners              m_onRemove;
      /// Reference to the data loader userd by this instance
      Loader                 m_loader;
      /// Property: Flag to indicate if items should be loaded (or not)
      bool                   m_doLoad = true;
      /// Property: Flag to indicate if unloaded items should be saved to the slice (or not)
      bool                   m_doOutputUnloaded = false;

      /// Register callback listener object
      void registerCallee(Listeners& listeners, const Listener& callee, bool add);

    public:

      /// Default constructor
      ConditionsManagerObject(LCDD& lcdd);

      /// Default destructor
      virtual ~ConditionsManagerObject();

      /// Access to the detector description instance
      LCDD& lcdd() const                    {  return m_lcdd;              }

      /// Access to the data loader
      ConditionsDataLoader* loader()  const {  return m_loader.get();      }

      /// Access to load flag
      bool doLoadConditions()  const        {  return m_doLoad;            }

      /// Access to flag to indicate if unloaded items should be saved to the slice (or not)
      bool doOutputUnloaded()  const        {  return m_doOutputUnloaded;  }

      /// Listener invocation when a condition is registered to the cache
      void onRegister(Condition condition);

      /// Listener invocation when a condition is deregistered from the cache
      void onRemove(Condition condition);

      /// (Un)Registration of conditions listeners with callback when a new condition is registered
      void callOnRegister(const Listener& callee, bool add);

      /// (Un)Registration of conditions listeners with callback when a condition is unregistered
      void callOnRemove(const Listener& callee, bool add);

      /// Access the used/registered IOV types
      const std::vector<const IOVType*> iovTypesUsed() const;

      /// Create IOV from string
      void fromString(const std::string& iov_str, IOV& iov);

      /// Register IOV using new string data
      ConditionsPool* registerIOV(const std::string& data);

      /** Overloadable interface  */
      /// Initialize the object after having set the properties
      virtual void initialize() = 0;

      /// Access IOV by its type
      virtual const IOVTypes& iovTypes () const = 0;

      /// Access IOV by its type
      virtual const IOVType* iovType (size_t iov_type) const = 0;

      /// Access IOV by its name
      virtual const IOVType* iovType (const std::string& iov_name) const = 0;

      /// Register new IOV type if it does not (yet) exist.
      /** Returns (false,pointer) if IOV existed and
       *  (true,pointer) if new IOV was registered to the manager.
       */
      virtual std::pair<bool, const IOVType*> registerIOVType(size_t iov_type, const std::string& iov_name) = 0;

      /// Register IOV with type and key (much more performant...)
      virtual ConditionsPool* registerIOV(const IOVType& typ, IOV::Key key) = 0;
      
      /// Access conditions multi IOV pool by iov type
      virtual ConditionsIOVPool* iovPool(const IOVType& type)  const = 0;

      /// Retrieve a condition set given a Detector Element and the conditions name according to their validity
      virtual Condition get(key_type key, const iov_type& req_validity) = 0;

      /// Retrieve a condition given a Detector Element and the conditions name
      virtual RangeConditions getRange(key_type key, const iov_type& req_validity) = 0;

      /// Push all pending updates to the conditions store. 
      /** Note:
       *  This does not yet make the new conditions availible to the clients
       */
      virtual void pushUpdates() = 0;

      /// Register new condition with the conditions store. Unlocked version, not multi-threaded
      virtual bool registerUnlocked(ConditionsPool* pool, Condition cond) = 0;

      /// Prepare all updates to the clients with the defined IOV
      virtual Result prepare(const IOV& req_iov, ConditionsSlice& slice) = 0;

      /// Clean conditions, which are above the age limit.
      /** @return Number of conditions cleaned/removed from the IOV pool of the given type   */
      virtual int clean(const IOVType* typ, int max_age) = 0;

      /// Full cleanup of all managed conditions.
      /** @return pair<Number of pools cleared, Number of conditions cleaned up and removed> */
      virtual std::pair<int,int> clear() = 0;

    };
  }    /* End namespace Geometry                       */
}      /* End namespace DD4hep                         */
#endif /* DDCOND_CONDITIONS_CONDITIONSMANAGEROBJECT_H  */
