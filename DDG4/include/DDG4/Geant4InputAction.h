// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4INPUTACTION_H
#define DD4HEP_DDG4_GEANT4INPUTACTION_H

// Framework include files
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4GeneratorAction.h"

// C/C++ include files
#include <vector>

// Forward declarations
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation  {

    /// Basic geant4 event reader class. This interface/base-class must be implemented by concrete readers.
    /** @class Geant4EventReader Geant4EventReader.h DDG4/Geant4EventReader.h
     * 
     * Base class to read input files containing simulation data.
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    class Geant4EventReader  {

    public:
      typedef Geant4Particle Particle;
    protected:
      /// File name
      std::string m_name;
      /// Flag if direct event access is supported
      bool m_directAccess;

    public:
      /// Initializing constructor
      Geant4EventReader(const std::string& nam);
      /// Default destructor
      virtual ~Geant4EventReader();
      /// File name
      const std::string& name()  const   {  return m_name;   }
      /// Flag if direct event access (by event sequence number) is supported (Default: false)
      bool hasDirectAccess() const  {  return m_directAccess; }
      /// Read an event and fill a vector of MCParticles.
      virtual int readParticles(int event_number, std::vector<Particle*>& particles) = 0;
    };

    /// Generic input action capable of using the Geant4EventReader class.
    /** @class Geant4InputAction Geant4InputAction.h DDG4/Geant4InputAction.h
     *
     * Concrete implementation of the Geant4 generator action base class
     * populating Geant4 primaries from Geant4 and HepStd files.
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    class Geant4InputAction : public Geant4GeneratorAction {

    public:
      typedef Geant4Particle Particle;

    protected:
      /// Property: input file
      std::string         m_input;
      /// Property: SYNCEVT
      int                 m_firstEvent;
      /// Property; interaction mask
      int                 m_mask;
      /// Property: Momentum downscaler for debugging
      double              m_momScale;
      /// Event reader object
      Geant4EventReader*  m_reader;

      /// Read an event and return a LCCollectionVec of MCParticles.
      int readParticles(int event_number, std::vector<Particle*>& particles);
      /// helper to report Geant4 exceptions
      std::string issue(int i) const;

    public:
      /// Standard constructor
      Geant4InputAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4InputAction();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }     /* End namespace Simulation   */
}       /* End namespace DD4hep */

#include "DD4hep/Plugins.h"
namespace {
  /// Factory template to create Geant4 event reader objects
  template <typename P> class Factory<P, DD4hep::Simulation::Geant4EventReader*(std::string)> {  public:
    static void Func(void *ret, void*, const std::vector<void*>& a, void*) 
    { *(DD4hep::Simulation::Geant4EventReader**)ret = (DD4hep::Simulation::Geant4EventReader*)new P(*(std::string*)a[0]);}
  };
}
/// Plugin defintion to create event reader objects
#define DECLARE_GEANT4_EVENT_READER(name)					\
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::Geant4EventReader*(std::string))

/// Plugin defintion to create event reader objects
#define DECLARE_GEANT4_EVENT_READER_NS(ns,name) typedef ns::name __##name##__; \
  PLUGINSVC_FACTORY_WITH_ID(__##name##__,std::string(#name),DD4hep::Simulation::Geant4EventReader*(std::string))

#endif  /* DD4HEP_DDG4_GEANT4INPUTACTION_H  */
