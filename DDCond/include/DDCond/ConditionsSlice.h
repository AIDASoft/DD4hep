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
#ifndef DD4HEP_DDCOND_CONDITIONSSLICE_H
#define DD4HEP_DDCOND_CONDITIONSSLICE_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/ConditionDerived.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsContent.h"
#include "DDCond/ConditionsManager.h"

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class UserPool;
    class ConditionsSlice;
    
    /// Conditions slice object. Defines which conditions should be loaded by the ConditionsManager.
    /**
     *  Object contains set of required conditions keys to be loaded to the user pool.
     *  It alkso contains the load information for the required conditions (conditions addresses).
     *  The address objects depend on the actual loader mechanism and must be specified the user.
     *  The information is then chained through the calls and made availible to the loader object.
     *
     *  On return it contains the individual condition load information.
     *
     *  Referenced by: ConditonsUserPool, ConditionsManager
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsSlice : public ConditionsMap  {
    public:
      typedef Condition::key_type                           key_type;
      typedef std::vector<std::shared_ptr<ConditionsPool> > ContainedPools;

      enum ManageFlag {
        REGISTER_MANAGER= 1<<0,
        REGISTER_POOL   = 1<<1,
        REGISTER_FULL   = REGISTER_MANAGER|REGISTER_POOL
      };
      enum LoadFlags  {
        REF_POOLS       = 1<<1
      };
      
      /// Helper to simplify the registration of new condtitions from arbitrary containers.
      /**
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Inserter  {
        ConditionsSlice& slice;
        ConditionsPool*  iov_pool = 0;

      public:
        Inserter(ConditionsSlice& s) : slice(s) {
          const IOV& iov = s.pool->validity();
          iov_pool = s.manager.registerIOV(*iov.iovType,iov.key());
        }
        template <typename T> Inserter(ConditionsSlice& s, const T& data) : slice(s) {
          const IOV& iov = s.pool->validity();
          iov_pool = s.manager.registerIOV(*iov.iovType,iov.key());
          std::for_each(std::begin(data), std::end(data), *this);
        }
        void operator()(Condition c) const  {
          slice.manager.registerUnlocked(*iov_pool,c);
          slice.pool->insert(c);
        }
        void operator()(const std::pair<Condition::key_type,Condition>& e) const  {
          (*this)(e.second);
        }
        template <typename T> void process(const T& data)  const   {
          std::for_each(std::begin(data), std::end(data), *this);
        }
      };

    public:
      /// Reference to the conditions manager.
      /** Not used by the object, simple for convenience.
       *  Then all actors are lumped together, which are used by the client code.
       */
      ConditionsManager         manager;
      /// Reference to the user pool managing all conditions of this slice
      std::unique_ptr<UserPool> pool;
      /// Container of conditions required by this slice
      std::shared_ptr<ConditionsContent> content;

      /// Store the result of the prepare step
      ConditionsManager::Result status;
      /// If requested refence the used pools with a shared pointer to inhibit cleanup
      ContainedPools            used_pools;
      /// Flag to steer conditions management
      unsigned long             flags = 0;
      
    protected:
      /// If flag conditonsManager["OutputUnloadedConditions"]=true: will contain conditions not loaded
      ConditionsContent::Conditions   m_missingConditions;
      /// If flag conditonsManager["OutputUnloadedConditions"]=true: will contain conditions not computed
      ConditionsContent::Dependencies m_missingDerivations;
      
      /// Default assignment operator
      ConditionsSlice& operator=(const ConditionsSlice& copy) = delete;

      /// Local optimization: Insert a set of conditions to the slice AND register them to the conditions manager.
      /** Note: The conditions already require a valid hash key                           */
      virtual bool manage(ConditionsPool* pool, Condition condition, ManageFlag flg);

      static Condition select_cond(Condition c)  {  return c; }
      template <typename T>
      static Condition select_cond(const std::pair<T,Condition>& c)  {  return c.second; }
    public:
      /// Default constructor
      ConditionsSlice() = delete;
      /// Initializing constructor
      ConditionsSlice(ConditionsManager m);
      /// Initializing constructor
      ConditionsSlice(ConditionsManager m, const std::shared_ptr<ConditionsContent>& c);
      /// Copy constructor (Special, partial copy only. Hence no assignment!)
      ConditionsSlice(const ConditionsSlice& copy);
      /// Default destructor. 
      virtual ~ConditionsSlice();

      /// Total size of all conditions contained in the slice
      size_t size() const   { return content->conditions().size()+content->derived().size(); }
      /// Set flag to reference the used pools during prepare
      void refPools()       { this->flags |= REF_POOLS;                                      }
      /// Set flag to not reference the used pools during prepare (and drop possibly pending)
      void derefPools();
      /// Access the map of conditions from the desired content
      const ConditionsContent::Conditions& conditions() const { return content->conditions();}
      /// Access the map of computational conditions from the desired content
      const ConditionsContent::Dependencies& derived() const  { return content->derived();   }
      /// Access the map of missing conditions (only valid after preparation)
      ConditionsContent::Conditions& missingConditions()      { return m_missingConditions;  }
      /// Access the map of missing computational conditions (only valid after preparation)
      ConditionsContent::Dependencies& missingDerivations()   { return m_missingDerivations; }
      /// Access the combined IOV of the slice from the pool
      const IOV& iov()  const;
      /// Clear the conditions content and the user pool.
      void reset();

      /// Insert a set of conditions to the slice AND register them to the conditions manager.
      /** Note: The conditions already require a valid hash key                           */
      template <typename T> void manage(const T& conds, ManageFlag flg)  {
        if ( !conds.empty() )  {
          ConditionsPool* p = (flg&REGISTER_MANAGER) ? manager.registerIOV(pool->validity()) : 0;
          for( const auto& e : conds )
            manage(p, select_cond(e), flg);
        }
      }
      /// Insert a set of conditions to the slice AND register them to the conditions manager.
      /** Note: The conditions already require a valid hash key                           */
      virtual bool manage(Condition condition, ManageFlag flg);

      /// Access all conditions from a given detector element
      std::vector<Condition> get(DetElement detector)  const;
      
      /** ConditionsMap interface implementation:                                         */
      /// ConditionsMap overload: Add a condition directly to the slice
      virtual bool insert(DetElement detector, Condition::itemkey_type key, Condition condition)  override;
      /// ConditionsMap overload: Access a condition
      virtual Condition get(DetElement detector, Condition::itemkey_type key)  const override;
      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      virtual std::vector<Condition> get(DetElement detector,
                                         Condition::itemkey_type lower,
                                         Condition::itemkey_type upper)  const  override;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(const Condition::Processor& processor) const  override;
      /// ConditionsMap overload: Interface to partially scan data content of the conditions mapping
      virtual void scan(DetElement       detector,
                        Condition::itemkey_type     lower,
                        Condition::itemkey_type     upper,
                        const Condition::Processor& processor) const  override;
    };

    /// Populate the conditions slice from the conditions manager (convenience)
    void fill_content(ConditionsManager mgr, ConditionsContent& content, const IOVType& typ);

  }        /* End namespace cond               */
}          /* End namespace dd4hep                   */
#endif     /* DD4HEP_DDCOND_CONDITIONSSLICE_H        */
