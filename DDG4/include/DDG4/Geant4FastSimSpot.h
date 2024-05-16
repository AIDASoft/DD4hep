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
#ifndef DDG4_GEANT4FASTSIMSPOT_H
#define DDG4_GEANT4FASTSIMSPOT_H

// Framework include files
#include <DDG4/Defs.h>

// Geant4 include files
#include <G4Version.hh>

#if G4VERSION_NUMBER < 1070

#include <G4ThreeVector.hh>

class G4FastHit
{
 public:
  G4FastHit() = default;
  G4FastHit(const G4ThreeVector& aPosition, G4double aEnergy)
    : fEnergy(aEnergy), fPosition(aPosition) {}
  G4FastHit(const G4ThreeVector& aPosition, G4double aEnergy, G4bool /* aDebug */)
    : fEnergy(aEnergy), fPosition(aPosition) {}
  virtual ~G4FastHit() = default;

  /// Set energy
  inline void SetEnergy(const G4double& aEnergy) { fEnergy = aEnergy; }
  /// Get energy
  inline G4double GetEnergy() const { return fEnergy; }
  /// Set position
  inline void SetPosition(const G4ThreeVector& aPosition) { fPosition = aPosition; }
  /// Get position
  inline G4ThreeVector GetPosition() const { return fPosition; }
 private:
  /// energy
  G4double fEnergy  {0e0};
  /// position
  G4ThreeVector fPosition  { };
};
#else
#include <G4FastHit.hh>
#endif

#include <G4Track.hh>
#include <G4FastTrack.hh>
#include <G4ThreeVector.hh>
#include <G4TouchableHandle.hh>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Spot definition for fast simulation and GFlash
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4FastSimSpot   {
    public:
      /// Default constructor disabled
      Geant4FastSimSpot() = delete;
      /// Initializing constructor
      Geant4FastSimSpot(const G4FastHit* h, const G4FastTrack* trk);
      /// Initializing constructor
      Geant4FastSimSpot(const G4FastHit* h, const G4FastTrack* trk, G4VTouchable* t);
      /// Default destructor
      ~Geant4FastSimSpot() = default;

      /// Spot's hit position
      G4ThreeVector hitPosition() const    { return hit->GetPosition();          }
      /// Energy deposit of the spot
      double        energy() const         { return hit->GetEnergy();            }
      /// Access the physical volume of the hit
      G4VPhysicalVolume* volume() const    { return touchable->GetVolume();      }

      /// Primary track position
      G4ThreeVector trackPosition() const  { return primary->GetPosition();      }
      /// Primary track momentum
      G4ThreeVector trackMomentum() const  { return primary->GetMomentum();      }
      /// Primary track kinetic energy
      double kineticEnergy() const         { return primary->GetKineticEnergy(); }
      /// Primary track particle definition
      const G4ParticleDefinition* trackDefinition() const 
      { return primary->GetParticleDefinition();                                 }
      /// Particle's local direction in the region's envelope solid
      G4ThreeVector particleLocalDirection()  const
      { return track->GetPrimaryTrackLocalDirection();                           }
      /// Particle's local momentum in the region's envelope solid
      G4ThreeVector particleLocalMomentum()  const
      { return track->GetPrimaryTrackLocalMomentum();                            }
      /// Particle's local position in the region's envelope solid
      G4ThreeVector particleLocalPosition()  const
      { return track->GetPrimaryTrackLocalPosition();                            }

      /// Distance to exit point from the region's envelope solid
      double distanceToOut()   const;
      /// Particle's global direction in the region's envelope solid
      G4ThreeVector particleDirection()  const;
      /// Particle's global momentum in the region's envelope solid
      G4ThreeVector particleMomentum()  const;
      /// Particle's global position in the region's envelope solid
      G4ThreeVector particlePosition()  const;
      
  public:
      const G4FastHit*   hit        { nullptr };
      const G4FastTrack* track      { nullptr };
      const G4Track*     primary    { nullptr };
      G4VTouchable*      touchable  { nullptr };
    };

    /// Initializing constructor
    inline Geant4FastSimSpot::Geant4FastSimSpot(const G4FastHit* h,
						const G4FastTrack* trk,
						G4VTouchable* t)
      : hit(h), track(trk), primary(trk->GetPrimaryTrack()), touchable(t)
    {
    }

    /// Initializing constructor
    inline Geant4FastSimSpot::Geant4FastSimSpot(const G4FastHit* h,
						const G4FastTrack* trk)
      : hit(h), track(trk), primary(trk->GetPrimaryTrack())
    {
    }

    /// Particle's global momentum in the region's envelope solid
    inline G4ThreeVector Geant4FastSimSpot::particleMomentum()  const    {
      auto mom = track->GetPrimaryTrackLocalMomentum();
      return track->GetInverseAffineTransformation()->TransformPoint(mom);
    }

    /// Particle's global direction in the region's envelope solid
    inline G4ThreeVector Geant4FastSimSpot::particleDirection()  const    {
      auto dir = track->GetPrimaryTrackLocalDirection();
      return track->GetInverseAffineTransformation()->TransformPoint(dir);
    }

    /// Particle's global position in the region's envelope solid
    inline G4ThreeVector Geant4FastSimSpot::particlePosition()  const    {
      auto pos = track->GetPrimaryTrackLocalPosition();
      return track->GetInverseAffineTransformation()->TransformPoint(pos);
    }

    /// Distance to exit point from the region's envelope solid
    inline double Geant4FastSimSpot::distanceToOut()   const     {
      auto pos = track->GetPrimaryTrackLocalPosition();
      auto dir = track->GetPrimaryTrackLocalDirection();
      return track->GetEnvelopeSolid()->DistanceToOut(pos, dir);
    }


  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4FASTSIMSPOT_H
