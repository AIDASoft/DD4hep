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
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4GeneratorActionInit.h"

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
  printP1("+++ Funished run %d after %d events (% events in total)",m_run,m_evtRun,m_evtTotal);
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

  /**
   *  This action should register all event extension required for the further 
   *  processing. We want to avoid that every client has to check if a given 
   *  object is present or not and than later install the required data structures.
   */
  context()->event().addExtension(new Geant4PrimaryMap());

  // The final set of created particles in the simulation.
  context()->event().addExtension(new Geant4ParticleMap());

  //
  // The Geant4PrimaryEvent extension contains a whole set of 
  // Geant4PrimaryInteraction objects each may represent a complete
  // interaction. Particles and vertices may be unbiased.
  // This is the input to the translator forming the final 
  // Geant4PrimaryInteraction (see below) containing rebiased particle
  // and vertex maps.
  Geant4PrimaryEvent* evt = new Geant4PrimaryEvent();
  context()->event().addExtension(evt);
  //
  // The Geant4PrimaryInteraction extension contains the final
  // vertices and particles ready to be injected to Geant4.
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  inter->setNextPID(-1);
  context()->event().addExtension(inter);
}
