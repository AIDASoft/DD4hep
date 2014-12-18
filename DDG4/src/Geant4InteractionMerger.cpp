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
#include "DDG4/Geant4InputHandling.h"
#include "DDG4/Geant4InteractionMerger.h"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4InteractionMerger::Geant4InteractionMerger(Geant4Context* context, const std::string& nam)
: Geant4GeneratorAction(context,nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4InteractionMerger::~Geant4InteractionMerger()  {
  InstanceCount::decrement(this);
}

/// Event generation action callback
void Geant4InteractionMerger::operator()(G4Event* /* event */)  {
  Geant4PrimaryEvent* evt = context()->event().extension<Geant4PrimaryEvent>();
  const std::vector<Geant4PrimaryEvent::Interaction*>& inter  = evt->interactions();
  debug("+++ Merging MC input record from %d interactions:",(int)inter.size());
  mergeInteractions(this,context());
}
