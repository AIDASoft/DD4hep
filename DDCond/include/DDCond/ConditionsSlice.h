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
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

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
      typedef Condition::key_type                key_type;
      typedef std::shared_ptr<ConditionsContent> Content;

    public:
      /// Reference to the conditions manager.
      /** Not used by the object, simple for convenience.
       *  Then all actors are lumped together, which are used by the client code.
       */
      ConditionsManager    manager;
      /// Reference to the user pool managing all conditions of this slice
      std::unique_ptr<UserPool> pool;
      /// Container of conditions required by this slice
      Content              content;

    protected:
      /// If flag conditonsManager["OutputUnloadedConditions"]=true: will contain conditions not loaded
      ConditionsContent::Conditions   m_missingConditions;
      /// If flag conditonsManager["OutputUnloadedConditions"]=true: will contain conditions not computed
      ConditionsContent::Dependencies m_missingDerivations;
      
      /// Default assignment operator
      ConditionsSlice& operator=(const ConditionsSlice& copy) = delete;

    public:
      /// Default constructor
      ConditionsSlice() = delete;
      /// Initializing constructor
      ConditionsSlice(ConditionsManager m);
      /// Copy constructor (Special, partial copy only. Hence no assignment!)
      ConditionsSlice(const ConditionsSlice& copy);
      /// Default destructor. 
      virtual ~ConditionsSlice();
      /// Access the map of missing conditions (only valid after preparation)
      ConditionsContent::Conditions& missingConditions()  { return m_missingConditions; }
      /// Access the map of missing computational conditions (only valid after preparation)
      ConditionsContent::Dependencies& missingDerivations() { return m_missingDerivations;}
      /// Clear the conditions content and the user pool.
      void reset();
      /// ConditionsMap overload: Add a condition directly to the slice
      virtual bool insert(DetElement detector, unsigned int key, Condition condition);
      /// ConditionsMap overload: Access a condition
      virtual Condition get(DetElement detector, unsigned int key)  const;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(Condition::Processor& processor) const;
    };

    /// Populate the conditions slice from the conditions manager (convenience)
    ConditionsSlice* createSlice(ConditionsManager mgr, const IOVType& typ);
  }        /* End namespace Conditions               */
}          /* End namespace DD4hep                   */
#endif     /* DD4HEP_DDCOND_CONDITIONSSLICE_H        */
