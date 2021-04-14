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

/// Framework include files
#include "MyTrackerHit.h"
#include "G4Track.hh"

using namespace SomeExperiment;

/// Assignment operator
void MyTrackerHit::copyFrom(const MyTrackerHit& c)  {
  if ( &c != this )  {
    this->dd4hep::sim::Geant4Tracker::Hit::copyFrom(c);
    this->step_length = c.step_length;
    this->postPos = c.postPos;
    this->prePos = c.prePos;
  }
}
