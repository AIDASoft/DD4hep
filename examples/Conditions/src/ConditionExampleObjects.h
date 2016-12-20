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
#ifndef DD4HEP_CONDITIONS_CONDITIONSEXAMPLEOBJECTS_H
#define DD4HEP_CONDITIONS_CONDITIONSEXAMPLEOBJECTS_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/ConditionsProcessor.h"

#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for conditions examples
  namespace ConditionExamples {

    using Geometry::LCDD;
    using Geometry::RotationZYX;
    using Geometry::DetElement;
    using Geometry::DetectorProcessor;
    
    using Conditions::UserPool;
    using Conditions::Condition;
    using Conditions::ConditionKey;
    using Conditions::DetConditions;
    using Conditions::ConditionsPool;
    using Conditions::ConditionsSlice;
    using Conditions::ConditionsPrinter;
    using Conditions::ConditionsManager;
    using Conditions::ConditionUpdateCall;

    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdate1 : public ConditionUpdateCall  {
      /// Output level
      PrintLevel printLevel;
    public:
      /// Initializing constructor
      ConditionUpdate1(PrintLevel p) : printLevel(p) {    }
      /// Default destructor
      virtual ~ConditionUpdate1()                     {    }
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, const Context& context);
    };

    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdate2 : public ConditionUpdateCall  {
      /// Output level
      PrintLevel printLevel;
    public:
      /// Initializing constructor
      ConditionUpdate2(PrintLevel p) : printLevel(p) {    }
      /// Default destructor
      virtual ~ConditionUpdate2()                     {    }
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, const Context& context);
    };

    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdate3 : public ConditionUpdateCall  {
      /// Output level
      PrintLevel printLevel;
    public:
      /// Initializing constructor
      ConditionUpdate3(PrintLevel p) : printLevel(p) {    }
      /// Default destructor
      virtual ~ConditionUpdate3()                     {    }
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, const Context& context);
    };
    
    /// Example how to populate the detector description with conditions constants
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsCreator : public DetectorProcessor {
      /// reference to the conditions manager
      ConditionsManager manager;
      /// Reference to the conditiosn user pool
      ConditionsPool*   pool;
      ConditionsSlice*  slice;
      /// Counter
      unsigned int      conditionCount;
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      ConditionsCreator(ConditionsManager& m,ConditionsPool* p,PrintLevel l)
        : manager(m), pool(p), slice(0), conditionCount(0), printLevel(l)  {  }
      /// Destructor
      virtual ~ConditionsCreator();
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level);
      template<typename T> Condition make_condition(DetElement de, const std::string& name, T val);
    };

    /// Example how to populate the detector description with derived conditions
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsDependencyCreator : public DetectorProcessor {
      /// Reference to the conditiosn user pool
      ConditionsSlice&  slice;
      /// Print level
      PrintLevel        printLevel;
      /// Three different update call types
      ConditionUpdateCall *call1, *call2, *call3;
      /// Constructor
      ConditionsDependencyCreator(ConditionsSlice& s,PrintLevel p);
      /// Destructor
      virtual ~ConditionsDependencyCreator();
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level);
    };

    /// This is important, otherwise the register and forward calls won't find them!
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsKeys : public DetectorProcessor {
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      ConditionsKeys(PrintLevel p) : printLevel(p) {}
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level);
    };

    /// Example how to access the conditions constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsDataAccess : public Conditions::ConditionsProcessor  {
      /// IOV to be checked
      const IOV& iov;
      /// Print level
      PrintLevel printLevel;
      /// Constructor
      ConditionsDataAccess(const IOV& i, UserPool* p) :
        ConditionsProcessor(p), iov(i), printLevel(DEBUG) {  }
      /// Callback to process a single detector element
      int processElement(DetElement de);
    };

    /// Helper to run DetElement scans
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct Scanner : public DetectorProcessor  {
      DetElement::Processor* proc;
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int)
      { return proc->processElement(de);                     }
      template <typename Q> Scanner& scan(Q& p, DetElement start)
      { Scanner obj; obj.proc = &p; obj.process(start, 0, true); return *this; }
      template <typename Q> Scanner& scan2(const Q& p, DetElement start)
      { Scanner obj; obj.proc = const_cast<Q*>(&p); obj.process(start, 0, true); return *this; }
    };

    /// Install the consitions and the conditions manager
    void installManagers(LCDD& lcdd);
  }       /* End namespace ConditionsExamples             */
}         /* End namespace DD4hep                         */
#endif    /* DD4HEP_CONDITIONS_CONDITIONSEXAMPLEOBJECTS_H */
