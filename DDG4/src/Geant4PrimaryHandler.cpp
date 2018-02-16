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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4InputHandling.h"
#include "DDG4/Geant4PrimaryHandler.h"

using namespace dd4hep::sim;

/// Standard constructor
Geant4PrimaryHandler::Geant4PrimaryHandler(Geant4Context* ctxt, const std::string& nam)
  : Geant4GeneratorAction(ctxt,nam)
{
  InstanceCount::increment(this);
  declareProperty("RejectPDGs", m_rejectPDGs);
}

/// Default destructor
Geant4PrimaryHandler::~Geant4PrimaryHandler()  {
  InstanceCount::decrement(this);
}

/// Event generation action callback
void Geant4PrimaryHandler::operator()(G4Event* event)  {
  generatePrimaries(this, context(), event);
}
