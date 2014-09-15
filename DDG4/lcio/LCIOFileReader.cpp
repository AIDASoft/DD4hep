// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================

// Framework include files
#include "LCIOEventReader.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep   {
  /*
   *   lcio namespace declaration
   */
  namespace Simulation {

    /** @class LcioEventReader LcioEventReader.h DDG4/LcioEventReader.h
     * 
     * Base class to read lcio event files
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    struct LCIOFileReader : public LCIOEventReader  {
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

