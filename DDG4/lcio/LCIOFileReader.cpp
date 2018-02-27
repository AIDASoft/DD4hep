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
  \package LCIOFileReader
 * \brief Plugin to read lcio files
 *
 And here we can put a longer description, parameters, examples...
 *
@}
 */

#ifndef DD4HEP_DDG4_LCIOFILEREADER_H
#define DD4HEP_DDG4_LCIOFILEREADER_H

// Framework include files
#include "LCIOEventReader.h"
#include "LCIOEventParameters.h"
#include "lcio.h"

using namespace lcio ;

// Forward declarations
namespace IO { class LCReader; }

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Base class to read lcio event files
    /**
     *  \author  P.Kostka (main author)
     *  \author  M.Frank  (code reshuffeling into new DDG4 scheme)
     *  \author  R.Ete    (added event parameters in event context)
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class LCIOFileReader : public LCIOEventReader  {
    protected:
      /// Reference to reader object
      IO::LCReader* m_reader;
      std::string m_collectionName;
    public:
      /// Initializing constructor
      LCIOFileReader(const std::string& nam);
      /// Default destructor
      virtual ~LCIOFileReader();

      /// Read an event and fill a vector of MCParticles.
      virtual EventReaderStatus readParticleCollection(int event_number, EVENT::LCCollection** particles);
      virtual EventReaderStatus moveToEvent(int event_number);
      virtual EventReaderStatus skipEvent() { return EVENT_READER_OK; }
      virtual EventReaderStatus setParameters(std::map< std::string, std::string >& parameters); 
    };
  }
}
#endif // DD4HEP_DDG4_LCIOFILEREADER_H

#include "DD4hep/Printout.h"
#include "DDG4/Factories.h"
#include "UTIL/ILDConf.h"

using namespace dd4hep::sim;

// Factory entry
DECLARE_GEANT4_EVENT_READER_NS(dd4hep::sim,LCIOFileReader)

/// Initializing constructor
dd4hep::sim::LCIOFileReader::LCIOFileReader(const std::string& nam)
: LCIOEventReader(nam)
, m_collectionName(LCIO::MCPARTICLE)
{
  m_reader = ::lcio::LCFactory::getInstance()->createLCReader(LCReader::directAccess);
  printout(INFO,"LCIOFileReader","Created file reader. Try to open input %s",nam.c_str());
  m_reader->open(nam);
  m_directAccess = false;
}

/// Default destructor
dd4hep::sim::LCIOFileReader::~LCIOFileReader()    {
  dd4hep::detail::deletePtr(m_reader);
}


/// moveToSpecifiedEvent, a.k.a. skipNEvents
Geant4EventReader::EventReaderStatus
dd4hep::sim::LCIOFileReader::moveToEvent(int event_number) {
  // ::lcio::LCEvent* evt = m_reader->readEvent(/*runNumber*/ 0, event_number);
  // fg: direct access does not work if run number is different from 0 and/or event numbers are not stored consecutively
  if( m_currEvent == 0 && event_number != 0 ) {
    m_reader->skipNEvents( event_number ) ;
    printout(INFO,"LCIOFileReader::moveToEvent","Skipping the first %d events ", event_number );
    printout(INFO,"LCIOFileReader::moveToEvent","Event number before skipping: %d", m_currEvent );
    m_currEvent = event_number;
    printout(INFO,"LCIOFileReader::moveToEvent","Event number after skipping: %d", m_currEvent );
  }
  return EVENT_READER_OK;
}

/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
dd4hep::sim::LCIOFileReader::readParticleCollection(int /*event_number*/, EVENT::LCCollection** particles)  {

  ::lcio::LCEvent* evt = m_reader->readNextEvent(); // simply read the events sequentially 
  ++m_currEvent ;

  if ( evt ) {
    *particles = evt->getCollection(m_collectionName);
    if ( *particles ) {
      printout(INFO,"LCIOFileReader","read collection %s from event %d in run %d ", 
               m_collectionName.c_str(), evt->getEventNumber(), evt->getRunNumber());
      
      // Create input event parameters context
      LCIOEventParameters *parameters = new LCIOEventParameters();
      parameters->setParameters(evt->getRunNumber(), evt->getEventNumber(), evt->parameters());
      context()->event().addExtension<LCIOEventParameters>( parameters );
      
      return EVENT_READER_OK;
    }
  }
  return EVENT_READER_EOF;
}

/// Set the parameters for the class, in particular the name of the MCParticle
/// list
Geant4EventReader::EventReaderStatus
dd4hep::sim::LCIOFileReader::setParameters( std::map< std::string, std::string > & parameters ) {
  _getParameterValue( parameters, "MCParticleCollectionName", m_collectionName, std::string(LCIO::MCPARTICLE));
  return EVENT_READER_OK;
}
