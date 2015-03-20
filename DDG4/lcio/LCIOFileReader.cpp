// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_LCIOFILEREADER_H
#define DD4HEP_DDG4_LCIOFILEREADER_H

// Framework include files
#include "LCIOEventReader.h"

// Forward declarations
namespace IO { class LCReader; }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation  {

    /// Base class to read lcio event files
    /**
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOFileReader : public LCIOEventReader  {
    protected:
      /// Reference to reader object
      IO::LCReader* m_reader;
    public:
      /// Initializing constructor
      LCIOFileReader(const std::string& nam);
      /// Default destructor
      virtual ~LCIOFileReader();
      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticleCollection(int event_number, EVENT::LCCollection** particles);
    };
  }
}
#endif // DD4HEP_DDG4_LCIOFILEREADER_H

#include "DD4hep/Printout.h"
#include "DDG4/Factories.h"
#include "UTIL/ILDConf.h"

using namespace DD4hep::Simulation;

// Factory entry
DECLARE_GEANT4_EVENT_READER_NS(DD4hep::Simulation,LCIOFileReader)

/// Initializing constructor
DD4hep::Simulation::LCIOFileReader::LCIOFileReader(const std::string& nam)
: LCIOEventReader(nam)
{
  m_reader = ::lcio::LCFactory::getInstance()->createLCReader(LCReader::directAccess);
  printout(INFO,"LCIOFileReader","Created file reader. Try to open input %s",nam.c_str());
  m_reader->open(nam);
  m_directAccess = true;
}

/// Default destructor
DD4hep::Simulation::LCIOFileReader::~LCIOFileReader()    {
  DD4hep::deletePtr(m_reader);
}

/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
DD4hep::Simulation::LCIOFileReader::readParticleCollection(int event_number, EVENT::LCCollection** particles)  {
  ::lcio::LCEvent* evt = m_reader->readEvent(/*runNumber*/ 0, event_number);
  if ( evt ) {
    *particles = evt->getCollection(LCIO::MCPARTICLE);
    if ( *particles ) return EVENT_READER_OK;
  }
  return EVENT_READER_ERROR;
}

