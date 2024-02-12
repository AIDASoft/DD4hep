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
#include <DDG4/Geant4StepHandler.h>
#include <DDSegmentation/Segmentation.h>
#include <DD4hep/DD4hepUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>

// Geant4 include files
#include <G4Version.hh>

namespace units = dd4hep;
using namespace dd4hep::sim;

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

/// Apply BirksLaw
double Geant4StepHandler::birkAttenuation() const    {
#if G4VERSION_NUMBER >= 1001
  static G4EmSaturation s_emSaturation(1);
#else
  static G4EmSaturation s_emSaturation();
  s_emSaturation.SetVerbose(1);
#endif

#if G4VERSION_NUMBER >= 1030
  static bool s_initialised = false;
  if(not s_initialised) {
      s_emSaturation.InitialiseG4Saturation();
      s_initialised = true;
  }
#endif

  double energyDeposition = step->GetTotalEnergyDeposit();
  double length = step->GetStepLength();
  double niel   = step->GetNonIonizingEnergyDeposit();
  const G4Track* trk = step->GetTrack();
  const G4ParticleDefinition* particle = trk->GetDefinition();
  const G4MaterialCutsCouple* couple = trk->GetMaterialCutsCouple();
  double engyVis = s_emSaturation.VisibleEnergyDeposition(particle,
                                                          couple,
                                                          length,
                                                          energyDeposition,
                                                          niel);
  return engyVis;
}
