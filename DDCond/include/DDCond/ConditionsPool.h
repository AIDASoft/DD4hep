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
#ifndef DDCOND_CONDITIONS_CONDITIONSPOOL_H
#define DDCOND_CONDITIONS_CONDITIONSPOOL_H

// Framework include files
#include "DD4hep/NamedObject.h"
#include "DD4hep/ConditionsMap.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class ConditionsSlice;
    class ConditionsIOVPool;
    class ConditionDependency;
    
    /// Class implementing the conditions collection for a given IOV type
    /**
     *  Implementation is mostly virtual, to allow to switch between
     *  different implementations, which allow for different lookup
     *  and store optimizations and/or various caches.
     *  The interface only represents the basic functionality required.
     *
     *  Please note:
     *  Users should not directly interact with object instances of this type.
     *  Data are not thread protected and interaction may cause serious harm.
     *  Only the ConditionsManager implementation should interact with
     *  this class or any subclass to ensure data integrity.
     *
     *  For convenience, the class definition is here.
     *  See ConditionsManager.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsPool : public NamedObject {
    protected:
      /// Handle to conditions manager object
      ConditionsManager m_manager;
      
    public:
      enum { AGE_NONE    = 0, 
             AGE_ANY     = 9999999,
             AGE_EXPIRED = 12345678
      };
      /// The IOV of the conditions hosted
      IOV* iov;
      /// Aging value
      int  age_value;

    public:
      /// Listener invocation when a condition is registered to the cache
      void onRegister(Condition condition);
      /// Listener invocation when a condition is deregistered from the cache
      void onRemove(Condition condition);

    public:
      /// Default constructor
      ConditionsPool(ConditionsManager mgr);
      /// Default destructor. Note: pool must be cleared by the subclass!
      virtual ~ConditionsPool();
      /// Print pool basics
      void print()   const;
      /// Print pool basics
      void print(const std::string& opt)   const;
      /// Total entry count
      virtual size_t size()  const = 0;
      /// Full cleanup of all managed conditions.
      virtual void clear() = 0;
      /// Register a new condition to this pool
      virtual bool insert(Condition cond) = 0;
      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& cond) = 0;
      /// Check if a condition exists in the pool
      virtual Condition exists(Condition::key_type key)  const = 0;
      /// Select the conditions matching the DetElement and the conditions name
      virtual size_t select(Condition::key_type key, RangeConditions& result) = 0;
      /// Select all conditions contained
      virtual size_t select_all(RangeConditions& result) = 0;
      /// Select the conditons, passing a predicate
      virtual size_t select_all(const ConditionsSelect& predicate) = 0;
      /// Select all conditions contained
      virtual size_t select_all(ConditionsPool& selection_pool) = 0;
    };

    /// Interface for conditions pool optimized to host conditions updates.
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class UpdatePool : public ConditionsPool  {

    public:
      typedef std::vector<Condition> ConditionEntries;
      /// Update container specification
      typedef std::map<const IOV*, ConditionEntries> UpdateEntries;

    public:
      /// Default constructor
      UpdatePool(ConditionsManager mgr);
      /// Default destructor.
      virtual ~UpdatePool();
      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual size_t popEntries(UpdateEntries& entries) = 0;
      /// Select the conditions matching the key
      virtual void select_range(Condition::key_type key, 
                                const IOV& req_validity,
                                RangeConditions& result) = 0;
    };

    /// Interface for conditions pool optimized to host conditions updates.
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class UserPool : public ConditionsMap {
    public:
      /// Forward definition of the key type
      typedef std::map<Condition::key_type,const ConditionDependency*> Dependencies;

    protected:
      /// The pool's interval of validity
      IOV                 m_iov;
      /// Handle to conditions manager object
      ConditionsManager   m_manager;

    public:
      /// Printout flags for debugging
      enum {
        PRINT_NONE   = 0,
        PRINT_INSERT = 1<<0,
        PRINT_CLEAR  = 1<<1
      };
      /// Processing flags (printout etc.)
      unsigned int flags = 0;

      /// ConditionsMap overload: Add a condition directly to the slice
      virtual bool insert(Condition condition) = 0;

    public:
      /// Default constructor
      UserPool(ConditionsManager mgr);
      /// Default destructor.
      virtual ~UserPool();
      /// Access the interval of validity for this user pool
      const IOV& validity() const    {  return m_iov;  }
      /// Access the interval of validity for this user pool
      const IOV* validityPtr() const {  return &m_iov; }
      /// Print pool content
      virtual void print(const std::string& opt) const = 0;
      /// Total entry count
      virtual size_t size()  const = 0;
      /// Full cleanup of all managed conditions.
      virtual void clear() = 0;
      /// Check a condition for existence
      virtual bool exists(Condition::key_type key)  const = 0;
      /// Check a condition for existence
      virtual bool exists(const ConditionKey& key)  const = 0;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(Condition::key_type key)  const = 0;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(const ConditionKey& key)  const = 0;
      /// Remove condition by key from pool.
      virtual bool remove(Condition::key_type hash_key) = 0;
      /// Remove condition by key from pool.
      virtual bool remove(const ConditionKey& key) = 0;
      /// ConditionsMap overload: Add a condition directly to the slice
      virtual bool insert(DetElement detector, Condition::itemkey_type key, Condition condition) = 0;

      /// ConditionsMap overload: Access a single condition
      virtual Condition get(DetElement detector, Condition::itemkey_type key)  const = 0;
      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      virtual std::vector<Condition> get(Condition::key_type lower,
                                         Condition::key_type upper)  const = 0;
      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      virtual std::vector<Condition> get(DetElement detector,
                                         Condition::itemkey_type lower,
                                         Condition::itemkey_type upper)  const = 0;

      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(const Condition::Processor& processor) const = 0;
      /// No ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(Condition::key_type lower,
                        Condition::key_type upper,
                        const Condition::Processor& processor) const = 0;
      /// No ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(DetElement detector,
                        Condition::itemkey_type lower,
                        Condition::itemkey_type upper,
                        const Condition::Processor& processor) const = 0;

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual ConditionsManager::Result prepare(const IOV&                  required, 
                                                ConditionsSlice&            slice,
                                                ConditionUpdateUserContext* user_param = 0) = 0;

      /// Evaluate and register all derived conditions from the dependency list
      virtual size_t compute(const Dependencies& dependencies,
                             ConditionUpdateUserContext* user_param,
                             bool force) = 0;
    };
  }        /* End namespace cond                     */
}          /* End namespace dd4hep                   */
#endif     /* DDCOND_CONDITIONS_CONDITIONSPOOL_H     */
