//==========================================================================
//  AIDA Detector description implementation for LCD
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
using namespace DD4hep;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

// Neede for backwards compatibility:
namespace DD4hep{namespace Simulation{typedef Geant4InputAction LCIOInputAction;}}
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

  //fg: for now we create exactly one event vertex here ( as before )
  //     and fill it below from the first final state particle 
  Geant4Vertex* vtx = new Geant4Vertex ;
  vertices.push_back( vtx );
  vtx->x = 0;
  vtx->y = 0;
  vtx->z = 0;
  vtx->time = 0;
  bool haveVertex = true ;


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
    else {
      cout << " #### WARNING - LCIOInputAction : unknown generator status : "
           << genStatus << " -> ignored ! " << endl;
    }

    // fill vertex information from first stable particle
    if( !haveVertex &&  genStatus == 1 ){
      vtx->x = p->vsx ;
      vtx->y = p->vsy ;
      vtx->z = p->vsz ;
      vtx->time = p->time ;
      haveVertex = false ;
    }

    if ( p->parents.size() == 0 )  {
      PropertyMask st(p->status);
      if ( st.isSet(G4PARTICLE_GEN_EMPTY) || st.isSet(G4PARTICLE_GEN_DOCUMENTATION) )
        vtx->in.insert(p->id);  // Beam particles and primary quarks etc.
      else
        vtx->out.insert(p->id); // Stuff, to be given to Geant4 together with daughters
    }

    if ( mcp->isCreatedInSimulation() )       status.set(G4PARTICLE_SIM_CREATED);
    if ( mcp->isBackscatter() )               status.set(G4PARTICLE_SIM_BACKSCATTER);
    if ( mcp->vertexIsNotEndpointOfParent() ) status.set(G4PARTICLE_SIM_PARENT_RADIATED);
    if ( mcp->isDecayedInTracker() )          status.set(G4PARTICLE_SIM_DECAY_TRACKER);
    if ( mcp->isDecayedInCalorimeter() )      status.set(G4PARTICLE_SIM_DECAY_CALO);
    if ( mcp->hasLeftDetector() )             status.set(G4PARTICLE_SIM_LEFT_DETECTOR);
    if ( mcp->isStopped() )                   status.set(G4PARTICLE_SIM_STOPPED);
    if ( mcp->isOverlay() )                   status.set(G4PARTICLE_SIM_OVERLAY);
    particles.push_back(p);
  }
  return EVENT_READER_OK;
}

