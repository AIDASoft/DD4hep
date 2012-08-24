// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4StepHandler.h"
using namespace DD4hep::Simulation;

const char* Geant4StepHandler::stepStatus(G4StepStatus status) {
  switch(status) {
    // Step reached the world boundary
  case fWorldBoundary:          return "WorldBoundary";
    // Step defined by a geometry boundary
  case fGeomBoundary:           return "GeomBoundary";
    // Step defined by a PreStepDoItVector
  case fAtRestDoItProc:         return "AtRestDoItProc";
    // Step defined by a AlongStepDoItVector
  case fAlongStepDoItProc:      return "AlongStepDoItProc";
    // Step defined by a PostStepDoItVector
  case fPostStepDoItProc:       return "PostStepDoItProc";
    // Step defined by the user Step limit in the logical volume
  case fUserDefinedLimit:       return "UserDefinedLimit";
    // Step defined by an exclusively forced PostStepDoIt process 
  case fExclusivelyForcedProc:  return "ExclusivelyForcedProc";
    // Step not defined yet
  case fUndefined:
  default:                      return "Undefined";
  };
}

const char* Geant4StepHandler::preStepStatus() const    {
  return stepStatus(pre ? pre->GetStepStatus() : fUndefined);
}

const char* Geant4StepHandler::postStepStatus() const {
  return stepStatus(post ? post->GetStepStatus() : fUndefined);
}

