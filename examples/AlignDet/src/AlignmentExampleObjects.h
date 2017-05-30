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
#ifndef DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H
#define DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/AlignedVolumePrinter.h"
#include "DD4hep/AlignmentsCalculator.h"

#include "DDCond/ConditionsSlice.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for alignment examples
  namespace AlignmentExamples {

    using Geometry::LCDD;
    using Geometry::RotationZYX;
    using Geometry::DetElement;
    using Geometry::DetectorProcessor;
    using Geometry::DetElementProcessor;
    
    using Conditions::Condition;
    using Conditions::ConditionKey;
    using Conditions::ConditionsMap;
    using Conditions::ConditionsPool;
    using Conditions::ConditionsSlice;
    using Conditions::ConditionsContent;
    using Conditions::ConditionsManager;
    using Conditions::DetElementConditionsCollector;

    using Alignments::Delta;
    using Alignments::Alignment;
    using Alignments::AlignmentData;
    using Alignments::AlignmentsCalculator;
    using Alignments::DetElementDeltaCollector;
    using Alignments::DetElementAlignmentsCollector;
    
    /// Example how to populate the detector description with alignment constants
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct AlignmentCreator : public DetectorProcessor {
      /// Reference to the conditions manager
      ConditionsManager manager;
      /// Reference to the used conditions pool
      ConditionsPool&   pool;
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      AlignmentCreator(ConditionsManager m,ConditionsPool& p)
        : manager(m), pool(p), printLevel(DEBUG) {}
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level);
    };

    /// Example how to access the alignment constants from a detector element
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct AlignmentDataAccess : public Alignments::AlignmentsProcessor  {
      /// Print level
      PrintLevel printLevel;
      /// Constructor
      AlignmentDataAccess(ConditionsMap& p) : AlignmentsProcessor(&p), printLevel(DEBUG) {}
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
      virtual int operator()(DetElement de, int)      {
        return proc->processElement(de);
      }
      template <typename Q> Scanner& scan(Q& p, DetElement start)      {
        Scanner obj;
        obj.proc = &p;
        obj.process(start, 0, true);
        return *this;
      }
      template <typename Q> Scanner& scan(const Q& p, DetElement start)  {
        Scanner obj;
        Q* q = const_cast<Q*>(&p);
        obj.proc = q;
        obj.process(start, 0, true); return *this;
      }
    };

    /// Install the consitions and the alignment manager
    void installManagers(LCDD& lcdd);
  }       /* End namespace AlignmentExamples           */
}         /* End namespace DD4hep                      */
#endif    /* DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H */
