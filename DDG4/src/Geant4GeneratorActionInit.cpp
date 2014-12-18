// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4GeneratorActionInit.h"
#include "DDG4/Geant4InputHandling.h"

#include "G4Run.hh"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4GeneratorActionInit::Geant4GeneratorActionInit(Geant4Context* context, const std::string& nam)
: Geant4GeneratorAction(context,nam), m_run(0), m_evtTotal(0), m_evtRun(0)
{
  InstanceCount::increment(this);
  context->kernel().runAction().callAtEnd(this,&Geant4GeneratorActionInit::end);
  context->kernel().runAction().callAtBegin(this,&Geant4GeneratorActionInit::begin);
}

/// Default destructor
Geant4GeneratorActionInit::~Geant4GeneratorActionInit()  {
  InstanceCount::decrement(this);
}

/// Begin-run action callback
void Geant4GeneratorActionInit::begin(const G4Run* run)   {
  m_evtRun = 0;
  m_run = run->GetRunID();
}

/// End-run action callback
void Geant4GeneratorActionInit::end(const G4Run* /* run */)   {
  printP1("+++ Funished run %d after %d events (%d events in total)",m_run,m_evtRun,m_evtTotal);
  m_evtRun = 0;
  m_run = 0;
}

/// Event generation action callback
void Geant4GeneratorActionInit::operator()(G4Event* /* event */)  {
  /// Update event counters
  ++m_evtTotal;
  ++m_evtRun;
  /// + Printout
  print("+++ Initializing event %d. Within run:%d event %d.",m_evtTotal,m_run,m_evtRun);
  generationInitialization(this,context());
}
