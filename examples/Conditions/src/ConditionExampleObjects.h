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
#ifndef DD4HEP_CONDITIONS_CONDITIONSEXAMPLEOBJECTS_H
#define DD4HEP_CONDITIONS_CONDITIONSEXAMPLEOBJECTS_H

// Framework include files
#include "DD4hep/Detector.h"
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
namespace dd4hep {

  /// Namespace for conditions examples
  namespace ConditionExamples {

    using cond::UserPool;
    using cond::ConditionsPool;
    using cond::ConditionsSlice;
    using cond::ConditionsContent;
    using cond::ConditionsPrinter;
    using cond::ConditionsManager;
    using cond::ConditionUpdateCall;
    using cond::ConditionUpdateContext;
    using cond::conditionsCollector;
    
    /// Helper to reduce the number of lines of code
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class OutputLevel {
    public:
      /// Output level
      PrintLevel printLevel = DEBUG;
      /// Default constructor
      OutputLevel() = default;
      /// Initializing constructor
      OutputLevel(PrintLevel p) : printLevel(p) {}
      /// Default destructor
      virtual ~OutputLevel() = default;
    };
    
    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdate1 : public ConditionUpdateCall, public OutputLevel  {
    public:
      /// Initializing constructor
      ConditionUpdate1(PrintLevel p) : OutputLevel(p) {    }
      /// Default destructor
      virtual ~ConditionUpdate1() = default;
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext& context) override  final;
      /// Interface to client Callback in order to update the condition
      virtual void resolve(Condition condition, ConditionUpdateContext& context) override  final;
    };

    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdate2 : public ConditionUpdateCall, public OutputLevel  {
    public:
      /// Initializing constructor
      ConditionUpdate2(PrintLevel p) : OutputLevel(p) {    }
      /// Default destructor
      virtual ~ConditionUpdate2() = default;
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext& context) override   final;
      /// Interface to client Callback in order to update the condition
      virtual void resolve(Condition condition, ConditionUpdateContext& context) override  final;
    };

    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdate3 : public ConditionUpdateCall, public OutputLevel  {
    public:
      /// Initializing constructor
      ConditionUpdate3(PrintLevel p) : OutputLevel(p) {    }
      /// Default destructor
      virtual ~ConditionUpdate3() = default;
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext& context) override  final;
      /// Interface to client Callback in order to update the condition
      virtual void resolve(Condition condition, ConditionUpdateContext& context) override  final;
    };
    
    /// This is important, otherwise the register and forward calls won't find them!
    /**
     *  \author  M.Frank
     *  \version 1.02
     *  \date    01/04/2016
     */
    class ConditionsKeys : public OutputLevel {
    public:
      /// Content object to be filled
      ConditionsContent& content;
      /// Constructor
      ConditionsKeys(ConditionsContent& c, PrintLevel p) : OutputLevel(p), content(c) {}
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level) const final;
    };

    /// Example how to populate the detector description with derived conditions
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsDependencyCreator : public OutputLevel  {
      /// Content object to be filled
      ConditionsContent&   content;
      /// Three different update call types
      ConditionUpdateCall *call1, *call2, *call3;
      /// Constructor
      ConditionsDependencyCreator(ConditionsContent& c, PrintLevel p);
      /// Destructor
      virtual ~ConditionsDependencyCreator();
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level) const final;
    };

    /// Example how to populate the detector description with conditions constants
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsCreator  : public OutputLevel  {
      /// Content object for which conditions are supposedly created
      ConditionsSlice& slice;
      /// Conditions pool the created conditions are inserted to (not equals user pool!)
      ConditionsPool&  pool;
      /// Constructor
      ConditionsCreator(ConditionsSlice& s, ConditionsPool& p, PrintLevel l=DEBUG)
        : OutputLevel(l), slice(s), pool(p)  {}
      /// Destructor
      virtual ~ConditionsCreator() = default;
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  const final;
      template<typename T> Condition make_condition(DetElement de,
                                                    const std::string& name,
                                                    T val)  const;
    };

    /// Example how to access the conditions constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct ConditionsDataAccess : public OutputLevel   {
      /// Reference to the IOV to be checked
      const IOV&     iov;
      /// Reference to the conditions map to access conditions
      ConditionsMap& map;
      /// Constructor
      ConditionsDataAccess(const IOV& i, ConditionsMap& m, PrintLevel l=DEBUG)
        : OutputLevel(l), iov(i), map(m) {}
      /// Destructor
      virtual ~ConditionsDataAccess() = default;
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  const;
      /// Common call to access selected conditions
      virtual int accessConditions(DetElement de,
                                   const std::vector<Condition>& conditions)  const;
    };

    /// Helper to run DetElement scans
    typedef DetectorScanner Scanner;
    
    /// Install the consitions and the conditions manager
    ConditionsManager installManager(Detector& description);
  }       /* End namespace condExamples             */
}         /* End namespace dd4hep                         */
#endif    /* DD4HEP_CONDITIONS_CONDITIONSEXAMPLEOBJECTS_H */
