// $Id: $
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
#include "DDG4/Geant4Random.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4InputHandling.h"
#include "DDG4/Geant4InteractionVertexSmear.h"

// C/C++ include files
#include <cmath>

using namespace dd4hep::sim;

/// Standard constructor
Geant4InteractionVertexSmear::Geant4InteractionVertexSmear(Geant4Context* ctxt, const std::string& nam)
  : Geant4GeneratorAction(ctxt, nam)
{
  InstanceCount::increment(this);
  declareProperty("Offset", m_offset);
  declareProperty("Sigma",  m_sigma);
  declareProperty("Mask",   m_mask = 1);
  m_needsControl = true;
}

/// Default destructor
Geant4InteractionVertexSmear::~Geant4InteractionVertexSmear() {
  InstanceCount::decrement(this);
}


/// Action to smear one single interaction according to the properties
void Geant4InteractionVertexSmear::smear(Interaction* inter)  const  {
  Geant4Random& rndm = context()->event().random();
  if ( inter )  {
    double dx = rndm.gauss(m_offset.x(),m_sigma.x());
    double dy = rndm.gauss(m_offset.y(),m_sigma.y());
    double dz = rndm.gauss(m_offset.z(),m_sigma.z());
    double dt = rndm.gauss(m_offset.t(),m_sigma.t());
    print("+++ Smearing primary vertex for interaction type %d (%d Vertices, %d particles) "
          "by (%+.2e mm, %+.2e mm, %+.2e mm, %+.2e ns)",
          m_mask,int(inter->vertices.size()),int(inter->particles.size()),dx,dy,dz,dt);
    smearInteraction(this,inter,dx,dy,dz,dt);
    return;
  }
  print("+++ No interaction of type %d present.",m_mask);
}

/// Callback to generate primary particles
void Geant4InteractionVertexSmear::operator()(G4Event*) {
  typedef std::vector<Geant4PrimaryInteraction*> _I;
  Geant4PrimaryEvent* evt = context()->event().extension<Geant4PrimaryEvent>();

  if ( m_mask >= 0 )  {
    Interaction* inter = evt->get(m_mask);
    smear(inter);
    return;
  }
  _I interactions = evt->interactions();
  for(_I::iterator i=interactions.begin(); i != interactions.end(); ++i)
    smear(*i);
}
