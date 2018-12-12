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
     *  Fully stack based condition resolution. Any user request
     *  for dependent conditions are fully handled by 
     *  - either the connected conditions pool (if present there) or
     *  - by the currently worked on items.
     *
     *  The ConditionsDependencyHandler implements the 
     *  ConditionResolver interface, which is offered to the clients as
     *  an embedded parameter during the conversion mechanism.
     *  Clients must direct any subsequent conditions access to the
     *  ConditionResolver interface in order to allow for upgrades of
     *  this implementation which might not be polymorph.
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsDependencyHandler : public ConditionResolver {
    public:
      /// Obect state definitions
      enum State  {  INVALID, CREATED, RESOLVED };
      /// Helper structure to define the current update item
      struct Work  {
      public:
        IOV                        _iov;
        /// Auxiliary information to resolve condition callbacks
        ConditionUpdateContext     context;
        /// Condition IOV
        IOV*                       iov       = 0;
        /// The final result: the condition object
        Condition::Object*         condition = 0;
        /// Flag to detect non resolvable circular dependencies
        int                        callstack = 0;
        /// Current conversion state of the item
        State                      state     = INVALID;
      public:
        /// Inhibit default constructor
        Work() = delete;
        /// Initializing constructor
        Work(ConditionResolver* r,
             const ConditionDependency* d,
             ConditionUpdateUserContext* u,
             const IOV& i)
          : _iov(i), context(r,d,&_iov,u) {}
        /// Copy constructor
        Work(const Work&) = default;
        /// Assignment operator
        Work& operator=(const Work&) = default;
        /// Helper to determine the IOV intersection taking into account dependencies
        void do_intersection(const IOV* iov);
        /// Helper function for the second level dependency resolution
        Condition resolve(Work*& current);
      };
      typedef std::map<Condition::key_type, const ConditionDependency*>  Dependencies;
      typedef std::map<Condition::key_type, Work*> WorkConditions;

    protected:
      /// Reference to conditions manager 
      ConditionsManagerObject*    m_manager;
      /// Reference to the user pool object
      UserPool&                   m_pool;
      /// Dependency container to be resolved.
      const Dependencies&         m_dependencies;
      /// User defined optional processing parameter
      ConditionUpdateUserContext* m_userParam;
      /// Local cacheL pool's IOV type
      const IOVType*              m_iovType = 0;
      /// The objects created during processing
      WorkConditions              m_todo;
      /// Handler's state
      State                       m_state = CREATED;
      /// Current block work item
      Work*                       m_block = 0;
      /// Current item of the block
      Work*                       m_currentWork = 0;
    public:
      /// Number of callbacks to the handler for monitoring
      mutable size_t              num_callback;

    protected:
      /// Internal call to trigger update callback
      void do_callback(Work* dep);

    public:
      /// Initializing constructor
      ConditionsDependencyHandler(ConditionsManager mgr,
                                  UserPool& pool, 
                                  const Dependencies& dependencies,
                                  ConditionUpdateUserContext* user_param);
      /// Default destructor
      ~ConditionsDependencyHandler();

      /// Access the conditions created during processing
      //const CreatedConditions& created()  const                  { return m_created;         }
      /// 1rst pass: Compute/create the missing conditions
      void compute();
      /// 2nd pass:  Handler callback for the second turn to resolve missing dependencies
      void resolve();

      /** ConditionResolver interface implementation         */
      /// Access to the detector description instance
      virtual Detector& detectorDescription() const  override;
      /// Access to the conditions manager
      virtual Ref_t manager() const  override                    { return m_manager;         }
      /// Access to pool IOV
      virtual const IOV& requiredValidity()  const  override     { return m_pool.validity(); }
      /// Accessor for the current conditons mapping
      virtual ConditionsMap& conditionsMap() const override      { return m_pool;            }
      /// ConditionResolver implementation: Interface to access conditions.
      virtual Condition get(const ConditionKey& key) override    { return get(key.hash);     }
      /// Interface to access conditions by conditions key
      virtual Condition get(const ConditionKey& key, bool throw_if_not)  override
      {  return get(key.hash, throw_if_not);                                                 }
      /// ConditionResolver implementation: Interface to access conditions
      virtual Condition get(Condition::key_type key) override    { return get(key, true);    }
      /// Interface to access conditions by hash value
      virtual Condition get(Condition::key_type key, bool throw_if_not)  override;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(DetElement de) override;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(Condition::detkey_type key) override;
      /// Interface to access conditions by hash value of the item (only valid at resolve!)
      virtual std::vector<Condition> getByItem(Condition::itemkey_type key)  override;
    };

  }        /* End namespace cond                */
}          /* End namespace dd4hep                    */

#endif     /* DDCOND_CONDITIONSDEPENDENCYHANDLER_H    */
