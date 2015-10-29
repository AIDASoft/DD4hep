// $Id$
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
#include "DDG4/Geant4StepHandler.h"
#include "DDSegmentation/Segmentation.h"
#include "DD4hep/DD4hepUnits.h"

using namespace DD4hep::Simulation;

/// Returns the step status in form of a string
const char* Geant4StepHandler::stepStatus(G4StepStatus status) {
  switch (status) {
    // Step reached the world boundary
  case fWorldBoundary:
    return "WorldBoundary";
    // Step defined by a geometry boundary
  case fGeomBoundary:
    return "GeomBoundary";
    // Step defined by a PreStepDoItVector
  case fAtRestDoItProc:
    return "AtRestDoItProc";
    // Step defined by a AlongStepDoItVector
  case fAlongStepDoItProc:
    return "AlongStepDoItProc";
    // Step defined by a PostStepDoItVector
  case fPostStepDoItProc:
    return "PostStepDoItProc";
    // Step defined by the user Step limit in the logical volume
  case fUserDefinedLimit:
    return "UserDefinedLimit";
    // Step defined by an exclusively forced PostStepDoIt process
  case fExclusivelyForcedProc:
    return "ExclusivelyForcedProc";
    // Step not defined yet
  case fUndefined:
  default:
    return "Undefined";
  };
}

/// Returns the pre-step status in form of a string
const char* Geant4StepHandler::preStepStatus() const {
  return stepStatus(pre ? pre->GetStepStatus() : fUndefined);
}

/// Returns the post-step status in form of a string
const char* Geant4StepHandler::postStepStatus() const {
  return stepStatus(post ? post->GetStepStatus() : fUndefined);
}

/// Coordinate transformation to global coordinate.
Position Geant4StepHandler::localToGlobal(const DDSegmentation::Vector3D& local)  const   {
  return localToGlobal(G4ThreeVector(local.X / dd4hep::mm,local.Y / dd4hep::mm,local.Z / dd4hep::mm));
}

/// Coordinate transformation to global coordinates.
Position Geant4StepHandler::localToGlobal(const Position& local)  const   {
  return localToGlobal(G4ThreeVector(local.X(),local.Y(),local.Z()));
}

/// Coordinate transformation to global coordinates
Position Geant4StepHandler::localToGlobal(double x, double y, double z)  const    {
  return localToGlobal(G4ThreeVector(x,y,z));
}

/// Coordinate transformation to global coordinates
Position Geant4StepHandler::localToGlobal(const G4ThreeVector& loc)  const    {
  G4TouchableHandle t = step->GetPreStepPoint()->GetTouchableHandle();
  G4ThreeVector p = t->GetHistory()->GetTopTransform().Inverse().TransformPoint(loc);
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
Position Geant4StepHandler::globalToLocal(double x, double y, double z)  const    {
  G4ThreeVector p = globalToLocalG4(G4ThreeVector(x,y,z));
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
Position Geant4StepHandler::globalToLocal(const Position& global)  const    {
  G4ThreeVector p = globalToLocalG4(G4ThreeVector(global.X(),global.Y(),global.Z()));
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
Position Geant4StepHandler::globalToLocal(const G4ThreeVector& global)  const    {
  G4ThreeVector p = globalToLocalG4(global);
  return Position(p.x(),p.y(),p.z());
}

/// Coordinate transformation to local coordinates
G4ThreeVector Geant4StepHandler::globalToLocalG4(double x, double y, double z)  const    {
  return globalToLocalG4(G4ThreeVector(x,y,z));
}

/// Coordinate transformation to local coordinates
G4ThreeVector Geant4StepHandler::globalToLocalG4(const G4ThreeVector& global)  const    {
  G4TouchableHandle t = step->GetPreStepPoint()->GetTouchableHandle();
  return t->GetHistory()->GetTopTransform().TransformPoint(global);
}
