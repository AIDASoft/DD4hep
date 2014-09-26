// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_LCIOEVENTREADER_H
#define DD4HEP_DDG4_LCIOEVENTREADER_H

// C/C++ include files
#include <string>

// Forward declarations
namespace EVENT { class LCCollection;    }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation  {

    // Forward declarations
    class Geant4Particle;

    /// Base class to read lcio files.
    /** 
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOEventReader  {

    public:
      typedef Geant4Particle Particle;
    protected:
      /// File name
      std::string m_name;

    public:
      /// Initializing constructor
      LCIOEventReader(const std::string& nam);
      /// Default destructor
      virtual ~LCIOEventReader();
      /// File name
      const std::string& name()  const   {  return m_name;   }
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual EVENT::LCCollection *readParticles() = 0;
    };

  }     /* End namespace Simulation   */
}       /* End namespace DD4hep */


#include "DD4hep/Plugins.h"
namespace {
  /// Factory to create Geant4 physics constructions
  template <typename P> class Factory<P, DD4hep::Simulation::LCIOEventReader*(std::string, int)> {  public:
    static void Func(void *ret, void*, const std::vector<void*>& a, void*) 
    { *(DD4hep::Simulation::LCIOEventReader**)ret = (DD4hep::Simulation::LCIOEventReader*)new P(*(std::string*)a[0],*(int*)a[1]);}
  };
}
/// Plugin defintion to create event reader objects
#define DECLARE_LCIO_EVENT_READER(name)					\
  PLUGINSVC_FACTORY_WITH_ID(name,std::string(#name),DD4hep::Simulation::LCIOEventReader*(std::string,int))

/// Plugin defintion to create event reader objects
#define DECLARE_LCIO_EVENT_READER_NS(ns,name) typedef ns::name __##name##__; \
  PLUGINSVC_FACTORY_WITH_ID(__##name##__,std::string(#name),DD4hep::Simulation::LCIOEventReader*(std::string,int))

#endif  /* DD4HEP_DDG4_LCIOEVENTREADER_H  */
