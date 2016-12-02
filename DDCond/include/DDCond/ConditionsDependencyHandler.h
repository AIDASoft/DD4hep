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
#ifndef DDCOND_CONDITIONSDEPENDENCYHANDLER_H
#define DDCOND_CONDITIONSDEPENDENCYHANDLER_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/ConditionDerived.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsDependencyCollection.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class UserPool;
    class ConditionsPool;
    class ConditionsManagerObject;
    class ConditionsDependencyCollection;
    
    /// Callback handler to update condition dependencies.
    /** 
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsDependencyHandler : public ConditionResolver {
    public:
      typedef ConditionsDependencyCollection Dependencies;

    protected:
      /// Reference to conditions manager 
      ConditionsManagerObject* m_manager;
      /// Reference to the user pool object
      UserPool&                m_pool;
      /// Dependency container to be resolved.
      const Dependencies&      m_dependencies;
      /// IOV target pool for this handler
      ConditionsPool*          m_iovPool;
      /// User defined optional processing parameter
      void*                    m_userParam;

    public:
      /// Number of callbacks to the handler for monitoring
      mutable size_t           num_callback;

    protected:
      /// Internal call to trigger update callback
      Condition::Object* do_callback(const ConditionDependency& dep) const;

    public:
      /// Initializing constructor
      ConditionsDependencyHandler(ConditionsManager mgr,
                                  UserPool& pool, 
                                  const Dependencies& dependencies,
                                  void* user_param);
      /// Default destructor
      ~ConditionsDependencyHandler();
      /// ConditionResolver implementation: Access to the detector description instance
      LCDD& lcdd() const;
      /// ConditionResolver implementation: Access to the conditions manager
      virtual Ref_t manager() const                     { return m_manager;         }
      /// Access to pool IOV
      virtual const IOV& requiredValidity()  const      { return m_pool.validity(); }
      /// ConditionResolver implementation: Interface to access conditions.
      virtual Condition get(const ConditionKey& key)  const { return get(key.hash); }
      /// ConditionResolver implementation: Interface to access conditions
      virtual Condition get(Condition::key_type key)  const;
      /// Handler callback to process multiple derived conditions
      Condition::Object* operator()(const ConditionDependency* dep)  const;
    };

  }        /* End namespace Conditions                */
}          /* End namespace DD4hep                    */

#endif     /* DDCOND_CONDITIONSDEPENDENCYHANDLER_H    */
