// $Id: Geant4StepHandler.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4GeometryInfo.h"

#include "G4Step.hh"
#include "G4VTouchable.hh"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

/// Default constructor. Takes the step's pre-touchable
Geant4TouchableHandler::Geant4TouchableHandler(const G4Step* step)  {
  touchable = step->GetPreStepPoint()->GetTouchable();
}

/// Helper: Generate placement path from touchable object
Geant4TouchableHandler::Geant4PlacementPath Geant4TouchableHandler::placementPath(bool exception) const {
  Geant4PlacementPath path;
  if ( touchable )   {
    int i, n=touchable->GetHistoryDepth();
    path.reserve(n);
    for (i=0; i < n; ++i) {
      G4VPhysicalVolume* pv = touchable->GetVolume(i);
      path.push_back(pv);
    }
    return path;
  }
  if ( exception )   {
    throw std::runtime_error("Attempt to access invalid G4 touchable object.");
  }
  return path;
}

/// Helper: Access the placement path of a Geant4 touchable object as a string
std::string Geant4TouchableHandler::path()  const   {
  return Geant4GeometryMaps::placementPath(this->placementPath());
}

