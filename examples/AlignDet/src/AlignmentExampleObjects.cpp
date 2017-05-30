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

// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Objects.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;

/// Install the consitions and the alignment manager
void DD4hep::AlignmentExamples::installManagers(LCDD& lcdd)  {
  // Now we instantiate the conditions manager
  lcdd.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);
}

/// Callback to process a single detector element
int AlignmentDataAccess::processElement(DetElement de)  {
  DetElementAlignmentsCollector select(de);
  mapping->scan(select);

  // Let's go for the deltas....
  for(const auto& align : select.alignments )  {
    const Delta& delta = align.data().delta;
    if ( delta.hasTranslation() || delta.hasPivot() || delta.hasRotation() )  {}
  }
  // Keep it simple. To know how to access stuff,
  // simply look in DDDCore/src/AlignmentsPrinter.cpp...
  Alignments::printElementPlacement(printLevel,"Example",de,*mapping);
  return 1;
}

/// Callback to process a single detector element
int AlignmentCreator::operator()(DetElement de, int)    {
  if ( de.ptr() != de.world().ptr() )  {
    Condition     cond(de.path()+"#alignment_delta", "alignment_delta");
    Delta&        delta = cond.bind<Delta>();
    cond->hash = ConditionKey::hashCode(de,"alignment_delta");
    cond->setFlag(Condition::ACTIVE|Condition::ALIGNMENT_DELTA);
    /// Simply move everything by 1 mm in z. Not physical, but this is just an example...
    delta.translation.SetZ(delta.translation.Z()+0.1*dd4hep::cm);
    delta.rotation = RotationZYX(0.999,1.001,0.999);
    delta.flags |= Delta::HAVE_TRANSLATION|Delta::HAVE_ROTATION;
    manager.registerUnlocked(pool, cond);
    printout(printLevel,"Example","++ Adding manually alignments for %s",de.path().c_str());
  }
  return 1;
}
