//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//====================================================================

// Framework include files
#include "HepMC3EventReader.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Factories.h"

#include <G4ParticleTable.hh>

#include <HepMC3/FourVector.h>
#include <HepMC3/GenEvent.h>
#include <HepMC3/GenParticle.h>
#include <HepMC3/GenVertex.h>
#include <HepMC3/Units.h>

#include "G4Event.hh"

using dd4hep::sim::HEPMC3EventReader;
using SGenParticle = std::shared_ptr<HepMC3::GenParticle>;
using PropertyMask = dd4hep::detail::ReferenceBitMask<int>;

namespace {
  template<class T> inline int GET_ENTRY(const std::map<T,int>& mcparts, T part) {
    auto ip = mcparts.find(part);
    if (ip == mcparts.end())  {
      throw std::runtime_error("Unknown particle identifier look-up!");
    }
    return (*ip).second;
  }
}

/// Initializing constructor
HEPMC3EventReader::HEPMC3EventReader(const string& fileName): Geant4EventReader(fileName) {}

/// Read an event and fill a vector of MCParticles.
HEPMC3EventReader::EventReaderStatus
HEPMC3EventReader::readParticles(int event_number, Vertices& vertices, Particles& particles) {
  HepMC3::GenEvent genEvent;
  std::map<SGenParticle, int> mcparts;
  std::vector<SGenParticle>  mcpcoll;
  EventReaderStatus ret = readGenEvent(event_number, genEvent);

  if ( ret != EVENT_READER_OK ) return ret;

  int NHEP = genEvent.particles().size();
  // check if there is at least one particle
  if ( NHEP == 0 ) return EVENT_READER_NO_PRIMARIES;

  mcpcoll.resize(NHEP,0);
  for(int i=0; i<NHEP; ++i ) {
    auto p = genEvent.particles().at(i);
    mcparts[p] = i;
    mcpcoll[i] = p;
  }

  double mom_unit = (genEvent.momentum_unit() == HepMC3::Units::MomentumUnit::GEV) ? CLHEP::GeV : CLHEP::MeV;
  double len_unit = (genEvent.length_unit() == HepMC3::Units::LengthUnit::MM) ? CLHEP::mm : CLHEP::cm;
  // build collection of MCParticles
  for(int i=0; i<NHEP; ++i )   {
    auto const& mcp = mcpcoll[i];
    Geant4ParticleHandle p(new Particle(i));
    auto const& mom = mcp->momentum();
    auto  vsx       = mcp->production_vertex()->position();
    auto  vex       = (mcp->end_vertex()) ?  mcp->end_vertex()->position() : HepMC3::FourVector();

    const float spin[]  = {0.0, 0.0, 0.0}; //mcp->getSpin(); // FIXME
    const int   color[] = {0, 0}; //mcp->getColorFlow(); // FIXME
    const int   pdg     = mcp->pid();
    PropertyMask status(p->status);
    p->pdgID        = pdg;
    p->charge       = 0; // int(mcp->getCharge()*3.0); // FIXME
    p->psx          = mom.get_component(0) * mom_unit;
    p->psy          = mom.get_component(1) * mom_unit;
    p->psz          = mom.get_component(2) * mom_unit;
    p->time         = vsx.get_component(3) * CLHEP::ns; // FIXME
    p->properTime   = vsx.get_component(3) * CLHEP::ns; // FIXME
    p->vsx          = vsx.get_component(0) * len_unit;
    p->vsy          = vsx.get_component(1) * len_unit;
    p->vsz          = vsx.get_component(2) * len_unit;
    p->vex          = vex.get_component(0) * len_unit;
    p->vey          = vex.get_component(1) * len_unit;
    p->vez          = vex.get_component(2) * len_unit;
    p->process      = 0;
    p->spin[0]      = spin[0];
    p->spin[1]      = spin[1];
    p->spin[2]      = spin[2];
    p->colorFlow[0] = color[0];
    p->colorFlow[0] = color[1];
    p->mass         = mcp->generated_mass()*mom_unit; // FIXME
    auto const &par = mcp->parents(), &dau=mcp->children();
    for(int num=dau.size(), k=0; k<num; ++k)
      p->daughters.insert(GET_ENTRY(mcparts,dau[k]));
    for(int num=par.size(), k=0; k<num; ++k)
      p->parents.insert(GET_ENTRY(mcparts,par[k]));

    int genStatus = mcp->status();
    if ( genStatus == 0 ) status.set(G4PARTICLE_GEN_EMPTY);
    else if ( genStatus == 1 ) status.set(G4PARTICLE_GEN_STABLE);
    else if ( genStatus == 2 ) status.set(G4PARTICLE_GEN_DECAYED);
    else if ( genStatus == 11 ) status.set(G4PARTICLE_GEN_DECAYED); // FIXME
    else if ( genStatus == 3 ) status.set(G4PARTICLE_GEN_DOCUMENTATION);
    else if ( genStatus == 4 ) status.set(G4PARTICLE_GEN_BEAM);
    else
      status.set(G4PARTICLE_GEN_OTHER);
    // Copy raw generator status
    p->genStatus = genStatus&G4PARTICLE_GEN_STATUS_MASK;

    if ( p->parents.size() == 0 )  {

      Geant4Vertex* vtx = new Geant4Vertex ;
      vertices.emplace_back( vtx );
      vtx->x = p->vsx;
      vtx->y = p->vsy;
      vtx->z = p->vsz;
      vtx->time = p->time;

      vtx->out.insert(p->id) ;
    }

    particles.emplace_back(p);
  }
  return EVENT_READER_OK;
}
