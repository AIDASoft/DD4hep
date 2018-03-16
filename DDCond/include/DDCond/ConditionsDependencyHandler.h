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
#ifndef DDCOND_CONDITIONSDEPENDENCYHANDLER_H
#define DDCOND_CONDITIONSDEPENDENCYHANDLER_H

// Framework include files
#include "DD4hep/DetElement.h"
#include "DD4hep/ConditionDerived.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class UserPool;
    class ConditionsPool;
    class ConditionsManagerObject;
    
    /// Callback handler to update condition dependencies.
    /** 
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsDependencyHandler : public ConditionResolver {
    public:
      enum State  {  CREATED, RESOLVED };
      struct Created  {
        const ConditionDependency* dependency;
        Condition::Object*         condition;
        State                      state;
        Created() = default;
        Created(const Created&) = default;
        Created& operator=(const Created&) = default;
        Created(const ConditionDependency* d, Condition::Object* o) : dependency(d), condition(o), state(CREATED) {}
      };
      typedef std::map<Condition::key_type, const ConditionDependency*>  Dependencies;
      typedef std::map<Condition::key_type, Created> CreatedConditions;
    protected:
      /// Reference to conditions manager 
      ConditionsManagerObject*    m_manager;
      /// Reference to the user pool object
      UserPool&                   m_pool;
      /// Dependency container to be resolved.
      const Dependencies&         m_dependencies;
      /// IOV target pool for this handler
      ConditionsPool*             m_iovPool;
      /// User defined optional processing parameter
      ConditionUpdateUserContext* m_userParam;
      /// The objects created during processing
      CreatedConditions           m_created;
      /// Handler's state
      State                       m_state = CREATED;

    public:
      /// Number of callbacks to the handler for monitoring
      mutable size_t              num_callback;

    protected:
      /// Internal call to trigger update callback
      Condition::Object* do_callback(const ConditionDependency* dep);

    public:
      /// Initializing constructor
      ConditionsDependencyHandler(ConditionsManager mgr,
                                  UserPool& pool, 
                                  const Dependencies& dependencies,
                                  ConditionUpdateUserContext* user_param);
      /// Default destructor
      ~ConditionsDependencyHandler();
      /// ConditionResolver implementation: Access to the detector description instance
      Detector& detectorDescription() const;

      /// Access the conditions created during processing
      const CreatedConditions& created()  const                  { return m_created;         }
      /// 1rst pass: Compute/create the missing conditions
      void compute();
      /// 2nd pass:  Handler callback for the second turn to resolve missing dependencies
      void resolve();

      /** ConditionResolver interface implementation         */
      /// ConditionResolver implementation: Access to the conditions manager
      virtual Ref_t manager() const  override                    { return m_manager;         }
      /// Access to pool IOV
      virtual const IOV& requiredValidity()  const  override     { return m_pool.validity(); }
      /// Accessor for the current conditons mapping
      virtual ConditionsMap& conditionsMap() const override      { return m_pool;            }
      /// ConditionResolver implementation: Interface to access conditions.
      virtual Condition get(const ConditionKey& key) override    { return get(key.hash);     }
      /// ConditionResolver implementation: Interface to access conditions
      virtual Condition get(Condition::key_type key) override;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(DetElement de) override;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(Condition::detkey_type key) override;
    };

  }        /* End namespace cond                */
}          /* End namespace dd4hep                    */

#endif     /* DDCOND_CONDITIONSDEPENDENCYHANDLER_H    */
