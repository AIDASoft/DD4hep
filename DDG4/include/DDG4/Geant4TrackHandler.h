// $Id: Geant4TrackHandler.h 683 2013-08-08 16:18:20Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4TRACKHANDLER_H
#define DD4HEP_GEANT4TRACKHANDLER_H

// Framework include files
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4Track.hh"
#include "G4TrajectoryPoint.hh"
#include "G4VTouchable.hh"
#include "G4VSensitiveDetector.hh"

// Forward declarations
class G4VTouchableHandle;
class G4VUserTrackInformation;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations;
    class Geant4TrackHandler;

    /** @class Geant4TrackHandler Geant4SensitiveDetector.h DDG4/Geant4SensitiveDetector.h
     *
     * Tiny helper/utility class to easily access Geant4 track information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackHandler {
    public:
      typedef G4VUserTrackInformation Info;
      typedef G4ReferenceCountedHandle<G4VTouchable> Touchable;
      /// Reference to the track object
      const G4Track* track;
      /// Initializing constructor
      Geant4TrackHandler(const G4Track* t)
          : track(t) {
      }
      /// Conversion to G4Track
      operator const G4Track*() const {
        return track;
      }
      /// Track's particle definition
      G4ParticleDefinition* trackDef() const {
        return track->GetDefinition();
      }
      /// Track position
      const G4ThreeVector& position() const {
        return track->GetPosition();
      }
      /// Track energy
      double energy() const {
        return track->GetTotalEnergy();
      }
      /// Track velocity
      double velocity() const {
        return track->GetVelocity();
      }
      /// Track length
      double length() const {
        return track->GetTrackLength();
      }
      /// Track time
      double time() const {
        return track->GetGlobalTime();
      }
      /// Physical (original) volume of the track
      G4VPhysicalVolume* vol() const {
        return track->GetVolume();
      }
      /// Next physical volume of the track
      G4VPhysicalVolume* nextVol() const {
        return track->GetNextVolume();
      }
      /// Logical volume of the origine vertex
      const G4LogicalVolume* vertexVol() const {
        return track->GetLogicalVolumeAtVertex();
      }

      /// Touchable of the track
      const Touchable& touchable() const {
        return track->GetTouchableHandle();
      }
      /// Next touchable of the track
      const Touchable& nextTouchable() const {
        return track->GetNextTouchableHandle();
      }

      /// Physical process of the track generation
      const G4VProcess* creatorProcess() const {
        return track->GetCreatorProcess();
      }
      /// User information block
      Info* userInfo() const {
        return track->GetUserInformation();
      }
      /// Set user information block (const mis-match)
      //void setUserInfo(Info* info)              { track->SetUserInformation(info);           }
      /// Specific user information block
      template <typename T> T* info() const {
        return (T*) userInfo();
      }
      /// Step information
      const G4Step* step() const {
        return track->GetStep();
      }
      /// Step number
      G4int stepNumber() const {
        return track->GetCurrentStepNumber();
      }

      int pdgID() const {
        G4ParticleDefinition* def = trackDef();
        return def ? def->GetPDGEncoding() : 0;
      }
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4HITS_H
