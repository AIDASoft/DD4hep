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
#include "DD4hep/AlignmentsPrinter.h"
#include "AlignmentExampleObjects.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Objects.h"

#include "DDAlign/DDAlignResetCall.h"
#include "DDAlign/DDAlignUpdateCall.h"
#include "DDAlign/DDAlignForwardCall.h"
#include "DDAlign/AlignmentsRegister.h"
#include "DDAlign/AlignmentsForward.h"
#include "DDAlign/AlignmentsReset.h"
#include "DDCond/ConditionsSlice.h"


using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;

/// Install the consitions and the alignment manager
void DD4hep::AlignmentExamples::installManagers(LCDD& lcdd)  {
  // Now we instantiate the conditions manager
  lcdd.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);
  // Now we instantiate the alignments manager
  lcdd.apply("DD4hep_AlignmentsManagerInstaller",0,(char**)0);
}

/// Register the alignment callbacks
void DD4hep::AlignmentExamples::registerAlignmentCallbacks(LCDD& lcdd,ConditionsSlice& slice)
{
  // Let's register the callbacks to compute dependent conditions/alignments
  // 1) Create dependencies for all deltas found in the conditions
  Alignments::AlignmentsRegister reg(slice,new Alignments::DDAlignUpdateCall());
  Scanner().scan(reg,lcdd.world());
  // 2) Create child dependencies if higher level alignments exist
  Alignments::AlignmentsForward  fwd(slice,new Alignments::DDAlignForwardCall());
  Scanner().scan(fwd,lcdd.world());
}

/// Register the alignment callbacks
void DD4hep::AlignmentExamples::registerResetCallbacks(LCDD& lcdd,ConditionsSlice& slice)
{
  // Let's register the callbacks to compute dependent conditions/alignments
  // 1) Create dependencies for all deltas found in the conditions
  Alignments::AlignmentsReset reg(slice,new Alignments::DDAlignResetCall());
  Scanner().scan(reg,lcdd.world());
  // 2) Create child dependencies if higher level alignments exist
  Alignments::AlignmentsForward  fwd(slice,new Alignments::DDAlignForwardCall());
  Scanner().scan(fwd,lcdd.world());
}

/// Callback to process a single detector element
int AlignmentDataAccess::processElement(DetElement de)  {
  DetAlign     a(de); // Use special facade...
  Alignments::Container container = a.alignments();
  // Let's go for the deltas....
  for(const auto& k : container.keys() )  {
    Alignment align    = container.get(k.first,pool);
    const Delta& delta = align.data().delta;
    if ( delta.hasTranslation() || delta.hasPivot() || delta.hasRotation() )  {}
  }
  // Keep it simple. To know how to access stuff,
  // simply look in DDDCore/src/AlignmentsPrinter.cpp...
  Alignments::printElementPlacement(printLevel,"Example",de,&pool);
  return 1;
}

/// Callback to process a single detector element
int AlignmentReset::processElement(DetElement de)    {
  DetAlign     a(de); // Use special facade...
  Alignments::Container container = a.alignments();

  for(const auto& k : container.keys() )  {
    Alignment      align = container.get(k.first,pool);
    AlignmentData& data  = align.data();
    Delta&         delta = data.delta;
    // Leave the flags to get the proper printout!
    //delta.flags &= ~(Delta::HAVE_TRANSLATION|Delta::HAVE_ROTATION|Delta::HAVE_PIVOT);
    delta.translation  = Geometry::Position();
    delta.rotation     = Geometry::RotationZYX();
    delta.pivot        = Delta::Pivot();
    data.worldDelta    = *Geometry::identityTransform();
    data.worldTrafo    = *Geometry::identityTransform();
    data.detectorTrafo = *Geometry::identityTransform();
    data.trToWorld     = Geometry::Transform3D();
    data.nodes.clear();
  }
  printout(printLevel,"Example","++ Reset alignment deltas for %s [%d keys]",
           de.path().c_str(), int(container.keys().size()));
  return 1;
}

/// Callback to process a single detector element
int AlignmentCreator::operator()(DetElement de, int)    {
  if ( de.ptr() != de.world().ptr() )  {
    DetConditions dc(de);
    Condition     cond(de.path()+"#alignment", "alignment");
    Delta&        delta = cond.bind<Delta>();
    cond->hash = ConditionKey::hashCode(cond->name);
    cond->setFlag(Condition::ACTIVE|Condition::ALIGNMENT);
    /// Simply move everything by 1 mm in z. Not physical, but this is just an example...
    delta.translation.SetZ(delta.translation.Z()+0.1*dd4hep::cm);
    delta.rotation = RotationZYX(0.999,1.001,0.999);
    delta.flags |= Delta::HAVE_TRANSLATION|Delta::HAVE_ROTATION;
    manager.registerUnlocked(pool, cond);
    printout(printLevel,"Example","++ Adding manually alignments for %s",de.path().c_str());
  }
  return 1;
}

/// Callback to process a single detector element
int AlignmentKeys::operator()(DetElement de, int)    {
  if ( de.ptr() != de.world().ptr() )  {
    DetConditions dc(de);
    dc.conditions().addKey(de.path()+"#alignment");
  }
  return 1;
}
