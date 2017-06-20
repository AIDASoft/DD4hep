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
#ifndef DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H
#define DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/AlignmentsCalculator.h"

#include "DDCond/ConditionsSlice.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for alignment examples
  namespace AlignmentExamples {

    using cond::ConditionsPool;
    using cond::ConditionsSlice;
    using cond::ConditionsContent;
    using cond::ConditionsManager;
    using cond::conditionsCollector;

    using align::AlignmentsPrinter;
    using align::AlignmentsCalculator;
    using align::AlignedVolumePrinter;
    using align::DeltaCollector;
    using align::deltaCollector;
    using align::alignmentsCollector;
    
    /// Example how to populate the detector description with alignment constants
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    class AlignmentCreator {
    public:
      /// Reference to the conditions manager
      ConditionsManager manager;
      /// Reference to the used conditions pool
      ConditionsPool&   pool;
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      AlignmentCreator(ConditionsManager m, ConditionsPool& p)
        : manager(m), pool(p), printLevel(DEBUG) {}
      /// Callback to process a single detector element
      int operator()(DetElement de, int level)  const;
    };

    /// Example how to access the alignment constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    class AlignmentDataAccess {
    public:
      ConditionsMap& mapping;
      /// Print level
      PrintLevel printLevel;
      /// Constructor
      AlignmentDataAccess(ConditionsMap& m) : mapping(m), printLevel(DEBUG) {}
      /// Callback to process a single detector element
      int operator()(DetElement de, int level)  const;
    };

    /// Helper to run DetElement scans
    typedef DetectorScanner Scanner;

    /// Install the consitions and the alignment manager
    ConditionsManager installManager(Detector& description);
  }       /* End namespace AlignmentExamples           */
}         /* End namespace dd4hep                      */
#endif    /* DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H */
