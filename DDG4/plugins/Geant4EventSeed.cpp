//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : A.Sailer
//
//=========================================================================

// Class include file
#include "Geant4EventSeed.h"

// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DD4hep/Printout.h>

#include <DDG4/EventParameters.h>
#include <DDG4/Geant4EventAction.h>
#include <DDG4/Geant4StackingAction.h>
#include <DDG4/Geant4Random.h>
#include <DDG4/Factories.h>

#include <CLHEP/Random/EngineFactory.h>

//Geant includes
#include <G4Run.hh>
#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4StackManager.hh>

using namespace dd4hep::sim;

/// Standard constructor
Geant4EventSeed::Geant4EventSeed(Geant4Context* c, const std::string& typ) : Geant4RunAction(c, typ),
									     m_initialSeed(0),
									     m_runID(0),
									     m_type(typ),
									     m_initialised(false)
{
  Geant4Action::runAction().callAtBegin(this,&Geant4EventSeed::begin);
  Geant4Action::generatorAction().callAtBegin(this,&Geant4EventSeed::setSeedForPrimaries);
  // Re-seed after GeneratePrimaries so file-based generators' SetEventID is visible.
  Geant4Action::eventAction().callAtBegin(this,&Geant4EventSeed::beginEvent);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4EventSeed::~Geant4EventSeed() {
  InstanceCount::decrement(this);
}

/// begin-of-run callback
void Geant4EventSeed::begin(const G4Run* run) {

  if(not m_initialised){
    m_initialised = true;
    m_initialSeed = Geant4Random::instance()->engine()->getSeed();
  }

  m_runID = run->GetRunID();

  dd4hep::printout( dd4hep::INFO, m_type, "Get RunID: runID=%u", m_runID );

}

/// begin-of-event callback
void Geant4EventSeed::beginEvent(const G4Event* evt) {
  dd4hep::printout(dd4hep::DEBUG, m_type, "EventSeed:: At beginEvent");
  setSeed(evt, true);
}

/// begin-of-event callback
void Geant4EventSeed::setSeedForPrimaries(const G4Event* evt) {
  dd4hep::printout(dd4hep::DEBUG, m_type, "EventSeed:: At generatePrimaries");
  // since this is before we read input files we cannot expect parameters to be present
  setSeed(evt, false);
}

/// general function for setting the seed, called by other callbacks
void Geant4EventSeed::setSeed(const G4Event* evt, bool checkForEventParameters) {

  Geant4Random *rndm = Geant4Random::instance();

  //Trying to use event and run id from the Geant4 event, unless we have them also in the EventParameters
  //Both are kept local: m_runID is owned by the begin-of-run callback and must not be overwritten here,
  //otherwise the value would leak into the calls that asked not to look at the EventParameters
  unsigned int eventID = evt->GetEventID();
  unsigned int runID   = m_runID;

  if(checkForEventParameters) {
    //Get EventParameters from the context
    EventParameters* parameters = context()->event().extension<EventParameters>(false);
    if(parameters) {
      //Unset numbers are negative in the EventParameters, keep the Geant4 values in that case
      if(parameters->eventNumber() < 0) {
        dd4hep::printout(dd4hep::DEBUG, m_type,
                         "EventSeed::setSeed: eventParameters have unset eventNumber=%d, keeping eventID=%u",
                         parameters->eventNumber(), eventID);
      } else {
        eventID = parameters->eventNumber();
      }
      if(parameters->runNumber() < 0) {
        dd4hep::printout(dd4hep::DEBUG, m_type,
                         "EventSeed::setSeed: eventParameters have unset runNumber=%d, keeping runID=%u",
                         parameters->runNumber(), runID);
      } else {
        runID = parameters->runNumber();
      }
      dd4hep::printout(dd4hep::DEBUG, m_type,
                       "EventSeed::setSeed: Found eventParameters: eventID=%u, runID=%u",
                       eventID, runID);
    } else {
      dd4hep::printout(dd4hep::DEBUG, m_type,
                       "EventSeed::setSeed: Did not find eventParameters");
    }
  }

  unsigned int newSeed = hash( m_initialSeed, eventID, runID );
  dd4hep::printout(dd4hep::INFO, m_type,
                   "EventSeed::setSeed: eventID=%u, runID=%u initialSeed=%u, newSeed=%u",
                   eventID, runID, m_initialSeed, newSeed );

  rndm->setSeed(newSeed);

  if (dd4hep::printLevel() <= dd4hep::DEBUG) {
    rndm->showStatus();
  }

}

DECLARE_GEANT4ACTION(Geant4EventSeed)
