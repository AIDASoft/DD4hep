// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DDG4/Geant4SensitiveDetector.h"

using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Constructor
Geant4SensitiveDetector::Geant4SensitiveDetector(const std::string& name, DetElement detector, LCDD& lcdd)
  : G4VSensitiveDetector(name), m_detector(detector), m_lcdd(lcdd)
{
  m_readout = detector.readout();
  collectionName.insert("trackerCollection");
}

/// Standard destructor
Geant4SensitiveDetector::~Geant4SensitiveDetector() {
}

/// Method invoked at the begining of each event. 
void Geant4SensitiveDetector::Initialize(G4HCofThisEvent* HCE) {
}

/// Method invoked at the end of each event. 
void Geant4SensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE) {
}

/// Method for generating hit(s) using the information of G4Step object.
G4bool Geant4SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist) {
}

/// Method is invoked if the event abortion is occured.
void Geant4SensitiveDetector::clear() {
}

//DECLARE_TRANSLATION(LCDD2Geant4,create_g4);
