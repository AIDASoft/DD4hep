// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4OutputAction.h"

// Geant 4 includes
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

using namespace DD4hep::Simulation;
using namespace DD4hep;
using namespace std;

/// Standard constructor
Geant4OutputAction::Geant4OutputAction(Geant4Context* ctxt, const string& nam)
  : Geant4EventAction(ctxt, nam), m_truth(0)
{
  InstanceCount::increment(this);
  declareProperty("Output", m_output);
  declareProperty("HandleErrorsAsFatal", m_errorFatal=true);
  context()->runAction().callAtBegin(this, &Geant4OutputAction::beginRun);
  context()->runAction().callAtEnd(this, &Geant4OutputAction::endRun);
}

/// Default destructor
Geant4OutputAction::~Geant4OutputAction() {
  InstanceCount::decrement(this);
}

/// begin-of-event callback
void Geant4OutputAction::begin(const G4Event* /* event */) {
}

/// End-of-event callback
void Geant4OutputAction::end(const G4Event* evt) {
  OutputContext < G4Event > ctxt(evt);
  G4HCofThisEvent* hce = evt->GetHCofThisEvent();
  if ( hce )  {
    int nCol = hce->GetNumberOfCollections();
    try  {
      m_truth = context()->event().extension<Geant4ParticleMap>(false);
      if ( m_truth && !m_truth->isValid() )  {
        m_truth = 0;
        printout(WARNING,name(),"+++ [Event:%d] No valid MC truth info present. "
                 "Is a Particle handler installed ?",evt->GetEventID());
      }
      try  {
        saveEvent(ctxt);
        for (int i = 0; i < nCol; ++i) {
          G4VHitsCollection* hc = hce->GetHC(i);
          saveCollection(ctxt, hc);
        }
      }
      catch(const exception& e)   {
        printout(ERROR,name(),"+++ [Event:%d] Exception while saving event:%s",
                 evt->GetEventID(),e.what());
        if ( m_errorFatal ) throw;
      }
      catch(...)   {
        printout(ERROR,name(),"+++ [Event:%d] UNKNWON Exception while saving event",
                 evt->GetEventID());
        if ( m_errorFatal ) throw;
      }
      commit(ctxt);
    }
    catch(const exception& e)   {
      printout(ERROR,name(),"+++ [Event:%d] Exception while saving event:%s",
               evt->GetEventID(),e.what());
      if ( m_errorFatal ) throw;
    }
    catch(...)   {
      printout(ERROR,name(),"+++ [Event:%d] UNKNWON Exception while saving event",
               evt->GetEventID());
      if ( m_errorFatal ) throw;
    }
    m_truth = 0;
    return;
  }
  printout(WARNING,"Geant4OutputAction",
           "+++ The value of G4HCofThisEvent is NULL. No collections saved!");
}

/// Commit data at end of filling procedure
void Geant4OutputAction::commit(OutputContext<G4Event>& /* ctxt */) {
}

/// Callback to initialize the storing of the Geant4 information
void Geant4OutputAction::beginRun(const G4Run* /* run */) {
}

/// Callback to store the Geant4 run information
void Geant4OutputAction::endRun(const G4Run* /* run */) {
}

/// Callback to store the Geant4 run information
void Geant4OutputAction::saveRun(const G4Run* /* run */) {
}

/// Callback to store the Geant4 event
void Geant4OutputAction::saveEvent(OutputContext<G4Event>& /* ctxt */) {
}

/// Callback to store each Geant4 hit collection
void Geant4OutputAction::saveCollection(OutputContext<G4Event>& /* ctxt */, G4VHitsCollection* /* collection */) {
}

