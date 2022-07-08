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
#ifndef EXAMPLES_CONDITIONS_SRC_CONDITIONANYEXAMPLEOBJECTS_H
#define EXAMPLES_CONDITIONS_SRC_CONDITIONANYEXAMPLEOBJECTS_H

// Framework include files
#include "ConditionExampleObjects.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for conditions examples
  namespace ConditionExamples {

    /// Specialized conditions update callback 
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionAnyUpdate1 : public ConditionUpdateCall, public OutputLevel  {
    public:
      /// Initializing constructor
      ConditionAnyUpdate1(PrintLevel p) : OutputLevel(p) {    }
      /// Default destructor
      virtual ~ConditionAnyUpdate1() = default;
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
    class ConditionAnyUpdate2 : public ConditionUpdateCall, public OutputLevel  {
    public:
      /// Initializing constructor
      ConditionAnyUpdate2(PrintLevel p) : OutputLevel(p) {    }
      /// Default destructor
      virtual ~ConditionAnyUpdate2() = default;
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext& context) override   final;
      /// Interface to client Callback in order to update the condition
      virtual void resolve(Condition condition, ConditionUpdateContext& context) override  final;
    };

    /// This is important, otherwise the register and forward calls won't find them!
    /**
     *  \author  M.Frank
     *  \version 1.02
     *  \date    01/04/2016
     */
    class ConditionsAnyKeys : public OutputLevel {
    public:
      /// Content object to be filled
      ConditionsContent& content;
      /// Constructor
      ConditionsAnyKeys(ConditionsContent& c, PrintLevel p) : OutputLevel(p), content(c) {}
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
    class ConditionsAnyDependencyCreator : public OutputLevel  {
    public:
      /// Content object to be filled
      ConditionsContent&   content;
      /// Three different update call types
      std::shared_ptr<ConditionUpdateCall> scall1, call1, call2, call3, call4, call5, call6, callUnresolved;
      /// Flag for special setup for ROOT persistency
      bool persist_conditions;
      /// Flag to indicate increased complexity
      int  extended;
    public:
      /// Constructor
      ConditionsAnyDependencyCreator(ConditionsContent& c, PrintLevel p, bool persist=false, int extended=0);
      /// Destructor
      virtual ~ConditionsAnyDependencyCreator() = default;
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
    class ConditionsAnyCreator  : public OutputLevel  {
    public:
      /// Content object for which conditions are supposedly created
      ConditionsSlice& slice;
      /// Conditions pool the created conditions are inserted to (not equals user pool!)
      ConditionsPool&  pool;
    public:
      /// Constructor
      ConditionsAnyCreator(ConditionsSlice& s, ConditionsPool& p, PrintLevel l=DEBUG)
        : OutputLevel(l), slice(s), pool(p)  {}
      /// Destructor
      virtual ~ConditionsAnyCreator() = default;
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  const final;
      template<typename T>
      Condition make_condition(DetElement de,
			       const std::string& name,
			       const T& val)  const;
      template<typename T, typename... Args>
      Condition make_condition_args(DetElement de,
				    const std::string& name,
				    Args... args)  const;
    };

    /// Example how to access the conditions constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    class ConditionsAnyDataAccess : public OutputLevel   {
    public:
      /// Reference to the IOV to be checked
      const IOV&     iov;
      /// Reference to the conditions map to access conditions
      ConditionsMap& map;

    public:
      /// Constructor
      ConditionsAnyDataAccess(const IOV& i, ConditionsMap& m, PrintLevel l=DEBUG)
        : OutputLevel(l), iov(i), map(m) {}
      /// Destructor
      virtual ~ConditionsAnyDataAccess() = default;
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level)  const;
      /// Common call to access selected conditions
      virtual int accessConditions(DetElement de,
                                   const std::vector<Condition>& conditions)  const;
    };
  }       /* End namespace condExamples             */
}         /* End namespace dd4hep                         */
#endif // EXAMPLES_CONDITIONS_SRC_CONDITIONEXAMPLEOBJECTS_H
