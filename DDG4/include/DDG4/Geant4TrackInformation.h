// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#ifndef DD4HEP_DDG4_GEANT4TRACKINFORMATION_H
#define DD4HEP_DDG4_GEANT4TRACKINFORMATION_H

// Framework include files
#include "G4VUserTrackInformation.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

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


  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINFORMATION_H
