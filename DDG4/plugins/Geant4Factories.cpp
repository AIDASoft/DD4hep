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

// Framework include files
#include "DDG4/Factories.h"
using namespace DD4hep::Simulation;

#include "DDG4/Geant4Random.h"
DECLARE_GEANT4ACTION(Geant4Random)

#include "DDG4/Geant4ActionPhase.h"
DECLARE_GEANT4ACTION(Geant4PhaseAction)

#include "DDG4/Geant4RunAction.h"
DECLARE_GEANT4ACTION(Geant4RunActionSequence)

#include "DDG4/Geant4EventAction.h"
DECLARE_GEANT4ACTION(Geant4EventActionSequence)

#include "DDG4/Geant4SteppingAction.h"
DECLARE_GEANT4ACTION(Geant4SteppingActionSequence)

#include "DDG4/Geant4TrackingAction.h"
DECLARE_GEANT4ACTION(Geant4TrackingActionSequence)

#include "DDG4/Geant4StackingAction.h"
DECLARE_GEANT4ACTION(Geant4StackingActionSequence)

#include "DDG4/Geant4GeneratorAction.h"
DECLARE_GEANT4ACTION(Geant4GeneratorActionSequence)

#include "DDG4/Geant4PhysicsList.h"
DECLARE_GEANT4ACTION(Geant4PhysicsList)
DECLARE_GEANT4ACTION(Geant4PhysicsListActionSequence)

#include "DDG4/Geant4SensDetAction.h"
DECLARE_GEANT4ACTION(Geant4SensDetActionSequence)

#include "DDG4/Geant4UIManager.h"
DECLARE_GEANT4ACTION(Geant4UIManager)

#include "DDG4/Geant4MonteCarloTruth.h"
DECLARE_GEANT4ACTION(Geant4DummyTruthHandler)

#include "DDG4/Geant4ParticleHandler.h"
DECLARE_GEANT4ACTION(Geant4ParticleHandler)

#include "DDG4/Geant4UserParticleHandler.h"
DECLARE_GEANT4ACTION(Geant4UserParticleHandler)

#include "DDG4/Geant4ParticlePrint.h"
DECLARE_GEANT4ACTION(Geant4ParticlePrint)

//=============================
#include "DDG4/Geant4TrackingPreAction.h"
DECLARE_GEANT4ACTION(Geant4TrackingPreAction)

#include "DDG4/Geant4TrackingPostAction.h"
DECLARE_GEANT4ACTION(Geant4TrackingPostAction)

//=============================
#include "DDG4/Geant4OutputAction.h"
DECLARE_GEANT4ACTION(Geant4OutputAction)

#include "DDG4/Geant4Output2ROOT.h"
DECLARE_GEANT4ACTION(Geant4Output2ROOT)

//=============================
#include "DDG4/Geant4ParticleGun.h"
DECLARE_GEANT4ACTION(Geant4ParticleGun)

//=============================
#include "DDG4/Geant4GeneratorActionInit.h"
DECLARE_GEANT4ACTION(Geant4GeneratorActionInit)

//=============================
#include "DDG4/Geant4IsotropeGenerator.h"
DECLARE_GEANT4ACTION(Geant4ParticleGenerator)
DECLARE_GEANT4ACTION(Geant4IsotropeGenerator)

//=============================
#include "DDG4/Geant4InteractionVertexSmear.h"
DECLARE_GEANT4ACTION(Geant4InteractionVertexSmear)

//=============================
#include "DDG4/Geant4InteractionVertexBoost.h"
DECLARE_GEANT4ACTION(Geant4InteractionVertexBoost)

//=============================
#include "DDG4/Geant4InteractionMerger.h"
DECLARE_GEANT4ACTION(Geant4InteractionMerger)

//=============================
#include "DDG4/Geant4PrimaryHandler.h"
DECLARE_GEANT4ACTION(Geant4PrimaryHandler)

//=============================
#include "DDG4/Geant4InputAction.h"
DECLARE_GEANT4ACTION(Geant4InputAction)

//=============================
#include "DDG4/Geant4GeneratorWrapper.h"
DECLARE_GEANT4ACTION(Geant4GeneratorWrapper)

//=============================
#include "DDG4/Geant4TestActions.h"
namespace DD4hep {  namespace Simulation   {
    namespace Test {}
    //using namespace Test;
    using namespace DD4hep::Simulation::Test;
    typedef Geant4TestSensitive Geant4TestSensitiveTracker;
    typedef Geant4TestSensitive Geant4TestSensitiveCalorimeter;
  }}

DECLARE_GEANT4ACTION(Geant4TestRunAction)
DECLARE_GEANT4ACTION(Geant4TestEventAction)
DECLARE_GEANT4ACTION(Geant4TestStepAction)
DECLARE_GEANT4ACTION(Geant4TestTrackAction)
//DECLARE_GEANT4ACTION(Geant4TestStackingAction)
DECLARE_GEANT4ACTION(Geant4TestGeneratorAction)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitive)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitiveTracker)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitiveCalorimeter)
