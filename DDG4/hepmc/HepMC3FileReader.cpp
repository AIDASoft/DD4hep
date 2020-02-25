//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//==========================================================================

/** \addtogroup Geant4EventReader
 *
 @{
  \package HepMC3FileReader
 * \brief Plugin to read HepMC3 Ascii files
 *
 And here we can put a longer description, parameters, examples...
 *
@}
 */

#include "HepMC3EventReader.h"

#include <HepMC3/ReaderFactory.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Base class to read hepmc3 event files
    /**
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class HEPMC3FileReader : public HEPMC3EventReader  {
    protected:
      /// Reference to reader object
      std::shared_ptr<HepMC3::Reader> m_reader;
    public:
      /// Initializing constructor
      HEPMC3FileReader(const std::string& nam);
      /// Default destructor
      virtual ~HEPMC3FileReader() = default;

      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readGenEvent(int event_number, HepMC3::GenEvent& genEvent);
      /// skip to event with event_number
      virtual EventReaderStatus moveToEvent(int event_number);
      //virtual EventReaderStatus skipEvent() { return EVENT_READER_OK; }
      virtual EventReaderStatus setParameters(std::map< std::string, std::string >& parameters);
    };
  }
}

#include "DD4hep/Printout.h"
#include "DDG4/Factories.h"

using dd4hep::sim::HEPMC3FileReader;
using dd4hep::sim::Geant4EventReader;

// Factory entry
DECLARE_GEANT4_EVENT_READER_NS(dd4hep::sim,HEPMC3FileReader)

/// Initializing constructor
HEPMC3FileReader::HEPMC3FileReader(const std::string& nam)
: HEPMC3EventReader(nam)
{
  m_reader = HepMC3::deduce_reader(nam);
  printout(INFO,"HEPMC3FileReader","Created file reader. Try to open input %s", nam.c_str());
  m_directAccess = false;
}

/// moveToSpecifiedEvent, a.k.a. skipNEvents
Geant4EventReader::EventReaderStatus
HEPMC3FileReader::moveToEvent(int event_number) {
  printout(INFO,"HEPMC3FileReader::moveToEvent","Skipping the first %d events ", event_number);
  while( m_currEvent != event_number) {
    printout(INFO,"HEPMC3FileReader::moveToEvent","Event number before skipping: %d", m_currEvent );
    HepMC3::GenEvent genEvent;
    m_reader->read_event(genEvent) ;
    ++m_currEvent;
    printout(INFO,"HEPMC3FileReader::moveToEvent","Event number after skipping: %d", m_currEvent );
  }
  return EVENT_READER_OK;
}

/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
HEPMC3FileReader::readGenEvent(int /*event_number*/, HepMC3::GenEvent& genEvent)  {
  m_reader->read_event(genEvent);
  ++m_currEvent;
  if (genEvent.particles().size()) {
    printout(INFO,"HEPMC3FileReader","Read event from file");
    return EVENT_READER_OK;
  }
  return EVENT_READER_EOF;
}

/// Set the parameters for the class
Geant4EventReader::EventReaderStatus
HEPMC3FileReader::setParameters( std::map< std::string, std::string > & parameters ) {
  _getParameterValue(parameters, "Flow1", m_flow1, std::string("flow1"));
  _getParameterValue(parameters, "Flow2", m_flow2, std::string("flow2"));
  return EVENT_READER_OK;
}
