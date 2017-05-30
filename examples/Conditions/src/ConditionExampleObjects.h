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
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/DetectorProcessor.h"

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
    using Conditions::ConditionsMap;
    using Conditions::ConditionsPool;
    using Conditions::ConditionsSlice;
    using Conditions::ConditionsContent;
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
      virtual Condition operator()(const ConditionKey& key, const Context& context) final;
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
      virtual Condition operator()(const ConditionKey& key, const Context& context)  final;
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
      virtual Condition operator()(const ConditionKey& key, const Context& context)  final;
    };
    
    /// This is important, otherwise the register and forward calls won't find them!
    /**
     *  \author  M.Frank
     *  \version 1.02
     *  \date    01/04/2016
     */
    struct ConditionsKeys : public DetectorProcessor {
      /// Content object to be filled
      ConditionsContent& content;
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      ConditionsKeys(ConditionsContent& c, PrintLevel p) : content(c), printLevel(p) {}
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  final;
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
      /// Content object to be filled
      ConditionsContent& content;
      /// Print level
      PrintLevel        printLevel;
      /// Three different update call types
      ConditionUpdateCall *call1, *call2, *call3;
      /// Constructor
      ConditionsDependencyCreator(ConditionsContent& c, PrintLevel p);
      /// Destructor
      virtual ~ConditionsDependencyCreator();
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  final;
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
      /// Content object for which conditions are supposedly created
      ConditionsSlice&   slice;
      /// Conditions pool the created conditions are inserted to (not equals user pool!)
      ConditionsPool&    pool;
      /// Counter
      unsigned int       conditionCount = 0;
      /// Print level
      PrintLevel         printLevel = DEBUG;

      /// Constructor
      ConditionsCreator(ConditionsSlice& s, ConditionsPool& p, PrintLevel l);
      /// Destructor
      virtual ~ConditionsCreator();
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  final;
      template<typename T> Condition make_condition(DetElement de, const std::string& name, T val);
    };

    /// Example how to access the conditions constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsDataAccess : public DetectorProcessor  {
      /// Reference to the IOV to be checked
      const IOV&     iov;
      /// Reference to the conditions map to access conditions
      ConditionsMap& map;
      /// Print level
      PrintLevel printLevel = DEBUG;
      /// Constructor
      ConditionsDataAccess(const IOV& i, ConditionsMap& m) : iov(i), map(m), printLevel(DEBUG) {}
      /// Destructor
      virtual ~ConditionsDataAccess();
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level);
      /// Common call to access selected conditions
      virtual int accessConditions(DetElement de, const std::vector<Condition>& conditions);
    };

    /// Example how to access the conditions constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct DetectorConditionsAccess : public ConditionsDataAccess  {
      /// Constructor
      DetectorConditionsAccess(const IOV& i, ConditionsMap& m) : ConditionsDataAccess(i,m) {}
      /// Destructor
      virtual ~DetectorConditionsAccess() {}
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  final;
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
      virtual int operator()(DetElement de, int) final
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
