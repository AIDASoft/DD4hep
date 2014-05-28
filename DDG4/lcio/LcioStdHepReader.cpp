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

    /** @class StdHepReader StdHepReader.h DDG4/StdHepReader.h
     * 
     * Base class to read StdHep files with lcio
     *
     * @author  P.Kostka (main author)
     * @author  M.Frank  (code reshuffeling into new DDG4 scheme)
     * @version 1.0
     */
    struct StdHepReader : public EventReader  {
    protected:
      /// Reference to reader object
      UTIL::LCStdHepRdr* m_reader;
    public:
      /// Initializing constructor
      StdHepReader(const std::string& nam, int);
      /// Default destructor
      virtual ~StdHepReader();
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual Particles *readEvent();
    };
  }     /* End namespace lcio   */
}       /* End namespace DD4hep */


#include "lcio.h"
#include "EVENT/LCIO.h"
#include "UTIL/LCStdHepRdr.h"

// Factory entry
typedef DD4hep::lcio::StdHepReader LcioStdHepReader;
DECLARE_LCIO_EVENT_READER(LcioStdHepReader)

/// Initializing constructor
DD4hep::lcio::StdHepReader::StdHepReader(const std::string& nam, int) 
  : EventReader(nam)
{
  m_reader = new ::lcio::LCStdHepRdr(m_name.c_str());
}

/// Default destructor
DD4hep::lcio::StdHepReader::~StdHepReader()    {
  deletePtr(m_reader);
}

/// Read an event and return a LCCollectionVec of MCParticles.
DD4hep::lcio::EventReader::Particles *DD4hep::lcio::StdHepReader::readEvent()   {
  return m_reader->readEvent();
}

