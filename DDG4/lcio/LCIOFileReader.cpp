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
      LCIOFileReader(const std::string& nam, int);
      /// Default destructor
      virtual ~LCIOFileReader();
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual EVENT::LCCollection *readParticles();
    };
  }
}
#endif // DD4HEP_DDG4_LCIOFILEREADER_H

#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "lcio.h"
#include "EVENT/LCIO.h"
#include "IO/LCReader.h"

using namespace EVENT;
using namespace DD4hep::Simulation;

// Factory entry
DECLARE_LCIO_EVENT_READER(LCIOFileReader)

/// Initializing constructor
LCIOFileReader::LCIOFileReader(const std::string& nam, int /* arg */) 
: LCIOEventReader(nam)
{
  m_reader = ::lcio::LCFactory::getInstance()->createLCReader();
  printout(INFO,"LCIOFileReader","Created file reader. Try to open input %s",nam.c_str());
  m_reader->open(nam);
}

/// Default destructor
LCIOFileReader::~LCIOFileReader()    {
  DD4hep::deletePtr(m_reader);
}

/// Read an event and return a LCCollectionVec of MCParticles.
EVENT::LCCollection *LCIOFileReader::readParticles()   {
  ::lcio::LCEvent* evt = m_reader->readNextEvent();
  if ( evt ) {
    return evt->getCollection(LCIO::MCPARTICLE);
  }
  return 0;
}

