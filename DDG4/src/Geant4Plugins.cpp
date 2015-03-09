// $Id: Readout.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Plugins.inl"
#include "DDG4/Factories.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4InputAction.h"
#include "DDG4/Geant4Converter.h"

#include "G4VSensitiveDetector.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4EquationOfMotion.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagneticField.hh"
#include "G4VProcess.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4VUserPhysicsList.hh"

DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VSensitiveDetector*, (std::string,DD4hep::Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(DD4hep::Simulation::Geant4Sensitive*, (DD4hep::Simulation::Geant4Context*,std::string,DD4hep::Geometry::DetElement*,DD4hep::Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(DD4hep::Simulation::Geant4Action*, (DD4hep::Simulation::Geant4Context*,std::string))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4MagIntegratorStepper*, (G4EquationOfMotion*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4MagIntegratorStepper*, (G4Mag_EqRhs*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4Mag_EqRhs*, (G4MagneticField*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VProcess*, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VPhysicsConstructor*, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4ParticleDefinition*, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(G4VUserPhysicsList*, (DD4hep::Simulation::Geant4PhysicsListActionSequence*,int))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (DD4hep::Geometry::LCDD*,const DD4hep::Simulation::Geant4Converter*,const std::map<std::string,std::string>*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(DD4hep::Simulation::Geant4EventReader*, (std::string))
