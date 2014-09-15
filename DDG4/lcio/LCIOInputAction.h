// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_LCIOINPUTACTION_H
#define DD4HEP_DDG4_LCIOINPUTACTION_H

// Framework include files
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4GeneratorAction.h"

// C/C++ include files
#include <set>
#include <map>
#include <vector>

// Forward declarations
class G4Event;
namespace IO    { class LCReader;        }
namespace UTIL  { class LCStdHepRdr;     }
namespace EVENT { class MCParticle;      }
namespace EVENT { class LCCollection;    }
namespace IMPL  { class MCParticleImpl;  }
namespace IMPL  { class LCCollectionVec; }

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep  {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation  {

    // Forward declarations
    class Geant4Particle;
    class LCIOEventReader;

    /** @class InputAction Geant4GeneratorAction.h DDG4/Geant4GeneratorAction.h
     *
     * Concrete implementation of the Geant4 generator action base class
     * populating Geant4 primaries from LCIO and HepStd files.
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    class LCIOInputAction : public Simulation::Geant4GeneratorAction {

    public:
      typedef Geant4Particle Particle;

    protected:
      /// Property: input file
      std::string       m_input;
      /// Property: SYNCEVT
      int               m_SYNCEVT;
      /// Property; interaction mask
      int               m_mask;
      /// Property: Momentum downscaler for debugging
      double            m_momScale;
      /// Event reader object
      LCIOEventReader*  m_reader;

      /// Read an event and return a LCCollectionVec of MCParticles.
      EVENT::LCCollection* readParticles(int which);
      /// helper to report Geant4 exceptions
      std::string issue(int i) const;

    public:
      /// Standard constructor
      LCIOInputAction(Simulation::Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~LCIOInputAction();
      /// Callback to generate primary particles
      virtual void operator()(G4Event*);

      std::set<int> relevantParticles(const std::vector<Particle*>& particles, Geant4ParticleHandle p) const;
    };
  }     /* End namespace lcio   */
}       /* End namespace DD4hep */

#endif  /* DD4HEP_DDG4_LCIOINPUTACTION_H  */
