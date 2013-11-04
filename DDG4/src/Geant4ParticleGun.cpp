// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4PARTICLEGUN_H
#define DD4HEP_DDG4_GEANT4PARTICLEGUN_H

// Framework include files
#include "DDG4/Geant4GeneratorAction.h"

// Forward declarations
class G4ParticleDefinition;
class G4ParticleGun;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {
    /** @class Geant4ParticleGun Geant4ParticleGun.h DDG4/Geant4ParticleGun.h
     * 
     * Implementation wrapper of the Geant4 particle gun
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4ParticleGun : public Geant4GeneratorAction  {
    protected:
      /// Position and shooting direction of the gun
      struct { double x,y,z; } m_position, m_direction;
      /// Particle energy
      double                   m_energy;
      /// Desired multiplicity of the particles to be shot
      int                      m_multiplicity;
      /// Particle name
      std::string              m_particleName;
      /// Pointer to geant4 particle definition
      G4ParticleDefinition*    m_particle;
      /// Pointer to the particle gun itself
      G4ParticleGun*           m_gun;
    public:
      /// Standard constructor
      Geant4ParticleGun(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4ParticleGun();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif /* DD4HEP_DDG4_GEANT4PARTICLEGUN_H  */

// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
//#include "DDG4/Geant4ParticleGun.h"
#include "DDG4/Factories.h"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep::Simulation;

DECLARE_GEANT4ACTION(Geant4ParticleGun)

/// Standard constructor
Geant4ParticleGun::Geant4ParticleGun(Geant4Context* context, const std::string& name)
: Geant4GeneratorAction(context,name), m_particle(0), m_gun(0)
{
  InstanceCount::increment(this);
  declareProperty("energy",       m_energy=50*MeV);
  declareProperty("multiplicity", m_multiplicity=1);
  declareProperty("pos_x",        m_position.x = 0);
  declareProperty("pos_y",        m_position.y = 0);
  declareProperty("pos_z",        m_position.z = 0);
  declareProperty("direction_x",  m_direction.x = 1);
  declareProperty("direction_y",  m_direction.y = 1);
  declareProperty("direction_z",  m_direction.z = 0.3);
  declareProperty("particle",     m_particleName= "e-");
}

/// Default destructor
Geant4ParticleGun::~Geant4ParticleGun()  {
  if ( m_gun ) delete m_gun;
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void  Geant4ParticleGun::operator()(G4Event* event) { 
  if ( 0 == m_gun ) {
    m_gun = new G4ParticleGun(m_multiplicity);
  }
  if ( 0 == m_particle || m_particle->GetParticleName() != m_particleName.c_str() )  {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    m_particle = particleTable->FindParticle(m_particleName);
    if ( 0 == m_particle )   {
      throw std::runtime_error("Bad particle type!");
    }
  }
  m_gun->SetParticleDefinition(m_particle);
  m_gun->SetParticleEnergy(m_energy);
  m_gun->SetParticleMomentumDirection(G4ThreeVector(m_direction.x,m_direction.y,m_direction.z));
  m_gun->SetParticlePosition(G4ThreeVector(m_position.x,m_position.y,m_position.z));
  m_gun->GeneratePrimaryVertex(event);
}
