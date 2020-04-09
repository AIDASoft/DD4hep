//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

/** \addtogroup Geant4EventReader
 *
 @{
  \package LCIOStdHepReader
 * \brief Base class to read StdHep files
 *
 *
@}
 */

#ifndef DD4HEP_DDG4_LCIOSTDHEPREADER_H
#define DD4HEP_DDG4_LCIOSTDHEPREADER_H

// LCIO include files
#include "UTIL/LCStdHepRdr.h"

// Framework include files
#include "LCIOEventReader.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

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
      LCIOStdHepReader(const std::string& nam);
      /// Default destructor
      virtual ~LCIOStdHepReader();
      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticleCollection(int event_number, EVENT::LCCollection** particles);
      virtual EventReaderStatus moveToEvent(int event_number);
      virtual EventReaderStatus skipEvent() { return EVENT_READER_OK; }

    };
  }     /* End namespace lcio   */
}       /* End namespace dd4hep */
#endif  /* DD4HEP_DDG4_LCIOSTDHEPREADER_H */

//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDG4/Factories.h"

// Factory entry
DECLARE_GEANT4_EVENT_READER_NS(dd4hep::sim,LCIOStdHepReader)

using namespace dd4hep::sim;

/// Initializing constructor
LCIOStdHepReader::LCIOStdHepReader(const std::string& nam)
  : LCIOEventReader(nam), m_reader(new UTIL::LCStdHepRdr(m_name.c_str()))
{
}

/// Default destructor
LCIOStdHepReader::~LCIOStdHepReader()    {
  dd4hep::detail::deletePtr(m_reader);
}


/// skipEvents if required
Geant4EventReader::EventReaderStatus
LCIOStdHepReader::moveToEvent(int event_number) {
  if( m_currEvent == 0 && event_number != 0 ) {
    printout(INFO,"LCIOStdHepReader::moveToEvent","Skipping the first %d events ", event_number );
    printout(INFO,"LCIOStdHepReader::moveToEvent","Event number before skipping: %d", m_currEvent );
    while ( m_currEvent < event_number ) {
      EVENT::LCCollection* particles = m_reader->readEvent();
      if ( 0 == particles ) return EVENT_READER_EOF;
      delete particles;
      ++m_currEvent;
    }
  }
  printout(INFO,"LCIOStdHepReader::moveToEvent","Event number after skipping: %d", m_currEvent );
  return EVENT_READER_OK;
}

/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
LCIOStdHepReader::readParticleCollection(int /*event_number*/, EVENT::LCCollection** particles)  {

  *particles = m_reader->readEvent();
  ++m_currEvent;

  if ( 0 == *particles ) return EVENT_READER_EOF;
  return EVENT_READER_OK;
}
