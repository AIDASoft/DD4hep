//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// @author  P.Kostka (main author)
// @author  M.Frank  (code reshuffeling into new DDG4 scheme)
//
//====================================================================

// Framework include files
#include "LCIOEventReader.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Factories.h"

#include "G4ParticleTable.hh"
#include "EVENT/MCParticle.h"
#include "EVENT/LCCollection.h"

#include "G4Event.hh"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;
typedef dd4hep::detail::ReferenceBitMask<int> PropertyMask;

// Neede for backwards compatibility:
namespace dd4hep{namespace sim{typedef Geant4InputAction LCIOInputAction;}}
DECLARE_GEANT4ACTION(LCIOInputAction)


namespace {
  inline int GET_ENTRY(const map<EVENT::MCParticle*,int>& mcparts, EVENT::MCParticle* part)  {
    map<EVENT::MCParticle*,int>::const_iterator ip=mcparts.find(part);
    if ( ip == mcparts.end() )  {
      throw runtime_error("Unknown particle identifier look-up!");
    }
    return (*ip).second;
  }
}


/// Initializing constructor
LCIOEventReader::LCIOEventReader(const string& nam)
  : Geant4EventReader(nam)
{
}

/// Default destructor
LCIOEventReader::~LCIOEventReader()   {
}


/// Read an event and fill a vector of MCParticles.
LCIOEventReader::EventReaderStatus
LCIOEventReader::readParticles(int event_number, 
                               Vertices& vertices,
                               vector<Particle*>& particles)
{
  EVENT::LCCollection*        primaries = 0;
  map<EVENT::MCParticle*,int> mcparts;
  vector<EVENT::MCParticle*>  mcpcoll;
  EventReaderStatus ret = readParticleCollection(event_number,&primaries);

  if ( ret != EVENT_READER_OK ) return ret;

  int NHEP = primaries->getNumberOfElements();
  // check if there is at least one particle
  if ( NHEP == 0 ) return EVENT_READER_NO_PRIMARIES;

  mcpcoll.resize(NHEP,0);
  for(int i=0; i<NHEP; ++i ) {
    EVENT::MCParticle* p = dynamic_cast<EVENT::MCParticle*>(primaries->getElementAt(i));
    mcparts[p] = i;
    mcpcoll[i] = p;
  }

  // build collection of MCParticles
  for(int i=0; i<NHEP; ++i )   {
    EVENT::MCParticle* mcp = mcpcoll[i];
    Geant4ParticleHandle p(new Particle(i));
    const double *mom   = mcp->getMomentum();
    const double *vsx   = mcp->getVertex();
    const double *vex   = mcp->getEndpoint();
    const float  *spin  = mcp->getSpin();
    const int    *color = mcp->getColorFlow();
    const int     pdg   = mcp->getPDG();
    PropertyMask status(p->status);
    p->pdgID        = pdg;
    p->charge       = int(mcp->getCharge()*3.0);
    p->psx          = mom[0]*CLHEP::GeV;
    p->psy          = mom[1]*CLHEP::GeV;
    p->psz          = mom[2]*CLHEP::GeV;
    p->time         = mcp->getTime()*CLHEP::ns;
    p->properTime   = mcp->getTime()*CLHEP::ns;
    p->vsx          = vsx[0]*CLHEP::mm;
    p->vsy          = vsx[1]*CLHEP::mm;
    p->vsz          = vsx[2]*CLHEP::mm;
    p->vex          = vex[0]*CLHEP::mm;
    p->vey          = vex[1]*CLHEP::mm;
    p->vez          = vex[2]*CLHEP::mm;
    p->process      = 0;
    p->spin[0]      = spin[0];
    p->spin[1]      = spin[1];
    p->spin[2]      = spin[2];
    p->colorFlow[0] = color[0];
    p->colorFlow[0] = color[1];
    p->mass         = mcp->getMass()*CLHEP::GeV;
    const EVENT::MCParticleVec &par = mcp->getParents(), &dau=mcp->getDaughters();
    for(int num=dau.size(),k=0; k<num; ++k)
      p->daughters.insert(GET_ENTRY(mcparts,dau[k]));
    for(int num=par.size(),k=0; k<num; ++k)
      p->parents.insert(GET_ENTRY(mcparts,par[k]));

    int genStatus = mcp->getGeneratorStatus();
    if ( genStatus == 0 ) status.set(G4PARTICLE_GEN_EMPTY);
    else if ( genStatus == 1 ) status.set(G4PARTICLE_GEN_STABLE);
    else if ( genStatus == 2 ) status.set(G4PARTICLE_GEN_DECAYED);
    else if ( genStatus == 3 ) status.set(G4PARTICLE_GEN_DOCUMENTATION);
    else if ( genStatus == 4 ) status.set(G4PARTICLE_GEN_BEAM);
    else
      status.set(G4PARTICLE_GEN_OTHER);
    // Copy raw generator status
    p->genStatus = genStatus&G4PARTICLE_GEN_STATUS_MASK;

    //fg: we simply add all particles without parents as with their own vertex.
    //    This might include the incoming beam particles, e.g. in
    //    the case of lcio files written with Whizard2, which is slightly odd,
    //    however should be treated correctly in Geant4InputHandling.cpp.
    //    We no longer make an attempt to identify the incoming particles
    //    based on the generator status, as this varies widely with different
    //    generators.

    if ( p->parents.size() == 0 )  {

      Geant4Vertex* vtx = new Geant4Vertex ;
      vertices.emplace_back( vtx );
      vtx->x = p->vsx;
      vtx->y = p->vsy;
      vtx->z = p->vsz;
      vtx->time = p->time;

      vtx->out.insert(p->id) ;
    }

    if ( mcp->isCreatedInSimulation() )       status.set(G4PARTICLE_SIM_CREATED);
    if ( mcp->isBackscatter() )               status.set(G4PARTICLE_SIM_BACKSCATTER);
    if ( mcp->vertexIsNotEndpointOfParent() ) status.set(G4PARTICLE_SIM_PARENT_RADIATED);
    if ( mcp->isDecayedInTracker() )          status.set(G4PARTICLE_SIM_DECAY_TRACKER);
    if ( mcp->isDecayedInCalorimeter() )      status.set(G4PARTICLE_SIM_DECAY_CALO);
    if ( mcp->hasLeftDetector() )             status.set(G4PARTICLE_SIM_LEFT_DETECTOR);
    if ( mcp->isStopped() )                   status.set(G4PARTICLE_SIM_STOPPED);
    if ( mcp->isOverlay() )                   status.set(G4PARTICLE_SIM_OVERLAY);
    particles.emplace_back(p);
  }
  return EVENT_READER_OK;
}

