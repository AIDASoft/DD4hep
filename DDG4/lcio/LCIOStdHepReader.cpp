// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//====================================================================
#ifndef DD4HEP_DDG4_LCIOSTDHEPREADER_H
#define DD4HEP_DDG4_LCIOSTDHEPREADER_H
// Framework include files
#include "LCIOEventReader.h"

// Forward declarations
namespace UTIL { class LCStdHepRdr; }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation  {

    /// Base class to read StdHep files with lcio
    /**
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOStdHepReader : public LCIOEventReader  {
    protected:
      /// Reference to Reader object
      UTIL::LCStdHepRdr* m_reader;
    public:
      /// Initializing constructor
      LCIOStdHepReader(const std::string& nam, int);
      /// Default destructor
      virtual ~LCIOStdHepReader();
      /// Read an event and return a LCCollectionVec of MCParticles.
      virtual EVENT::LCCollection *readParticles();
    };
  }     /* End namespace lcio   */
}       /* End namespace DD4hep */
#endif  /* DD4HEP_DDG4_LCIOSTDHEPREADER_H */

#include "lcio.h"
#include "EVENT/LCIO.h"
#include "UTIL/LCStdHepRdr.h"
#include "DD4hep/Primitives.h"

using namespace DD4hep::Simulation;

// Factory entry
DECLARE_LCIO_EVENT_READER(LCIOStdHepReader)

/// Initializing constructor
LCIOStdHepReader::LCIOStdHepReader(const std::string& nam, int) 
  : LCIOEventReader(nam)
{
  m_reader = new ::lcio::LCStdHepRdr(m_name.c_str());
}

/// Default destructor
LCIOStdHepReader::~LCIOStdHepReader()    {
  DD4hep::deletePtr(m_reader);
}

/// Read an event and return a LCCollectionVec of MCParticles.
EVENT::LCCollection *LCIOStdHepReader::readParticles()   {
  return m_reader->readEvent();
}
