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
#ifndef DD4HEP_DDALIGN_ALIGNMENTCALCULATOR_H
#define DD4HEP_DDALIGN_ALIGNMENTCALCULATOR_H

// Framework include files
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/ConditionsMap.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    /// Alignment calculator instance to handle alignment dependencies
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentsCalculator  {
    public:
      typedef std::map<DetElement, Delta> Deltas;
      typedef Conditions::ConditionsMap   Alignments;
      
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

    public:

      /// Default constructor
      AlignmentsCalculator() = default;
      /// Copy constructor
      AlignmentsCalculator(const AlignmentsCalculator& copy) = delete;
      /// Assignment operator
      AlignmentsCalculator& operator=(const AlignmentsCalculator& mgr) = delete;
      /// Access the default alignment name
      static unsigned int alignment_item_key();
      /// Access the default alignment name
      static unsigned int alignment_delta_item_key();
      /// Compute all alignment conditions of the internal dependency list
      Result compute(const Deltas& deltas, Alignments& alignments)  const;
    };

    /// Add results
    inline AlignmentsCalculator::Result&
    AlignmentsCalculator::Result::operator +=(const Result& result)  {
      computed += result.computed;
      missing  += result.missing;
      return *this;
    }
    /// Subtract results
    inline AlignmentsCalculator::Result&
    AlignmentsCalculator::Result::operator -=(const Result& result)  {
      computed -= result.computed;
      missing  -= result.missing;
      return *this;
    }

  }       /* End namespace Alignments                  */
}         /* End namespace DD4hep                      */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTCALCULATOR_H      */
