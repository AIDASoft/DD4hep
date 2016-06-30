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
#ifndef DDCOND_CONDITIONSPOOL_H
#define DDCOND_CONDITIONSPOOL_H

// Framework include files
#include "DD4hep/Mutex.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DDCond/ConditionsManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class Entry;
    class ConditionsPool;
    class ReplacementPool;

    /// Class implementing the conditions collection for a given IOV type
    /**
     *  Implementation is mostly virtual, to allow to switch between
     *  different implementations, which allow for different lookup
     *  and store optimizations and/or various caches.
     *  The interface only represents the basic functionality required.
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
      enum { NO_POOL_TYPE     = 0,
	     UPDATE_POOL_TYPE = 1,
	     USER_POOL_TYPE   = 2
      };
      /// IOV type of the conditions hosted by this pool
      const IOVType*   iovType;
      /// The IOV of the conditions hosted
      IOV*             iov;
      /// Aging value
      int              age_value;
      /// Pool type (regular, user, ipdate,...)
      int              pool_type;

    protected:

      /// Unconditionally create a new condition from the input data
      /** 
       *  Common function for all pools.....
       *  \author  M.Frank
       *  \version 1.0
       */
      virtual Condition create(ConditionsPool* pool, const Entry* cond);

    public:
      /// Default constructor
      ConditionsPool(ConditionsManager mgr);
      /// Default destructor. Note: pool must be cleared by the subclass!
      virtual ~ConditionsPool();
      /// Print pool basics
      void print(const std::string& opt)   const;
      /// Listener invocation when a condition is registered to the cache
      void onRegister(Condition condition);
      /// Listener invocation when a condition is deregistered from the cache
      void onRemove(Condition condition);
      /// Register a new condition to this pool
      virtual void insert(Condition cond) = 0;
      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& cond) = 0;
      /// Full cleanup of all managed conditions.
      virtual void clear() = 0;
      /// Check if a condition exists in the pool
      virtual Condition exists(DetElement, const std::string&)  const = 0;
      /// Check if a condition exists in the pool
      virtual Condition exists(Condition)  const = 0;
      /// Select the conditions matching the DetElement and the conditions name
      virtual void select(DetElement det, const std::string& cond_name, RangeConditions& result) = 0;
      /// Select all conditions contained
      virtual void select_all(RangeConditions& result) = 0;
      /// Select all conditions contained
      virtual void select_all(ConditionsPool& selection_pool) = 0;
      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_used(RangeConditions& result) = 0;
      /// Total entry count
      virtual int count()  const = 0;
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
      /// Pool type identifier
      static int poolType()  {  return UPDATE_POOL_TYPE; }
      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual void popEntries(UpdateEntries& entries) = 0;
      /// Register a new condition to this pool
      virtual Condition insertEntry(ConditionsPool* pool, Entry* cond) = 0;
      /// Select the conditions matching the DetElement and the conditions name
      virtual void select_range(DetElement det,
                                const std::string& cond_name, 
                                const IOV& req_validity,
                                RangeConditions& result) = 0;
    };

    /// Interface for conditions pool optimized to host conditions updates.
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class UserPool : public ConditionsPool  {
    public:
      /// Default constructor
      UserPool(ConditionsManager mgr);
      /// Default destructor.
      virtual ~UserPool();
      /// Pool type identifier
      static int poolType()  {  return USER_POOL_TYPE; }
      /// Access the interval of validity for this user pool
      virtual const IOV& validity() const = 0;
      /// Update interval of validity for this user pool (should only be called by ConditionsManager)
      virtual void setValidity(const IOV& value) = 0;
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSPOOL_H    */
