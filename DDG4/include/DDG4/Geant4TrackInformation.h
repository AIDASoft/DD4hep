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

#ifndef DDG4_GEANT4TRACKINFORMATION_H
#define DDG4_GEANT4TRACKINFORMATION_H

// Framework include files
#include "G4VUserTrackInformation.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Unused.
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackInformation : public G4VUserTrackInformation {
    public:
      enum {
        STORE = 1 << 0, LAST = 1 << 31
      };
    protected:
      int m_flags;

    public:
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


  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4TRACKINFORMATION_H
