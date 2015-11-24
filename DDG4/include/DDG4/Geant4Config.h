// $Id: $
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
//
//  Small helper file with abbreviations to write configurations
//  for CINT; also used when writing the XML configuration.
//
//====================================================================
#ifndef DDG4_GEANT4CONFIG_H
#define DDG4_GEANT4CONFIG_H

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    template <typename TYPE> class Geant4Handle;
    class Geant4Kernel;
    class Geant4Action;
    class Geant4Filter;
    class Geant4PhaseAction;
    class Geant4RunAction;
    class Geant4EventAction;
    class Geant4TrackingAction;
    class Geant4StackingAction;
    class Geant4SteppingAction;
    class Geant4Sensitive;
    class Geant4ActionPhase;
    class Geant4GeneratorAction;
    class Geant4PhysicsList;
    class Geant4UserInitialization;
    class Geant4UserParticleHandler;
    class Geant4GeneratorActionSequence;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4PhysicsListActionSequence;
    class Geant4SensDetActionSequence;
    class Geant4UserInitializationSequence;
    class Geant4DetectorConstruction;
    class Geant4DetectorConstructionSequence;

    /// Convenience namespace to ease the setupup of DDG4 applications
    namespace Setup {
      // Basics
      typedef Geant4Kernel Kernel;
      //typedef Geant4Handle<Geant4Kernel> KernelH;
      // Actions
      typedef Geant4Handle<Geant4Action>                       Action;
      typedef Geant4Handle<Geant4Filter>                       Filter;
      typedef Geant4Handle<Geant4PhaseAction>                  PhaseAction;
      typedef Geant4Handle<Geant4GeneratorAction>              GenAction;
      typedef Geant4Handle<Geant4RunAction>                    RunAction;
      typedef Geant4Handle<Geant4EventAction>                  EventAction;
      typedef Geant4Handle<Geant4TrackingAction>               TrackAction;
      typedef Geant4Handle<Geant4StackingAction>               StackAction;
      typedef Geant4Handle<Geant4SteppingAction>               StepAction;
      typedef Geant4Handle<Geant4PhysicsList>                  PhysicsList;
      typedef Geant4Handle<Geant4ActionPhase>                  Phase;
      typedef Geant4Handle<Geant4Sensitive>                    Sensitive;
      typedef Geant4Handle<Geant4UserInitialization>           Initialization;
      typedef Geant4Handle<Geant4DetectorConstruction>         DetectorConstruction;

      // Sequences
      typedef Geant4Handle<Geant4SensDetActionSequence>        SensitiveSeq;
      typedef Geant4Handle<Geant4GeneratorActionSequence>      GeneratorSeq;
      typedef Geant4Handle<Geant4RunActionSequence>            RunActionSeq;
      typedef Geant4Handle<Geant4EventActionSequence>          EventActionSeq;
      typedef Geant4Handle<Geant4TrackingActionSequence>       TrackActionSeq;
      typedef Geant4Handle<Geant4SteppingActionSequence>       StepActionSeq;
      typedef Geant4Handle<Geant4StackingActionSequence>       StackActionSeq;
      typedef Geant4Handle<Geant4PhysicsListActionSequence>    PhysicsActionSeq;
      typedef Geant4Handle<Geant4UserInitializationSequence>   InitializationSeq;
      typedef Geant4Handle<Geant4DetectorConstructionSequence> DetectorConstructionSeq;
    }

  }    // End namespace Simulation
}      // End namespace DD4hep

#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4DetectorConstruction.h"
#include "DDG4/Geant4ActionPhase.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Geant4UserParticleHandler.h"
#include "DDG4/ComponentUtils.h"
#include "DD4hep/LCDD.h"

#endif // DDG4_GEANT4CONFIG_H
