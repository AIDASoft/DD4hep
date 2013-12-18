// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_LCIO_EVENTREADER_H
#define DD4HEP_DDG4_LCIO_EVENTREADER_H

// Framework include files
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4GeneratorAction.h"

// C/C++ include files
#include <set>
#include <map>

// Forward declarations
class G4Event;
class G4PrimaryParticle;
namespace IO    { class LCReader;        }
namespace UTIL  { class LCStdHepRdr;     }
namespace EVENT { class MCParticle;      }
namespace IMPL  { class MCParticleImpl;  }
namespace IMPL  { class LCCollectionVec; }


/*
 *   DD4hep namespace declaration
 */
namespace DD4hep   {
  /*
   *   lcio namespace declaration
   */
  namespace lcio {

    /** @class EventReader EventReader.h DDG4/EventReader.h
     * 
     * Base class to read lcio files
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    class EventReader  {
    public:
      typedef IMPL::LCCollectionVec Particles;
    protected:
      /// File name
      std::string m_name;
    public:
      /// Initializing constructor
      EventReader(const std::string& nam);
      /// Default destructor
      virtual ~EventReader();
      /// File name
      const std::string& name()  const   {  return m_name;   }
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual Particles *readEvent() = 0;
    };

    /** @class LcioGeneratorAction Geant4GeneratorAction.h DDG4/Geant4GeneratorAction.h
     *
     * Concrete implementation of the Geant4 generator action base class
     * populating Geant4 primaries from LCIO and HepStd files.
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    class LcioGeneratorAction : public Simulation::Geant4GeneratorAction {
    public:
      typedef EVENT::MCParticle     MCParticle;
      typedef IMPL::MCParticleImpl  Particle;
      typedef IMPL::LCCollectionVec Particles;
      typedef std::set<MCParticle*>                    LCPrimarySet;
      typedef std::set<G4PrimaryParticle*>             G4PrimarySet;
      typedef std::map<MCParticle*,G4PrimaryParticle*> G4ParticleMap;
    protected:
      /// Property: input file
      std::string   m_input;
      /// Property: spread of primary vertex in Z
      double        m_primaryVertexSpreadZ;
      /// Property: lorentz transformation angle
      double        m_lorentzTransformationAngle;
      /// Property: SYNCEVT
      int           m_SYNCEVT;
      /// map of partciles already import to Geant4 event
      G4ParticleMap m_g4ParticleMap;
      /// Set of LCIO particles already converted
      LCPrimarySet  m_convertedParticles;
      /// Event reader object
      EventReader*  m_reader;

      /// Read an event and return a LCCollectionVec of MCParticles.
      Particles*    readEvent(int which);
    public:
      /// Standard constructor
      LcioGeneratorAction(Simulation::Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~LcioGeneratorAction();
      /// Callback to generate primary particles
      virtual void operator()(G4Event*);
      G4PrimarySet getRelevantParticles(MCParticle* p);
    };
  }     /* End namespace lcio   */
}       /* End namespace DD4hep */

#include "DD4hep/Plugins.h"
namespace {
  /// Factory to create Geant4 physics constructions
  template <typename P> class Factory<P, DD4hep::lcio::EventReader*(std::string, int)> {  public:
    static void Func(void *ret, void*, const std::vector<void*>& a, void*) 
    { *(DD4hep::lcio::EventReader**)ret = (DD4hep::lcio::EventReader*)new P(*(std::string*)a[0],*(int*)a[1]);}
  };
}
/// Plugin defintion to create event reader objects
#define DECLARE_LCIO_EVENT_READER(name)					\
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::lcio::EventReader*(std::string,int))

/// Plugin defintion to create event reader objects
#define DECLARE_LCIO_EVENT_READER_NS(ns,name) typedef ns::name __##name##__; \
  PLUGINSVC_FACTORY_WITH_ID(__##name##__,std::string(#name),DD4hep::lcio::EventReader*(std::string,int))

#endif  /* DD4HEP_DDG4_LCIO_EVENTREADER_H  */
