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
#ifndef DD4HEP_ALIGNMENTSCALCULATOR_H
#define DD4HEP_ALIGNMENTSCALCULATOR_H

// Framework include files
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/ConditionsMap.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace cond   { class ConditionUpdateContext;  }
  
  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace align {

    /// Alignment calculator instance to handle alignment dependencies
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentsCalculator  {
    public:

      /// Object encapsulating the result of a computation call to the alignments calculator
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Result  {
      public:
        size_t computed = 0;
        size_t missing  = 0;
	size_t multiply = 0;
        Result() = default;
        /// Copy constructor
        Result(const Result& result) = default;
        /// Assignment operator
        Result& operator  =(const Result& result) = default;
        /// Add results
        Result& operator +=(const Result& result);
        /// Subtract results
        Result& operator -=(const Result& result);
        /// Summed counters
        size_t total() const { return computed+missing; }
      };

      /// Functor for path ordered maps as they are needed for the calculator
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class PathOrdering {
      public:
        bool operator()(const DetElement& a, const DetElement& b) const
        { return a.path() < b.path(); }
      };

      typedef std::map<DetElement,const Delta*,PathOrdering> OrderedDeltas;
      typedef std::map<Condition::key_type,DetElement>       ExtractContext;

      /// Scanner to find all alignment deltas in the detector hierarchy
      /**
       *  The deltas are collected in the appropriate container suited for the
       *  calculator object, so that no re-ordering is necessary.
       *  
       *  Usage:
       *
       *  /// Interface to client Callback in order to update the condition
       *  Condition AlignmentCall::operator()(const ConditionKey& key,
       *                                      ConditionUpdateContext& ctxt)  {
       *  ....
       *   DetectorScanner().scan(AlignmentsCalculator::Scanner(ctxt,deltas),top);
       *  ....
       *  }
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Scanner  {
      public:
        /// Reference to the user pool taking into account IOV intersections
        cond::ConditionUpdateContext& context;
        /// Collection container
        OrderedDeltas&                deltas;
        /// Resulting IOV
        IOV*                          iov = 0;
      public:
        /// Default constructor
        Scanner() = delete;
        /// Initializing constructor
        Scanner(cond::ConditionUpdateContext& m, OrderedDeltas& d) : context(m), deltas(d) {}
        /// Initializing constructor with IOV specification
        Scanner(cond::ConditionUpdateContext& m, OrderedDeltas& d, IOV* i) : context(m), deltas(d), iov(i) {}
        /// Default move constructor is disabled
        Scanner(cond::ConditionUpdateContext& m, OrderedDeltas&& p) = delete;
        /// R-value copy from a temporary
        Scanner(Scanner&& copy) = delete;
        /// Copy constructor
        Scanner(const Scanner& copy) = delete;
        /// Default destructor
        ~Scanner() = default;
        /// Assignment operator
        Scanner& operator=(Scanner&& copy) = delete;
        /// Assignment operator
        Scanner& operator=(const Scanner& copy) = delete;
        /// Callback to output alignments information
        int operator()(DetElement de, int)  const;
      };

    public:

      /// Default constructor
      AlignmentsCalculator() = default;
      /// Copy constructor
      AlignmentsCalculator(const AlignmentsCalculator& copy) = delete;
      /// Assignment operator
      AlignmentsCalculator& operator=(const AlignmentsCalculator& mgr) = delete;
      /// Compute all alignment conditions of the internal dependency list
      Result compute(const std::map<DetElement, Delta>& deltas,
                     ConditionsMap& alignments)  const;
      /// Compute all alignment conditions of the internal dependency list
      Result compute(const std::map<DetElement, const Delta*>& deltas,
                     ConditionsMap& alignments)  const;
      /// Optimized call using already properly ordered Deltas
      Result compute(const OrderedDeltas& deltas, ConditionsMap& alignments)  const;

      /// Helper: Extract all Delta-conditions from the conditions map
      size_t extract_deltas(cond::ConditionUpdateContext& context,
                            OrderedDeltas& deltas,
                            IOV* effective_iov=0)   const;
      /// Helper: Extract all Delta-conditions from the conditions map starting at a certain sub-tree
      size_t extract_deltas(DetElement start,
                            cond::ConditionUpdateContext& ctxt,
                            OrderedDeltas& deltas,
                            IOV* effective_iov=0)   const;
      /// Helper: Extract all Delta-conditions from the conditions map
      /** If the extraction context is empty, it shall be filled.
       *  On every subsequent call the existing context is used and 
       *  the Delta-conditions are extracted directly using a linear scan 
       *  of the conditions map. Depending on the size of the conditons map
       *  this can lead to significant speed improvements.
       */
      size_t extract_deltas(cond::ConditionUpdateContext& context,
                            ExtractContext& extract_context,
                            OrderedDeltas& deltas,
                            IOV* effective_iov=0)   const;

      /// Helper: Extract all Delta-conditions from the conditions map starting at a certain sub-tree
      /**
       *  Please note: An extract_context is only valid for one sub-tree.
       */
      size_t extract_deltas(DetElement start,
                            cond::ConditionUpdateContext& ctxt,
                            ExtractContext& extract_context,
                            OrderedDeltas& deltas,
                            IOV* effective_iov=0)   const;
    };

    /// Add results
    inline AlignmentsCalculator::Result&
    AlignmentsCalculator::Result::operator +=(const Result& result)  {
      multiply += result.multiply;
      computed += result.computed;
      missing  += result.missing;
      return *this;
    }
    /// Subtract results
    inline AlignmentsCalculator::Result&
    AlignmentsCalculator::Result::operator -=(const Result& result)  {
      multiply -= result.multiply;
      computed -= result.computed;
      missing  -= result.missing;
      return *this;
    }

  }       /* End namespace align                  */
}         /* End namespace dd4hep                      */
#endif // DD4HEP_ALIGNMENTSCALCULATOR_H
