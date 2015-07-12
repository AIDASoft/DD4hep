// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDG4/Geant4InteractionVertexBoost.h"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4InteractionVertexBoost::Geant4InteractionVertexBoost(Geant4Context* ctxt, const std::string& nam)
  : Geant4GeneratorAction(ctxt, nam)
{
  InstanceCount::increment(this);
  declareProperty("Angle", m_angle = 0);
  declareProperty("Mask",  m_mask = 0);
  m_needsControl = true;
}

/// Default destructor
Geant4InteractionVertexBoost::~Geant4InteractionVertexBoost() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4InteractionVertexBoost::operator()(G4Event*) {
  Geant4PrimaryEvent::Interaction* inter =
    context()->event().extension<Geant4PrimaryEvent>()->get(m_mask);
  if ( inter )  {
    boostInteraction(this, inter, m_angle);
  }
}
