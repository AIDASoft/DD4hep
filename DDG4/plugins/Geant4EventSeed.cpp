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
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"

#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4Random.h"
#include "DDG4/Factories.h"

#include "CLHEP/Random/EngineFactory.h"

//Geant includes
#include <G4Run.hh>
#include <G4Event.hh>

using namespace dd4hep::sim;

/// Standard constructor
Geant4EventSeed::Geant4EventSeed(Geant4Context* c, const std::string& typ) : Geant4RunAction(c, typ),
									     m_initialSeed(0),
									     m_runID(0),
									     m_type(typ),
									     m_initialised(false)
{
  Geant4Action::runAction().callAtBegin(this,&Geant4EventSeed::begin);
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

  Geant4Random *rndm = Geant4Random::instance();

  unsigned int eventID = evt->GetEventID();
  unsigned int newSeed = hash( m_initialSeed, eventID, m_runID );

  dd4hep::printout( dd4hep::INFO, m_type,
		    "At beginEvent: eventID=%u, runID=%u initialSeed=%u, newSeed=%u" ,
		    evt->GetEventID(),  m_runID, m_initialSeed, newSeed );

  rndm->setSeed( newSeed );

  if ( dd4hep::printLevel() <= dd4hep::DEBUG ) {
    rndm->showStatus();
  }

}

DECLARE_GEANT4ACTION(Geant4EventSeed)
