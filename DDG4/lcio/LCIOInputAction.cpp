// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
// @author  P.Kostka (main author)
// @author  M.Frank  (code reshuffeling into new DDG4 scheme)
//
//====================================================================

// Framework include files
#include "LCIOInputAction.h"
#include "LCIOEventReader.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"
#include "G4ParticleTable.hh"
#include "EVENT/MCParticle.h"
#include "EVENT/LCCollection.h"

#include "G4Event.hh"

using namespace std;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

namespace {
  inline int GET_ENTRY(const map<EVENT::MCParticle*,int>& mcparts, EVENT::MCParticle* part)  {
    map<EVENT::MCParticle*,int>::const_iterator ip=mcparts.find(part);
    if ( ip == mcparts.end() )  {
      throw runtime_error("Unknown particle identifier look-up!");
    }
    return (*ip).second;
  }
}

/// Standard constructor
LCIOInputAction::LCIOInputAction(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context,name), m_reader(0)
{
  declareProperty("Input",          m_input);
  declareProperty("Sync",           m_SYNCEVT=0);
  declareProperty("Mask",           m_mask = 0);
  declareProperty("MomentumScale",  m_momScale = 1.0);
  m_needsControl = true;
}

/// Default destructor
LCIOInputAction::~LCIOInputAction()   {
}

/// helper to report Geant4 exceptions
string LCIOInputAction::issue(int i)  const  {
  stringstream str;
  str << "LCIOInputAction[" << name() << "]: Event " << i << " ";
  return str.str();
}

/// Read an event and return a LCCollection of MCParticles.
EVENT::LCCollection* LCIOInputAction::readParticles(int which)  {
  int evid = which + m_SYNCEVT;
  if ( 0 == m_reader )  {
    if ( m_input.empty() )  {
      throw runtime_error("InputAction: No inoput file declared!");
    }
    string err;
    TypeName tn = TypeName::split(m_input,"|");
    try  {
      m_reader = PluginService::Create<LCIOEventReader*>(tn.first,tn.second,int(0));
      if ( 0 == m_reader )   {
	abortRun(issue(evid)+"Error creating reader plugin.",
		 "Failed to create file reader of type %s. Cannot open dataset %s",
		 tn.first.c_str(),tn.second.c_str());
	return 0;
      }
    }
    catch(const std::exception& e)  {
      err = e.what();
    }
    if ( !err.empty() )  {
      abortRun(issue(evid)+err,"Error when reading file %s",m_input.c_str());
      return 0;
    }
  }
  EVENT::LCCollection* parts = m_reader->readParticles();
  if ( 0 == parts )  {
    abortRun(issue(evid)+"Error when reading file - may be end of file.",
	     "Error when reading file %s",m_input.c_str());
  }
  return parts;
}

/// Callback to generate primary particles
void LCIOInputAction::operator()(G4Event* event)   {
  Geant4Event& evt = context()->event();
  Geant4PrimaryEvent*         prim = evt.extension<Geant4PrimaryEvent>();
  Geant4PrimaryInteraction*   inter = new Geant4PrimaryInteraction();
  EVENT::LCCollection*        primaries = readParticles(event->GetEventID());
  map<EVENT::MCParticle*,int> mcparts;
  vector<EVENT::MCParticle*>  mcpcoll;

  prim->add(m_mask, inter);
  // check if there is at least one particle
  if ( 0 == primaries ) return;

  int NHEP = primaries->getNumberOfElements();
  // check if there is at least one particle
  if ( NHEP == 0 ) return;

  mcpcoll.resize(NHEP,0);
  for(int i=0; i<NHEP; ++i ) {
    EVENT::MCParticle* p=dynamic_cast<EVENT::MCParticle*>(primaries->getElementAt(i));
    mcparts[p] = i;
    mcpcoll[i] = p;
  }

  print("+++ Particle interaction with %d generator particles ++++++++++++++++++++++++",NHEP);
  Geant4Vertex* vtx = new Geant4Vertex();
  vtx->x = 0;
  vtx->y = 0;
  vtx->z = 0;
  vtx->time = 0;
  inter->vertices.insert(make_pair(m_mask,vtx));
  // build collection of MCParticles
  G4ParticleTable*      tab = G4ParticleTable::GetParticleTable();
  for(int i=0; i<NHEP; ++i )   {
    EVENT::MCParticle* mcp = mcpcoll[i];
    Geant4ParticleHandle p(new Particle(i));
    const double mom_scale = m_momScale;
    const double *mom   = mcp->getMomentum();
    const double *vsx   = mcp->getVertex();
    const double *vex   = mcp->getEndpoint();
    const float  *spin  = mcp->getSpin();
    const int    *color = mcp->getColorFlow();
    G4ParticleDefinition* def = tab->FindParticle(mcp->getPDG());
    PropertyMask status(p->status);
    p->pdgID        = mcp->getPDG();
    p->charge       = int(mcp->getCharge()*3.0);
    p->psx          = mom_scale*mom[0]*GeV;
    p->psy          = mom_scale*mom[1]*GeV;
    p->psz          = mom_scale*mom[2]*GeV;
    p->time         = mcp->getTime()*ns;
    p->properTime   = mcp->getTime()*ns;
    p->vsx          = vsx[0]*mm;
    p->vsy          = vsx[1]*mm;
    p->vsz          = vsx[2]*mm;
    p->vex          = vex[0]*mm;
    p->vey          = vex[1]*mm;
    p->vez          = vex[2]*mm;
    p->definition   = def;
    p->process      = 0;
    p->spin[0]      = spin[0];
    p->spin[1]      = spin[1];
    p->spin[2]      = spin[2];
    p->colorFlow[0] = color[0];
    p->colorFlow[0] = color[1];
    p->mass         = mcp->getMass()*GeV;
    const EVENT::MCParticleVec &par = mcp->getParents(), &dau=mcp->getDaughters();
    for(int num=dau.size(),k=0; k<num; ++k)
      p->daughters.insert(GET_ENTRY(mcparts,dau[k]));
    for(int num=par.size(),k=0; k<num; ++k)
      p->parents.insert(GET_ENTRY(mcparts,par[k]));

    int genStatus = mcp->getGeneratorStatus();
    if ( genStatus == 0 ) status.set(G4PARTICLE_GEN_EMPTY);
    if ( genStatus == 1 ) status.set(G4PARTICLE_GEN_STABLE); 
    if ( genStatus == 2 ) status.set(G4PARTICLE_GEN_DECAYED);
    if ( genStatus == 3 ) status.set(G4PARTICLE_GEN_DOCUMENTATION);
    if ( mcp->isCreatedInSimulation() )       status.set(G4PARTICLE_SIM_CREATED);
    if ( mcp->isBackscatter() )               status.set(G4PARTICLE_SIM_BACKSCATTER);
    if ( mcp->vertexIsNotEndpointOfParent() ) status.set(G4PARTICLE_SIM_PARENT_RADIATED);
    if ( mcp->isDecayedInTracker() )          status.set(G4PARTICLE_SIM_DECAY_TRACKER);
    if ( mcp->isDecayedInCalorimeter() )      status.set(G4PARTICLE_SIM_DECAY_CALO);
    if ( mcp->hasLeftDetector() )             status.set(G4PARTICLE_SIM_LEFT_DETECTOR);
    if ( mcp->isStopped() )                   status.set(G4PARTICLE_SIM_STOPPED);
    if ( mcp->isOverlay() )                   status.set(G4PARTICLE_SIM_OVERLAY);
    if ( par.size() == 0 )  {
      if ( status.isSet(G4PARTICLE_GEN_EMPTY) || status.isSet(G4PARTICLE_GEN_DOCUMENTATION) )
	vtx->in.insert(p->id);  // Beam particles and primary quarks etc.
      else
	vtx->out.insert(p->id); // Stuff, to be given to Geant4 together with daughters
    }
    inter->particles.insert(make_pair(p->id,p));
    p.dump3(outputLevel()-1,name(),"+->");
  }
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(LCIOInputAction)
