// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4Particle;
    class Geant4ParticleHandler;

    /** Geant4ParticleHandler user extension action called by the particle handler.
     *
     *  Collect optional MC particle information and attacjh it to the particle object
     *  Clients may inherit from this class and override the approriate methods
     *  to add additional information in form of a DataExtension object to the Particle.
     *
     *  The dfault implementation is always empty!
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4UserParticleHandler : public Geant4Action  {
    public:
      typedef Geant4Particle Particle;
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
      virtual void step(const G4Step* step, G4SteppingManager* mgr, Particle& particle);
      /// Pre-track action callback
      virtual void begin(const G4Track* track, Particle& particle);
      /// Post-track action callback
      virtual void end(const G4Track* track, Particle& particle);
      /// Callback when parent should be combined
      virtual void combine(Particle& to_be_deleted, Particle& remaining_parent);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4USERPARTICLEHANDLER_H
