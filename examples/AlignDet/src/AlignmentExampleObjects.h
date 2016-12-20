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
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/AlignedVolumePrinter.h"

#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsManager.h"
#include "DDAlign/AlignmentsManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for alignment examples
  namespace AlignmentExamples {

    using Geometry::LCDD;
    using Geometry::RotationZYX;
    using Geometry::DetElement;
    using Geometry::DetectorProcessor;
    
    using Conditions::UserPool;
    using Conditions::Condition;
    using Conditions::ConditionKey;
    using Conditions::ConditionsPool;
    using Conditions::ConditionsSlice;
    using Conditions::ConditionsManager;
    using Conditions::DetConditions;

    using Alignments::Delta;
    using Alignments::DetAlign;
    using Alignments::Alignment;
    using Alignments::AlignmentsManager;
    
    /// Example how to populate the detector description with alignment constants
    /**
     *  This is simply a DetElement crawler...
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct AlignmentCreator : public DetectorProcessor {
      ConditionsManager manager;
      ConditionsPool*   pool;
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      AlignmentCreator(ConditionsManager m,ConditionsPool* p)
        : manager(m), pool(p), printLevel(DEBUG) {}
      /// Callback to process a single detector element
      virtual int operator()(DetElement de, int level);
    };

    // This is important, otherwise the register and forward calls won't find them!
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    struct AlignmentKeys : public DetectorProcessor {
      /// Constructor
      AlignmentKeys() = default;
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
      UserPool& pool;
      /// Print level
      PrintLevel        printLevel;
      /// Constructor
      AlignmentDataAccess(UserPool& p) : AlignmentsProcessor(0), pool(p),
                                         printLevel(DEBUG) {
      }
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

    /// Install the consitions and the alignment manager
    void installManagers(LCDD& lcdd);
    /// Register the alignment callbacks
    void registerAlignmentCallbacks(LCDD& lcdd,
                                    ConditionsSlice& slice,
                                    Alignments::AlignmentsManager alignMgr);

    
  }       /* End namespace AlignmentExamples           */
}         /* End namespace DD4hep                      */
#endif    /* DD4HEP_ALIGNDET_ALIGNMENTEXAMPLEOBJECTS_H */
