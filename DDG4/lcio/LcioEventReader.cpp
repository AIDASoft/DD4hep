// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================

// Framework include files
#include "EventReader.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep   {
  /*
   *   lcio namespace declaration
   */
  namespace lcio {

    /** @class LcioEventReader LcioEventReader.h DDG4/LcioEventReader.h
     * 
     * Base class to read lcio event files
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    struct LcioEventReader : public EventReader  {
    protected:
      /// Reference to reader object
      IO::LCReader* m_reader;
    public:
      /// Initializing constructor
      LcioEventReader(const std::string& nam, int);
      /// Default destructor
      virtual ~LcioEventReader();
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual Particles *readEvent();
    };
  }
}

#include "DD4hep/Printout.h"
#include "lcio.h"
#include "EVENT/LCIO.h"
#include "IO/LCReader.h"

using namespace EVENT;

// Factory entry
typedef DD4hep::lcio::LcioEventReader LcioEventReader;
DECLARE_LCIO_EVENT_READER(LcioEventReader)

/// Initializing constructor
LcioEventReader::LcioEventReader(const std::string& nam, int /* arg */) 
: EventReader(nam)
{
  m_reader = ::lcio::LCFactory::getInstance()->createLCReader();
  printout(INFO,"LcioEventReader","Created file reader. Try to open input %s",nam.c_str());
  m_reader->open(nam);
}

/// Default destructor
LcioEventReader::~LcioEventReader()    {
  deletePtr(m_reader);
}

/// Read an event and return a LCCollectionVec of MCParticles.
DD4hep::lcio::EventReader::Particles *LcioEventReader::readEvent()   {
  ::lcio::LCEvent* evt = m_reader->readNextEvent();
  if ( evt ) {
    Particles* mcVec = (Particles*)evt->getCollection(LCIO::MCPARTICLE);
    return mcVec;
  }
  return 0;
}

