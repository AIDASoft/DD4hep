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
#ifndef DDG4_GEANT4GFLASHSPOTHANDLER_H
#define DDG4_GEANT4GFLASHSPOTHANDLER_H

// Framework include files
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4GFlashSpot.hh"
#include "G4VTouchable.hh"
#include "G4VSensitiveDetector.hh"
#include "G4EmSaturation.hh"
#include "G4Version.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations;
    class Geant4GFlashSpotHandler;

    /// Helper class to ease the extraction of information from a G4GFlashSpot object.
    /**
     *
     * Helper/utility class to easily access Geant4 gflashspot information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GFlashSpotHandler {
    public:
      const G4GFlashSpot* gflashspot;
      const G4FastTrack*  fast_track;
      const G4Track*      track;
      /// Inhibit default constructor
      Geant4GFlashSpotHandler() = delete;
      /// Initializing constructor
      Geant4GFlashSpotHandler(const G4GFlashSpot* s) : gflashspot(s) {
        fast_track = gflashspot->GetOriginatorTrack();
	track      = fast_track->GetPrimaryTrack();
      }
      /// No copy constructor
      Geant4GFlashSpotHandler(const Geant4GFlashSpotHandler& copy) = delete;
      /// No move constructor
      Geant4GFlashSpotHandler(Geant4GFlashSpotHandler&& copy) = delete;
      /// Assignment operator inhibited. Should not be copied
      Geant4GFlashSpotHandler& operator=(const Geant4GFlashSpotHandler& copy) = delete;
      /// Move operator inhibited. Should not be copied
      Geant4GFlashSpotHandler& operator=(Geant4GFlashSpotHandler&& copy) = delete;
      
      G4ParticleDefinition* trackDef() const {
        return track->GetDefinition();
      }
      int trkPdgID() const {
        return track->GetDefinition()->GetPDGEncoding();
      }
      /// Returns total energy deposit
      double energy() const  {
	return gflashspot->GetEnergySpot()->GetEnergy();
      }
      /// Returns the pre-gflashspot position
      Position position() const {
        const G4ThreeVector p = gflashspot->GetEnergySpot()->GetPosition();
        return Position(p.x(), p.y(), p.z());
      }
      /// Direction calculated from the post- and pre-position ofthe gflashspot
      Position direction()  const  {
        const G4ThreeVector p = track->GetMomentumDirection();
        return Position(p.x(), p.y(), p.z());
      }
      /// Return track momentum in DD4hep notation
      Momentum momentum() const {
        const G4ThreeVector& p = track->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      /// Returns the post-step position as a G4ThreeVector
      G4ThreeVector positionG4() const {
        return gflashspot->GetEnergySpot()->GetPosition();
      }
      /// Returns the track momentum as a G4ThreeVector
      G4ThreeVector momentumG4() const {
        return track->GetMomentum();
      }
      /// Access the enery deposit of the energy spot
      double deposit() const  {
        return gflashspot->GetEnergySpot()->GetEnergy();
      }
      /// Access the G4 track ID
      int trkID() const  {
        return track->GetTrackID();
      }
      /// Access the G4 track ID of the parent track
      int parentID() const  {
        return track->GetParentID();
      }
      double trkTime() const  {
        return track->GetGlobalTime();
      }
      double trkEnergy() const  {
        return track->GetTotalEnergy();
      }
      double trkKineEnergy() const  {
        return track->GetKineticEnergy();
      }
      int trkStatus()  const  {
        return track->GetTrackStatus();
      }
      bool trkAlive() const  {
        return track->GetTrackStatus() == fAlive;
      }
      const G4VProcess* trkProcess()  const  {
        return track->GetCreatorProcess();
      }
      const G4VTouchable* touchable() const {
        return gflashspot->GetTouchableHandle().operator->();
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
      /// Coordinate transformation to global coordinates.
      /** Note: Positions are in units of MM! */
      Position localToGlobal(const Position& local)  const;
      /// Coordinate transformation to global coordinates.
      /** Note: DDSegmentation points are units in CM! Conversion done inside! */
      Position localToGlobal(const DDSegmentation::Vector3D& local)  const;
      /// Coordinate transformation to global coordinates in MM
      Position localToGlobal(const G4ThreeVector& local)  const;
      /// Coordinate transformation to global coordinates in MM
      Position localToGlobal(double x, double y, double z)  const;

      /// Coordinate transformation to local coordinates
      Position globalToLocal(double x, double y, double z)  const;
      /// Coordinate transformation to local coordinates
      Position globalToLocal(const Position& global)  const;
      /// Coordinate transformation to local coordinates
      Position globalToLocal(const G4ThreeVector& global)  const;
      /// Coordinate transformation to local coordinates
      G4ThreeVector globalToLocalG4(double x, double y, double z)  const;
      /// Coordinate transformation to local coordinates with G4 objects
      G4ThreeVector globalToLocalG4(const G4ThreeVector& loc)  const;
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4GFLASHSPOTHANDLER_H
