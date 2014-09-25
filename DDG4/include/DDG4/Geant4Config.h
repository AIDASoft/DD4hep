// $Id: Geant4config.h 615 2013-06-18 11:13:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Small helper file with abbreviations to write configurations
//  for CINT; also used when writing the XML configuration.
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DDG4_GEANT4CONFIG_H
#define DDG4_GEANT4CONFIG_H

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    template <typename TYPE> class Geant4Handle;
    class Geant4Kernel;
    class Geant4Action;
    class Geant4Filter;
    class Geant4RunAction;
    class Geant4EventAction;
    class Geant4TrackingAction;
    class Geant4StackingAction;
    class Geant4SteppingAction;
    class Geant4Sensitive;
    class Geant4ActionPhase;
    class Geant4GeneratorAction;
    class Geant4PhysicsList;
    class Geant4UserParticleHandler;
    class Geant4GeneratorActionSequence;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4PhysicsListActionSequence;
    class Geant4SensDetActionSequence;

    /*
     *   Simulation setup namespace declaration
     */
    namespace Setup {
      // Basics
      typedef Geant4Kernel Kernel;
      //typedef Geant4Handle<Geant4Kernel> KernelH;
      // Actions
      typedef Geant4Handle<Geant4Action> Action;
      typedef Geant4Handle<Geant4Filter> Filter;
      typedef Geant4Handle<Geant4GeneratorAction> GenAction;
      typedef Geant4Handle<Geant4RunAction> RunAction;
      typedef Geant4Handle<Geant4EventAction> EventAction;
      typedef Geant4Handle<Geant4TrackingAction> TrackAction;
      typedef Geant4Handle<Geant4StackingAction> StackAction;
      typedef Geant4Handle<Geant4SteppingAction> StepAction;
      typedef Geant4Handle<Geant4PhysicsList> PhysicsList;
      typedef Geant4Handle<Geant4ActionPhase> Phase;
      typedef Geant4Handle<Geant4Sensitive> Sensitive;

      // Sequences
      typedef Geant4Handle<Geant4SensDetActionSequence>     SensitiveSeq;
      typedef Geant4Handle<Geant4GeneratorActionSequence>   GeneratorSeq;
      typedef Geant4Handle<Geant4RunActionSequence>         RunActionSeq;
      typedef Geant4Handle<Geant4EventActionSequence>       EventActionSeq;
      typedef Geant4Handle<Geant4TrackingActionSequence>    TrackActionSeq;
      typedef Geant4Handle<Geant4SteppingActionSequence>    StepActionSeq;
      typedef Geant4Handle<Geant4StackingActionSequence>    StackActionSeq;
      typedef Geant4Handle<Geant4PhysicsListActionSequence> PhysicsActionSeq;
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
#include "DDG4/Geant4ActionPhase.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4UserParticleHandler.h"
#include "DDG4/ComponentUtils.h"
#include "DD4hep/LCDD.h"

#endif // DDG4_GEANT4CONFIG_H
