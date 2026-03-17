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
#ifndef DDG4_LCIO_LCIOEVENTREADER_H
#define DDG4_LCIO_LCIOEVENTREADER_H

// Framework include files
#include <DDG4/Geant4InputAction.h>
#include <memory>

// Forward declarations
namespace EVENT { class LCCollection;    }

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Base class to read lcio files.
    /**
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOEventReader : public Geant4EventReader  {
    public:
      /// unique_ptr type for a particle collection. The deleter is either a no-op
      /// (collection owned by the reader) or calls delete (caller-owned collection)
      using CollectionOwner = std::unique_ptr<EVENT::LCCollection, void(*)(EVENT::LCCollection*)>;

      /// Initializing constructor
      LCIOEventReader(const std::string& nam);
      /// Default destructor
      virtual ~LCIOEventReader();

      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticles(int event_number,
                                              Vertices& vertices,
                                              std::vector<Particle*>& particles);
      /// Read an event and return a LCCollectionVec of MCParticles.
      /// Implementations set particles to a CollectionOwner whose deleter reflects
      /// ownership: a no-op if the collection is owned by the reader, or delete if
      /// it is caller-owned.
      virtual EventReaderStatus readParticleCollection(int event_number,
                                                       CollectionOwner& particles) = 0;
    };

  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_LCIO_LCIOEVENTREADER_H
