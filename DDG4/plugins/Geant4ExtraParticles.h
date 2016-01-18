//
// Authors: Tomohiko Tanabe <tomohiko@icepp.s.u-tokyo.ac.jp>
//          Taikan Suehara <suehara@icepp.s.u-tokyo.ac.jp>
// Proted from Mokka by A.Sailer (CERN )
//
// $Id$
// $Name: $

#ifndef ExtraParticles_hh
#define ExtraParticles_hh 1

#include "DDG4/Geant4Action.h"

// geant4
#include "G4VPhysicsConstructor.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4ParticleTable.hh"

namespace DD4hep  {

  namespace Simulation  {

    class Geant4ExtraParticles : public G4VPhysicsConstructor, 
                                 public Geant4Action
    {
    public:
      Geant4ExtraParticles(Geant4Context* ctxt, const std::string& nam);
      virtual ~Geant4ExtraParticles();
      void ConstructParticle();
      void ConstructProcess();
      //static bool FileExists();

    private:

      G4ParticleTable::G4PTblDicIterator* ParticleIterator;

      std::string m_pdgfile;

       
      G4Decay _decay; 	 
      G4hIonisation _ionise; 	 
      G4hMultipleScattering _scatter;
    };
  }}
#endif

