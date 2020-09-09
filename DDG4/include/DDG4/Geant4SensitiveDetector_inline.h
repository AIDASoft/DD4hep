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

#ifndef DDG4_GEANT4SENSITIVEDETECTOR_INLINE_H
#define DDG4_GEANT4SENSITIVEDETECTOR_INLINE_H

// Framework include files
#include "DDG4/Geant4SensitiveDetector.h"

/// Constructor. The sensitive detector element is identified by the detector name
template <class SD> dd4hep::sim::Geant4GenericSD<SD>::Geant4GenericSD(const std::string& nam, Detector& description_ref)
  : Geant4SensitiveDetector(nam, description_ref) {
  defineCollection(m_sensitive.hitsCollection());
}

/// Initialize the sensitive detector for the usage of a single hit collection
template <class SD> bool dd4hep::sim::Geant4GenericSD<SD>::defineCollection(const std::string& coll_name) {
  return Geant4SensitiveDetector::defineCollection(coll_name);
}

/** Method invoked at the begining of each event.
 *  The hits collection(s) created by this sensitive detector must
 *  be set to the G4HCofThisEvent object at one of these two methods.
 */
template <class SD> void dd4hep::sim::Geant4GenericSD<SD>::Initialize(G4HCofThisEvent* HCE) {
  this->Geant4SensitiveDetector::Initialize(HCE);
}

/// Method invoked at the end of each event.
template <class SD> void dd4hep::sim::Geant4GenericSD<SD>::EndOfEvent(G4HCofThisEvent* HCE) {
  this->Geant4SensitiveDetector::EndOfEvent(HCE);
}

/// Method invoked if the event was aborted.
template <class SD> void dd4hep::sim::Geant4GenericSD<SD>::clear() {
  this->Geant4SensitiveDetector::clear();
}

/// Method for generating hit(s) using the information of G4Step object.
template <class SD> G4bool dd4hep::sim::Geant4GenericSD<SD>::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  return this->Geant4SensitiveDetector::ProcessHits(step, history);
}

/// Method for generating hit(s) using the information of G4Step object.
template <class SD> bool dd4hep::sim::Geant4GenericSD<SD>::buildHits(G4Step*, G4TouchableHistory*) {
  return true;
}

#endif // DDG4_GEANT4SENSITIVEDETECTOR_INLINE_H
