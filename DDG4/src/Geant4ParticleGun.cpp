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
#include <DD4hep/Printout.h>
#include <DD4hep/InstanceCount.h>
#include <DDG4/Geant4Context.h>
#include <DDG4/Geant4Primary.h>
#include <DDG4/Geant4ParticleGun.h>
#include <DDG4/Geant4InputHandling.h>
#include <CLHEP/Units/SystemOfUnits.h>

// C/C++ include files
#include <limits>

using namespace dd4hep::sim;

/// Standard constructor
Geant4ParticleGun::Geant4ParticleGun(Geant4Context* ctxt, const std::string& nam)
  : Geant4IsotropeGenerator(ctxt,nam), m_shotNo(0)
{
  InstanceCount::increment(this);
  m_needsControl = true;
  declareProperty("Standalone",   m_standalone = true);
  declareProperty("mask",         m_mask);
  declareProperty("isotrop",      m_isotrop = false);
  declareProperty("standalone",   m_standalone);
  // Backwards compatibility: Un-capitalize
  declareProperty("position",     m_position);
  declareProperty("distribution", m_distribution);
  declareProperty("direction",    m_direction);
  declareProperty("particle",     m_particleName);
  declareProperty("multiplicity", m_multiplicity);
  declareProperty("print",        m_print = true);
}

/// Default destructor
Geant4ParticleGun::~Geant4ParticleGun() {
  InstanceCount::decrement(this);
}

/// Particle modification. Caller presets defaults to: ( direction = m_direction, momentum = [mMin, mMax])
void Geant4ParticleGun::getParticleDirection(int num, ROOT::Math::XYZVector& direction, double& momentum) const  {
  ( m_isotrop )
    ? this->Geant4IsotropeGenerator::getParticleDirection(num, direction, momentum)
    : this->Geant4ParticleGenerator::getParticleDirection(num, direction, momentum);
}

/// Callback to generate primary particles
void Geant4ParticleGun::operator()(G4Event* event)   {
  double r = m_direction.R(), eps = std::numeric_limits<float>::epsilon();
  if ( r > eps )  {
    m_direction.SetXYZ(m_direction.X()/r, m_direction.Y()/r, m_direction.Z()/r);
  }

  if ( m_standalone )  {
    generationInitialization(this,context());
  }

  //bit wasteful to always set this :(
  if( m_energy != -1 ){
    m_momentumMin = m_energy;
    m_momentumMax = m_energy;
  }

  print("Shoot [%d] [%.3f , %.3f] GeV %s pos:(%.3f %.3f %.3f)[mm] dir:(%6.3f %6.3f %6.3f)",
        m_shotNo, m_momentumMin/CLHEP::GeV, m_momentumMax/CLHEP::GeV, m_particleName.c_str(),
        m_position.X()/CLHEP::mm, m_position.Y()/CLHEP::mm, m_position.Z()/CLHEP::mm,
        m_direction.X(), m_direction.Y(), m_direction.Z());
  this->Geant4ParticleGenerator::operator()(event);
  if ( m_print )   {
    this->Geant4ParticleGenerator::printInteraction(m_mask);
  }
  ++m_shotNo;
  if ( m_standalone ) {
    mergeInteractions(this,context());
    generatePrimaries(this,context(),event);
  }
}
