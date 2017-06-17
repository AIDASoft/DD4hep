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

// Framework include files
#include "AlignmentExampleObjects.h"
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Objects.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::AlignmentExamples;

/// Install the consitions and the alignment manager
ConditionsManager dd4hep::AlignmentExamples::installManager(Detector& description)  {
  // Now we instantiate the conditions manager
  description.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);
  ConditionsManager manager = ConditionsManager::from(description);
  manager["PoolType"]       = "DD4hep_ConditionsLinearPool";
  manager["UserPoolType"]   = "DD4hep_ConditionsMapUserPool";
  manager["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
  manager.initialize();
  return manager;
}

/// Callback to process a single detector element
int AlignmentDataAccess::operator()(DetElement de, int) const {
  vector<Alignment> alignments;
  alignmentsCollector(mapping,alignments)(de);

  // Let's go for the deltas....
  for(const auto& align : alignments )  {
    const Delta& delta = align.data().delta;
    if ( delta.hasTranslation() || delta.hasPivot() || delta.hasRotation() )  {}
  }
  // Keep it simple. To know how to access stuff,
  // simply look in DDDCore/src/AlignmentsPrinter.cpp...
  align::printElementPlacement(printLevel,"Example",de,mapping);
  return 1;
}

/// Callback to process a single detector element
int AlignmentCreator::operator()(DetElement de, int)  const  {
  if ( de.ptr() != de.world().ptr() )  {
    Condition cond(de.path()+"#"+align::Keys::deltaName,align::Keys::deltaName);
    Delta&    delta = cond.bind<Delta>();
    cond->hash = ConditionKey(de.key(),align::Keys::deltaKey).hash;
    cond->setFlag(Condition::ACTIVE|Condition::ALIGNMENT_DELTA);
    /// Simply move everything by 1 mm in z. Not physical, but this is just an example...
    delta.translation.SetZ(delta.translation.Z()+0.1*dd4hep::cm);
    delta.rotation = RotationZYX(0.999,1.001,0.999);
    delta.flags |= Delta::HAVE_TRANSLATION|Delta::HAVE_ROTATION;
    if ( !manager.registerUnlocked(pool, cond) )   {
      printout(WARNING,"Example","++ Failed to register condition %s.",cond.name());
    }
    printout(printLevel,"Example","++ Adding manually alignments for %s",cond.name());
  }
  return 1;
}
