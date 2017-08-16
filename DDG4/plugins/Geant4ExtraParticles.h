//
// Authors: Tomohiko Tanabe <tomohiko@icepp.s.u-tokyo.ac.jp>
//          Taikan Suehara <suehara@icepp.s.u-tokyo.ac.jp>
// Proted from Mokka by A.Sailer (CERN )
//
#ifndef ExtraParticles_hh
#define ExtraParticles_hh 1

#include "DDG4/Geant4PhysicsConstructor.h"

// geant4
#include "G4ProcessManager.hh"
#include "G4Decay.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4ParticleTable.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    class Geant4ExtraParticles : public Geant4PhysicsConstructor    {
    public:
      /// Standard constructor with initailization parameters
      Geant4ExtraParticles(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4ExtraParticles();
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcess(Constructor& ctor);
      /// Callback to construct particles
      virtual void constructParticle(Constructor& ctor);

    private:
      std::string m_pdgfile;    
      G4Decay* m_decay; 	 
      G4hIonisation* m_ionise; 	 
      G4hMultipleScattering* m_scatter;
    };
  }
}
#endif

