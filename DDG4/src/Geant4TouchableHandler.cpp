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
#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4GeometryInfo.h"

#include "G4Step.hh"
#include "G4VTouchable.hh"

// C/C++ include files
#include <stdexcept>

using namespace dd4hep::sim;

/// Default constructor. Takes the step's pre-touchable
Geant4TouchableHandler::Geant4TouchableHandler(const G4Step* step, bool use_post_step_point)  {
  const G4StepPoint* p = use_post_step_point ? step->GetPostStepPoint() : step->GetPreStepPoint();
  touchable = p->GetTouchable();
}

/// Default constructor. Takes the step's pre-touchable
Geant4TouchableHandler::Geant4TouchableHandler(const G4Step* step)  {
  touchable = step->GetPreStepPoint()->GetTouchable();
}

/// Touchable history depth
int Geant4TouchableHandler::depth() const   {
  return touchable->GetHistoryDepth();
}

/// Helper: Generate placement path from touchable object
Geant4TouchableHandler::Geant4PlacementPath Geant4TouchableHandler::placementPath(bool exception) const {
  Geant4PlacementPath path_val;
  if ( touchable )   {
    int i, n=touchable->GetHistoryDepth();
    path_val.reserve(n);
    for (i=0; i < n; ++i) {
      G4VPhysicalVolume* pv = touchable->GetVolume(i);
      path_val.emplace_back(pv);
    }
    return path_val;
  }
  if ( exception )   {
    throw std::runtime_error("Attempt to access invalid G4 touchable object.");
  }
  return path_val;
}

/// Helper: Access the placement path of a Geant4 touchable object as a string
std::string Geant4TouchableHandler::path()  const   {
  return Geant4GeometryInfo::placementPath(this->placementPath());
}

