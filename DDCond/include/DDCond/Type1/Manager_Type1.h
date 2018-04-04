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
#ifndef DDCOND_CONDITIONSMANAGEROBJECT_TYPE1_H
#define DDCOND_CONDITIONSMANAGEROBJECT_TYPE1_H

// Framework include files%
#include "DD4hep/Mutex.h"
#include "DD4hep/ObjectExtensions.h"
#include "DDCond/ConditionsManagerObject.h"

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    class Entry;
    class ConditionsPool;
    class ConditionsSlice;
    class ConditionsIOVPool;
    class ConditionsDataLoader;

    /// The data class behind a conditions manager handle
    /**
     *  For convenience, the class definition is here.
     *  See ConditionsManager.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Manager_Type1 : public ConditionsManagerObject, 
                          public ObjectExtensions
    {
    public:
      typedef std::vector<ConditionsIOVPool*> TypedConditionPool;

    protected:

      /** Generic interface of any concrete instance  */
      
      /// Property: maximal number of IOV types to be handled
      int                     m_maxIOVTypes;
      /// Property: ConditionsPool constructor type (default: empty. MUST BE SET!)
      std::string             m_poolType;
      /// Property: UpdatePool constructor type (default: DD4hep_ConditionsLinearUpdatePool)
      std::string             m_updateType;
      /// Property: UserPool constructor type (default: DD4hep_ConditionsLinearUserPool)
      std::string             m_userType;
      /// Property: Conditions loader type (default: "multi" -> DD4hep_Conditions_multi_Loader)
      std::string             m_loaderType;

      /// Collection of IOV types managed
      IOVTypes                m_iovTypes;

      /** Specialized interface only used by this implementation  */
      /// Lock to protect the update/delayed conditions pool
      dd4hep_mutex_t          m_updateLock;
      /// Lock to protect the pool of all known conditions
      dd4hep_mutex_t          m_poolLock;
      /// Reference to update conditions pool
      std::unique_ptr<UpdatePool>  m_updatePool;

    public:
      /// Managed pool of typed conditions indexed by IOV-type and IOV key
      TypedConditionPool      m_rawPool;
      /// Public access: if locked, DetElements stay intact and are not altered
      int                     m_locked;

    protected:
      /// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
      bool select(key_type key, const IOV& req_validity, RangeConditions& conditions);

      /// Retrieve  a condition set given a Detector Element and the conditions name according to their validity
      bool select_range(key_type key, const IOV& req_validity, RangeConditions& conditions);

      /// Register a set of new managed condition for an IOV range. Called by __load_immediate
      // void __register_immediate(RangeConditions& c);


      /// Helper to check iov and user pool and create user pool if not present
      void __get_checked_pool(const IOV& required_validity, std::unique_ptr<UserPool>& user_pool);
      
      /** Generic interface of any concrete instance  */
      
      /// Set a single conditions value to be managed.
      /// Requires EXTERNALLY held lock on update pool!
      Condition __queue_update(cond::Entry* data);

    public:
      /// Standard constructor
      Manager_Type1(Detector& description);

      /// Default destructor
      virtual ~Manager_Type1();

      /// Access to managed pool of typed conditions indexed by IOV-type and IOV key
      //const TypedConditionPool& conditionsPool() const  {  return m_rawPool; }
	  
      /// Initialize object and define factories
      void initialize();

      /// Register new IOV type if it does not (yet) exist.
      /** Returns (false,pointer) if IOV existed and
       *  (true,pointer) if new IOV was registered to the manager.
       */
      virtual std::pair<bool, const IOVType*> registerIOVType(size_t iov_index, const std::string& iov_name) final;
      
      /// Access IOV by its type
      virtual const IOVTypes& iovTypes () const final { return  m_iovTypes;  }

      /// Access IOV by its type
      virtual const IOVType* iovType (size_t iov_index) const final;

      /// Access IOV by its name
      virtual const IOVType* iovType (const std::string& iov_name) const final;

      /// Register IOV with type and key
      virtual ConditionsPool* registerIOV(const IOVType& typ, IOV::Key key)  final;

      /// Access conditions multi IOV pool by iov type
      ConditionsIOVPool* iovPool(const IOVType& type)  const  final;

      /// Register new condition with the conditions store. Unlocked version, not multi-threaded
      virtual bool registerUnlocked(ConditionsPool& pool, Condition cond)  final;

      /// Clean conditions, which are above the age limit.
      /** @return Number of conditions cleaned/removed from the IOV pool of the given type   */
      int clean(const IOVType* typ, int max_age)  final;

      /// Invoke cache cleanup with user defined policy
      /** @return pair<Number of pools cleared, Number of conditions cleaned up and removed> */
      std::pair<int,int> clean(const ConditionsCleanup& cleaner)  final;

      /// Full cleanup of all managed conditions.
      /** @return pair<Number of pools cleared, Number of conditions cleaned up and removed> */
      std::pair<int,int> clear()  final;

      /// Push all pending updates to the conditions store. 
      /** Note:
       *  This does not yet make the new conditions availible to the clients
       */
      virtual void pushUpdates()  final;
#if 0 
      /// Retrieve a condition set given a Detector Element and the conditions name according to their validity  (deprecated)
      virtual Condition get(key_type key, const IOV& req_validity)  final;

      /// Retrieve a condition given a Detector Element and the conditions name (deprecated)
      virtual RangeConditions getRange(key_type key, const IOV& req_validity)  final;
#endif
      /// Create empty user pool object
      virtual std::unique_ptr<UserPool> createUserPool(const IOVType* iovT)  const;

      /// Prepare all updates for the given keys to the clients with the defined IOV
      /**
       *   @arg      req_validity [IOV]            Required interval of validity of the selected conditions
       *   @arg      slice        []               Conditions slice with load/computation recipes.
       *
       *   @return   
       */
      Result prepare(const IOV& req_iov, ConditionsSlice& slice, ConditionUpdateUserContext* ctxt)  final;

    };
  }        /* End namespace cond               */
}          /* End namespace dd4hep                   */
#endif     /* DDCOND_CONDITIONSMANAGEROBJECT_TYPE1_H */
