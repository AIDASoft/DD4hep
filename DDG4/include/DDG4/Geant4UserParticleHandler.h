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
#ifndef DD4HEP_DDG4_GEANT4USERPARTICLEHANDLER_H
#define DD4HEP_DDG4_GEANT4USERPARTICLEHANDLER_H

// Framework include files
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4Step;
class G4Track;
class G4Event;
class G4SteppingManager;


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Particle;
    class Geant4ParticleHandler;

    /// Geant4ParticleHandler user extension action called by the particle handler.
    /**
     *  Collect optional MC particle information and attacjh it to the particle object
     *  Clients may inherit from this class and override the approriate methods
     *  to add additional information in form of a DataExtension object to the Particle.
     *
     *  The default implementation is always empty!
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserParticleHandler : public Geant4Action  {
    public:
      typedef Geant4Particle Particle;

    protected:
      /// Property: Energy cut below which particles are not collected, but assigned to the parent
      double m_kinEnergyCut;

    public:
      /// Standard constructor
      Geant4UserParticleHandler(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4UserParticleHandler();
      /// Event generation action callback
      virtual void generate(G4Event* event, Geant4ParticleHandler* handler);
      /// Pre-event action callback
      virtual void begin(const G4Event* event);
      /// Post-event action callback
      virtual void end(const G4Event* event);

      /// User stepping callback
      /** Allow the user to intercept particle handling in the pre track action.
       *  The default implementation is empty.
       *
       *  Note: The particle passed is a temporary and will be copied if kept.
       */
      virtual void step(const G4Step* step, G4SteppingManager* mgr, Particle& particle);

      /// Pre-track action callback.
      /** Allow the user to intercept particle handling in the pre track action.
       *  e.g. attach relevant user information.
       *  The default implementation is empty.
       *
       *  Note: The particle passed is a temporary and will be copied if kept.
       */
      virtual void begin(const G4Track* track, Particle& particle);

      /// Post-track action callback
      /** Allow the user to force the particle handling in the post track action
       *  set the reason mask to NULL in order to drop the particle.
       *  The parent's reasoning mask will be or'ed with the particle's mask
       *  to preserve the MC truth for the hit creation.
       *  The default implementation is empty.
       *
       *  Note: The particle passed is a temporary and will be copied if kept.
       */
      virtual void end(const G4Track* track, Particle& particle);

      /// Callback to be answered if the particle MUST be kept during recombination step
      /** Allow the user to force the particle handling either by
       *  or the reason mask with G4PARTICLE_KEEP_USER or
       *  to set the reason mask to NULL in order to drop it.
       *  The default implementation is empty.
       *
       *  If the reason mask entry is set to G4PARTICLE_FORCE_KILL
       *  or is set to NULL, the particle is ALWAYS removed
       *
       *  The default implementation calls
       *  Geant4ParticleHandler::defaultKeepParticle(particle)
       *  Please have a look therein if it suffices your needs!
       *
       *  Note: This may override all other decisions!
       *        Default implementation is empty.
       *
       */
      virtual bool keepParticle(Particle& particle);

      /// Callback when parent should be combined
      /** Called before a particle is removed from the final record.
       *  Relevant particle properties of the parent may be updated.
       *  The default implementation is empty.
       */
      virtual void combine(Particle& to_be_deleted, Particle& remaining_parent);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4USERPARTICLEHANDLER_H
