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
 * \brief Plugin to read HepMC3 files
 *
 * The HepMC3 File reader uses the HepMC3 Reader Factory to open any HepMC3 compatible file.
 *
 * However, it was only tested on HepMC3 ascii files produced by Whizard 2.8.2, so please use it carefully, and validate
 * its conversion carefully before extensive usage.
 *
@}
 */

#include "HepMC3EventReader.h"

#include "DDG4/EventParameters.h"

#include <HepMC3/ReaderFactory.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    template <class T=HepMC3::GenEvent> void EventParameters::ingestParameters(T const& genEvent) {
      for(auto const& attr: genEvent.attributes()){
        for(auto const& inAttr: attr.second){
          std::stringstream strstr;
          strstr << attr.first;
          // if more than one entry for given key, or if counter not 0 append "_counterValue"
          if(attr.second.size() > 1 or inAttr.first != 0){
            strstr << "_" << inAttr.first;
          }
          auto attr_as_int = std::dynamic_pointer_cast<HepMC3::IntAttribute>(inAttr.second);
          auto attr_as_flt = std::dynamic_pointer_cast<HepMC3::FloatAttribute>(inAttr.second);
          if(attr_as_int) {
            m_intValues[strstr.str()] = {attr_as_int->value()};
          } else if(attr_as_flt) {
            m_fltValues[strstr.str()] = {attr_as_flt->value()};
          } else { // anything else
            m_strValues[strstr.str()] = {inAttr.second->unparsed_string()};
          }
        }
      }
    }


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
    m_reader->skip(event_number);
    m_currEvent = event_number;
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
    // Create input event parameters context
    try {
      Geant4Context* ctx = context();
      EventParameters *parameters = new EventParameters();
      parameters->setEventNumber(genEvent.event_number());
      parameters->ingestParameters(genEvent);
      ctx->event().addExtension<EventParameters>(parameters);
    }
    catch(std::exception &)
    {
    }
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
