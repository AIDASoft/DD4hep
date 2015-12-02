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
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4ParticleGun.h"
#include "DDG4/Geant4InputHandling.h"
#include "CLHEP/Units/SystemOfUnits.h"

// C/C++ include files
#include <limits>

using namespace std;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ParticleGun::Geant4ParticleGun(Geant4Context* ctxt, const string& nam)
  : Geant4IsotropeGenerator(ctxt,nam), m_shotNo(0)
{
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("isotrop",      m_isotrop = false);
  declareProperty("Standalone",   m_standalone = true);
  // Backwards compatibility: Un-capitalize
  declareProperty("position",     m_position);
  declareProperty("direction",    m_direction);
  declareProperty("energy",       m_energy);
  declareProperty("particle",     m_particleName);
  declareProperty("multiplicity", m_multiplicity);
}

/// Default destructor
Geant4ParticleGun::~Geant4ParticleGun() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4ParticleGun::operator()(G4Event* event)   {
  if ( m_isotrop )   {
    double mom = 0.0;
    this->Geant4IsotropeGenerator::getParticleDirection(0,m_direction,mom);
  }
  else  {
    double r = m_direction.R(), eps = numeric_limits<float>::epsilon();
    if ( r > eps )  {
      m_direction.SetXYZ(m_direction.X()/r, m_direction.Y()/r, m_direction.Z()/r);
    }
  }

  if ( m_standalone )  {
    generationInitialization(this,context());
  }
  this->Geant4ParticleGenerator::operator()(event);
  print("Shoot [%d] %.3f GeV %s pos:(%.3f %.3f %.3f)[mm] dir:(%6.3f %6.3f %6.3f)",
        m_shotNo, m_energy/CLHEP::GeV, m_particleName.c_str(),
        m_position.X()/CLHEP::mm, m_position.Y()/CLHEP::mm, m_position.Z()/CLHEP::mm,
        m_direction.X(),m_direction.Y(), m_direction.Z());

  this->Geant4ParticleGenerator::printInteraction();
  ++m_shotNo;
  if ( m_standalone ) {
    mergeInteractions(this,context());
    generatePrimaries(this,context(),event);
  }
}
