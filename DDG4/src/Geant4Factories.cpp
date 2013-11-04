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
