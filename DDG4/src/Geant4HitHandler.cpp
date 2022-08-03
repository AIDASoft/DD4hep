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
#include <DDG4/Geant4HitHandler.h>
#include <DD4hep/DD4hepUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

// Geant4 include files
#include <G4NavigationHistory.hh>

using namespace dd4hep;
using namespace dd4hep::sim;

/// Coordinate transformation to global coordinate.
Position Geant4HitHandler::localToGlobal(const DDSegmentation::Vector3D& local)  const   {
  return localToGlobal(G4ThreeVector(local.X / dd4hep::mm,local.Y / dd4hep::mm,local.Z / dd4hep::mm));
}

/// Coordinate transformation to global coordinates.
Position Geant4HitHandler::localToGlobal(const Position& local)  const   {
  return localToGlobal(G4ThreeVector(local.X(),local.Y(),local.Z()));
}

/// Coordinate transformation to global coordinates
Position Geant4HitHandler::localToGlobal(double x, double y, double z)  const    {
  return localToGlobal(G4ThreeVector(x,y,z));
}

/// Coordinate transformation to global coordinates
Position Geant4HitHandler::localToGlobal(const G4ThreeVector& loc)  const    {
  G4ThreeVector p = touchable_ptr->GetHistory()->GetTopTransform().Inverse().TransformPoint(loc);
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
Position Geant4HitHandler::globalToLocal(double x, double y, double z)  const    {
  G4ThreeVector p = globalToLocalG4(G4ThreeVector(x,y,z));
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
Position Geant4HitHandler::globalToLocal(const Position& global)  const    {
  G4ThreeVector p = globalToLocalG4(G4ThreeVector(global.X(),global.Y(),global.Z()));
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
Position Geant4HitHandler::globalToLocal(const G4ThreeVector& global)  const    {
  G4ThreeVector p = globalToLocalG4(global);
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
G4ThreeVector Geant4HitHandler::globalToLocalG4(double x, double y, double z)  const    {
  return globalToLocalG4(G4ThreeVector(x,y,z));
}

/// Coordinate transformation to local coordinates
G4ThreeVector Geant4HitHandler::globalToLocalG4(const G4ThreeVector& global)  const    {
  return touchable_ptr->GetHistory()->GetTopTransform().TransformPoint(global);
}
