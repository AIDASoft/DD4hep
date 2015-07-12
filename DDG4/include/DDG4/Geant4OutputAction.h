// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDG4_GEANT4OUTPUTACTION_H
#define DD4HEP_DDG4_GEANT4OUTPUTACTION_H

// Framework include files
#include "DDG4/Geant4EventAction.h"

// Forward declarations
class G4Run;
class G4Event;
class G4VHitsCollection;


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4ParticleMap;

    /// Base class to output Geant4 event data to persistent media
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4OutputAction: public Geant4EventAction {
    protected:
      /// Helper class for thread savety
      template <typename T> class OutputContext {
      public:
        const T* context;
        void* userData;
        OutputContext(const T* c)
          : context(c), userData(0) {
        }
        template <typename U> U* data() const {
          return (U*) userData;
        }
      };

      /// Property: "Output" output destination
      std::string m_output;
      /// Property: "HandleErrorsAsFatal" Handle errors as fatal and rethrow eventual exceptions
      bool        m_errorFatal;
      /// Reference to MC truth object
      Geant4ParticleMap* m_truth;
    public:
      /// Standard constructor
      Geant4OutputAction(Geant4Context* c, const std::string& nam);
      /// Default destructor
      virtual ~Geant4OutputAction();

      /// begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);
      /// Callback to initialize storing the Geant4 information
      virtual void beginRun(const G4Run* run);
      /// Callback to store the Geant4 run information
      virtual void endRun(const G4Run* run);

      /// Callback to store the Geant4 event
      virtual void saveRun(const G4Run* run);
      /// Callback to store the Geant4 event
      virtual void saveEvent(OutputContext<G4Event>& ctxt);
      /// Callback to store each Geant4 hit collection
      virtual void saveCollection(OutputContext<G4Event>& ctxt, G4VHitsCollection* collection);
      /// Commit data at end of filling procedure
      virtual void commit(OutputContext<G4Event>& ctxt);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4OUTPUTACTION_H
