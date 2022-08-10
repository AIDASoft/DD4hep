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
#ifndef DDG4_GEANT4HITHANDLER_H
#define DDG4_GEANT4HITHANDLER_H

// Framework include files
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4EmSaturation.hh"
#include "G4Version.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations;
    class Geant4HitHandler;

    /// Helper class to ease the extraction of information from a G4Hit object.
    /**
     *
     * Helper/utility class to easily access Geant4 hit information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HitHandler {
    protected:
    public:
      const G4Track* track;
      const G4VTouchable* touchable_ptr;
      /// Inhibit default constructor
      Geant4HitHandler() = delete;
      /// Initializing constructor
      Geant4HitHandler(const G4Track* t, const G4VTouchable* h)
	: track(t), touchable_ptr(h)
      {
      }
      /// No copy constructor
      Geant4HitHandler(const Geant4HitHandler& copy) = delete;
      /// No move constructor
      Geant4HitHandler(Geant4HitHandler&& copy) = delete;
      /// Assignment operator inhibited. Should not be copied
      Geant4HitHandler& operator=(const Geant4HitHandler& copy) = delete;
      /// Move operator inhibited. Should not be copied
      Geant4HitHandler& operator=(Geant4HitHandler&& copy) = delete;
      
      const G4VTouchable* touchable() const {
        return touchable_ptr;
      }
      G4ParticleDefinition* trackDef() const {
        return track->GetDefinition();
      }
      int trkPdgID() const {
        return track->GetDefinition()->GetPDGEncoding();
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
      Momentum trkMom() const {
        const G4ThreeVector& p = track->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      bool isSensitive(const G4LogicalVolume* lv) const {
        return lv ? (0 != lv->GetSensitiveDetector()) : false;
      }
      bool isSensitive(const G4VPhysicalVolume* pv) const {
        return pv ? isSensitive(pv->GetLogicalVolume()) : false;
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
