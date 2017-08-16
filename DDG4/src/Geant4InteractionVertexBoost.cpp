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
#include "DDG4/Geant4InteractionVertexBoost.h"

using namespace dd4hep::sim;

/// Standard constructor
Geant4InteractionVertexBoost::Geant4InteractionVertexBoost(Geant4Context* ctxt, const std::string& nam)
  : Geant4GeneratorAction(ctxt, nam)
{
  InstanceCount::increment(this);
  declareProperty("Angle", m_angle = 0);
  declareProperty("Mask",  m_mask = 1);
  m_needsControl = true;
}

/// Default destructor
Geant4InteractionVertexBoost::~Geant4InteractionVertexBoost() {
  InstanceCount::decrement(this);
}

/// Action to boost one single interaction according to the properties
void Geant4InteractionVertexBoost::boost(Interaction* inter)  const  {
  if ( inter )  {
    boostInteraction(this, inter, m_angle);
    return;
  }
  print("+++ No interaction of type %d present.",m_mask);
}

/// Callback to generate primary particles
void Geant4InteractionVertexBoost::operator()(G4Event*) {
  typedef std::vector<Geant4PrimaryInteraction*> _I;
  Geant4PrimaryEvent* evt = context()->event().extension<Geant4PrimaryEvent>();

  if ( m_mask >= 0 )  {
    Interaction* inter = evt->get(m_mask);
    boost(inter);
    return;
  }
  _I interactions = evt->interactions();
  for(_I::iterator i=interactions.begin(); i != interactions.end(); ++i)
    boost(*i);
}
