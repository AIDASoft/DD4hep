// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DDG4/Factories.h"
using namespace DD4hep::Simulation;

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
//DECLARE_GEANT4ACTION(Geant4TestGeneratorAction)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitive)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitiveTracker)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitiveCalorimeter)
