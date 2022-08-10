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
#ifndef DDG4_GEANT4FASTSIMHANDLER_H
#define DDG4_GEANT4FASTSIMHANDLER_H

// Framework include files
#include <DDG4/Geant4HitHandler.h>
#include <DDG4/Geant4FastSimSpot.h>

// Geant4 include files
#include <G4VTouchable.hh>
#include <G4VSensitiveDetector.hh>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations;
    class Geant4FastSimHandler;

    /// Helper class to ease the extraction of information from a G4FastSimSpot object.
    /**
     *
     * Helper/utility class to easily access Geant4 gflashspot/fast simulation information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4FastSimHandler  : public Geant4HitHandler  {
    public:
      const Geant4FastSimSpot* spot;

      /// Inhibit default constructor
      Geant4FastSimHandler() = delete;
      /// Initializing constructor
      Geant4FastSimHandler(const Geant4FastSimSpot* sp)
	: Geant4HitHandler(sp->primary, sp->touchable), spot(sp)
      {
      }
      /// No copy constructor
      Geant4FastSimHandler(const Geant4FastSimHandler& copy) = delete;
      /// No move constructor
      Geant4FastSimHandler(Geant4FastSimHandler&& copy) = delete;
      /// Assignment operator inhibited. Should not be copied
      Geant4FastSimHandler& operator=(const Geant4FastSimHandler& copy) = delete;
      /// Move operator inhibited. Should not be copied
      Geant4FastSimHandler& operator=(Geant4FastSimHandler&& copy) = delete;
      /// Returns total energy deposit
      double energy() const  {
	return spot->hit->GetEnergy();
      }
      /// Return track momentum in DD4hep notation
      Momentum momentum() const {
        const G4ThreeVector& p = track->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      /// Returns the pre-gflashspot position
      Position avgPosition() const {
        const G4ThreeVector p = spot->hit->GetPosition();
        return Position(p.x(), p.y(), p.z());
      }
      /// Returns the post-step position as a G4ThreeVector
      G4ThreeVector avgPositionG4() const {
        return spot->hit->GetPosition();
      }
      /// Direction calculated from the post- and pre-position ofthe gflashspot
      Position direction()  const  {
        const G4ThreeVector p = track->GetMomentumDirection();
        return Position(p.x(), p.y(), p.z());
      }
      /// Returns the track momentum as a G4ThreeVector
      G4ThreeVector momentumG4() const {
	return track->GetMomentum();
      }
      /// Access the enery deposit of the energy spot
      double deposit() const  {
        return spot->hit->GetEnergy();
      }
      G4VPhysicalVolume* volume() const {
	return touchable()->GetVolume();
      }
      G4LogicalVolume* logvol() const {
        return volume()->GetLogicalVolume();
      }
      G4VSolid* solid() const {
        return touchable()->GetSolid();
      }
      G4VSensitiveDetector* sd()  const {
        G4LogicalVolume* lv = logvol();
        return lv ? lv->GetSensitiveDetector() : 0;
      }
      const char* sdName(const char* undefined = "") const {
        G4VSensitiveDetector* s = sd();
        return s ? s->GetName().c_str() : undefined;
      }
      bool isSensitive() const {
	G4LogicalVolume* lv = logvol();
        return lv ? (0 != lv->GetSensitiveDetector()) : false;
      }
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4FASTSIMSPOTHANDLER_H
