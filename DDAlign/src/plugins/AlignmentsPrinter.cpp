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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetFactoryHelper.h"
//#include "DD4hep/Conditions.h"
//#include "DD4hep/DetAlign.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/objects/AlignmentsInterna.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

// ======================================================================================

namespace {
  /// Print alignments
  /**
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    18/11/2016
   *   \ingroup DD4HEP_DDALIGN
   */
  class AlignmentPrinter : public AlignmentsProcessor {
  public:
    /// Initializing constructor
    AlignmentPrinter() : AlignmentsProcessor(0) {}
    /// Default destructor
    virtual ~AlignmentPrinter() = default;
    /// Callback to output conditions information
    virtual int operator()(Alignment a)  {
      const Alignments::Delta& D = a.data().delta;
      printout(INFO,"Alignment","++ (%11s-%8s-%5s) Cond:%p 'Alignment'",
               D.hasTranslation() ? "Translation" : "",
               D.hasRotation() ? "Rotation" : "",
               D.hasPivot() ? "Pivot" : "",
               a.data().hasCondition() ? a.data().condition.ptr() : 0);
      return 1;
    }
    /// Container callback for object processing
    virtual int operator()(Container container)
    {  return this->self_t::operator()(container);  }
    /// Callback to output conditions information
    virtual int operator()(DetElement de)
    {  return this->self_t::operator()(de);         }
  };
}

/// Convert alignments conditions to alignment objects
static void* ddalign_AlignmentsPrinter(Geometry::LCDD& /* lcdd */, int /* argc */, char** /* argv */)  {
  return (AlignmentsProcessor*)(new AlignmentPrinter()); 
}

DECLARE_LCDD_CONSTRUCTOR(DDAlign_AlignmentsPrinter,ddalign_AlignmentsPrinter)
