// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKINFORMATION_H
#define DD4HEP_DDG4_GEANT4TRACKINFORMATION_H

// Framework include files
#include "G4VUserTrackInformation.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4TrackInformation Geant4Action.h DDG4/Geant4Action.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4TrackInformation : public G4VUserTrackInformation {
      enum {
        STORE = 1 << 0, LAST = 1 << 31
      };
      int m_flags;
      /// Default constructor
      Geant4TrackInformation();
      /// Standard destructor
      virtual ~Geant4TrackInformation();
      /// Access flag if track should be stored
      bool storeTrack() const {
        return (m_flags & STORE) != 0;
      }
      /// Access flag if track should be stored
      Geant4TrackInformation& storeTrack(bool value);
    };


  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINFORMATION_H
