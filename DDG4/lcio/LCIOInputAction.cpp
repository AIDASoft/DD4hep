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
#include "G4Event.hh"
#include "EVENT/MCParticle.h"
#include "EVENT/LCCollection.h"
#include "Randomize.hh"

using namespace std;
using namespace DD4hep::Simulation;

typedef DD4hep::ReferenceBitMask<int> PropertyMask;

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

namespace {
  inline int GET_ENTRY(const map<EVENT::MCParticle*,int>& mcparts, EVENT::MCParticle* part)  {
    map<EVENT::MCParticle*,int>::const_iterator ip=mcparts.find(part);
    if ( ip == mcparts.end() )  {
      throw runtime_error("Unknown particle identifier look-up!");
    }
    return (*ip).second;
  }
  struct FindVertex {
    double x,y,z,t;
    FindVertex(double xx, double yy, double zz, double tt) { x=xx; y=yy; z=zz; t=tt; }
    bool operator()(const Geant4Vertex* o)   const  {
      if ( fabs(o->x - x) > numeric_limits<double>::epsilon() ) return false;
      if ( fabs(o->y - y) > numeric_limits<double>::epsilon() ) return false;
      if ( fabs(o->z - z) > numeric_limits<double>::epsilon() ) return false;
      if ( fabs(o->time - t) > numeric_limits<double>::epsilon() ) return false;
      return true;
    }
  };
}

/// Callback to generate primary particles
void LCIOInputAction::operator()(G4Event* event)   {
  Geant4Event& evt = context()->event();
  Geant4PrimaryEvent* prim = evt.extension<Geant4PrimaryEvent>();
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  EVENT::LCCollection* primaries = readParticles(event->GetEventID());
  map<EVENT::MCParticle*,int> mcparts;
  vector<EVENT::MCParticle*> mcpcoll;
  vector<Particle*> col;

  prim->add(m_mask, inter);
  // check if there is at least one particle
  if ( 0 == primaries ) return;

  int NHEP = primaries->getNumberOfElements();
  // check if there is at least one particle
  if ( NHEP == 0 ) return;

  col.resize(NHEP,0);
  mcpcoll.resize(NHEP,0);
  for(int i=0; i<NHEP; ++i ) {
    EVENT::MCParticle* p=dynamic_cast<EVENT::MCParticle*>(primaries->getElementAt(i));
    mcparts[p] = i;
    mcpcoll[i] = p;
    col[i] = new Particle(i);
  }

  print("+++ Particle interaction with %d generator particles ++++++++++++++++++++++++",NHEP);
  // build collection of MCParticles
  G4ParticleTable*      tab = G4ParticleTable::GetParticleTable();
  for(int i=0; i<NHEP; ++i )   {
    EVENT::MCParticle* mcp = mcpcoll[i];
    Geant4ParticleHandle p(col[i]);
    const double mom_scale = m_momScale;
    const double *mom   = mcp->getMomentum();
    const double *vsx   = mcp->getVertex();
    const double *vex   = mcp->getEndpoint();
    const float  *spin  = mcp->getSpin();
    const int    *color = mcp->getColorFlow();
    G4ParticleDefinition* def = tab->FindParticle(mcp->getPDG());
    PropertyMask status(p->status);

    p->reason       = 0;
    p->usermask     = 0;
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
    status.set(G4PARTICLE_GEN_CREATED);
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
    inter->particles.insert(make_pair(p->id,p));
    p.dump3(outputLevel()-1,name(),"+->");
  }

  // put initial particles to the vertex
  vector<Geant4Vertex*> vertices;
  double time_end = 0.0;

  for( size_t i=0; i< col.size(); i++ )  {
    Particle* p = col[i];
    if ( p->time > time_end ) time_end = p->time;
  }

  // Create Primary Vertex object
  set<int> missing;
  for( size_t i=0; i< col.size(); i++ )  {
    Geant4ParticleHandle p(col[i]);
    PropertyMask reason(p->reason);
    PropertyMask status(p->status);
    bool empty   = status.isSet(G4PARTICLE_GEN_EMPTY);
    bool stable  = status.isSet(G4PARTICLE_GEN_STABLE);
    bool decayed = status.isSet(G4PARTICLE_GEN_DECAYED);
    bool docu    = status.isSet(G4PARTICLE_GEN_DOCUMENTATION);

    if ( docu || empty || decayed )   {
      status.set(G4PARTICLE_GEN_HISTORY);
    }
    else if ( stable )   {
      missing.insert(i);
      reason.set(G4PARTICLE_PRIMARY);
    }
    else if ( !stable )  {
      double ene = p.energy();
      double proper_time = std::fabs((time_end - p->time) * p->mass) / ene;
      double precision   = std::pow(10E0,-DBL_DIG)*std::max(time_end,p->time)*p->mass/ene;
      if ( proper_time >= precision )  {
	p->properTime = proper_time;
      }
      reason.set(G4PARTICLE_PRIMARY);
      missing.insert(i);
    }
    else  { // Catchall: will not make it to the primary record.....
      status.set(G4PARTICLE_GEN_HISTORY);
    }
  }
  set<Geant4Particle*> missing_def;
  for(set<int>::iterator i=missing.begin(); i != missing.end(); ++i)   {
    Geant4ParticleHandle p(col[*i]);
    Geant4Vertex* vtx = 0;
    PropertyMask reason(p->reason);

    if ( 0 == p->definition ) {
      missing_def.insert(p);
      continue;
    }
    vector<Geant4Vertex*>::iterator iv=find_if(vertices.begin(),vertices.end(),
					       FindVertex(p->vsx,p->vsy,p->vsz,p->time));
    if ( iv == vertices.end() )  {
      vtx = new Geant4Vertex();
      vtx->x = p->vsx;
      vtx->y = p->vsy;
      vtx->z = p->vsz;
      vtx->time = p->time;
      vertices.push_back(vtx);
      inter->vertices.insert(make_pair(inter->vertices.size(),vtx));
      vtx->out.insert(*i);
    }
    else  {
      (*iv)->out.insert(*i);
    }
  }
  if ( missing_def.size() > 0 )  {
    for(set<Geant4Particle*>::const_iterator i=missing_def.begin(); i!= missing_def.end(); ++i)
      Geant4ParticleHandle(*i).dump1(WARNING,name(),"!!!! Missing definition");
    //abortRun(issue(event->GetEventID()),"Error creating primary particles.",
    // "+++ Missing particle definitions for primary particles. Geant4 cannot handle this!");
  }

#if 0
  // put initial particles to the vertex
  for( size_t i=0; i< col.size(); i++ )  {
    Particle* p = col[i];
    if ( p->parents.size()==0 )   {
      set<int> outgoing = relevantParticles(col, p);
      prim_vtx->out.insert(outgoing.begin(),outgoing.end());
    }
  }
#endif
}

set<int> LCIOInputAction::relevantParticles(const vector<Particle*>& particles, Geant4ParticleHandle p) const    {
  set<int> result;
  PropertyMask mask(p->status);
  // CASE1: if p is a stable particle: search for it in G4ParticleMap
  // if it is already there: get G4PrimaryParticle version of p from G4ParticleMap
  // else: create G4PrimaryParticle version of p and write it to G4ParticleMap
  // in the end: insert this single G4PrimaryParticle verion of p to the 
  // relevant particle list and return this "list".
  if ( mask.isSet(G4PARTICLE_GEN_STABLE) )  {
    result.insert(p->id);
    return result;
  }

  if ( p->daughters.size() > 0 )   {
    // calculate proper time
    Particle::Particles::iterator id;
    int idau = *p->daughters.begin();
    Particle* dp = particles[idau];
    double ten = 10;
    double mass = p->definition->GetPDGMass();
    double proper_time = std::fabs((dp->time - p->time) * mass) / p.energy();
    double aPrecision = dp->time * mass / p.energy();
    double bPrecision =  p->time * mass / p.energy();
    double proper_time_precision = std::pow(ten,-DBL_DIG)
      * std::max(std::fabs(aPrecision),std::fabs(bPrecision));
    bool isProperTimeZero = ( proper_time <= proper_time_precision );

    // CASE2: if p is not stable: get first decay daughter and calculate the proper time of p
    // if proper time is not zero: particle is "relevant", since it carries vertex information
    // if p is already in G4ParticleMap: take it
    // otherwise: create G4PrimaryParticle version of p and write it to G4ParticleMap
    // now collect all relevant particles of all daughters and setup "relevant mother-
    // daughter-relations" between relevant decay particles and G4PrimaryParticle version of p
    // in the end: insert only the G4PrimaryParticle version of p to the
    // relevant particle list and return this "list". The added particle has now the correct pre-assigned
    // decay products and (hopefully) the right lifetime.
    if ( isProperTimeZero == false ) {
      result.insert(p->id);
      p->properTime = proper_time;
      for (id=p->daughters.begin(); id!=p->daughters.end(); ++id)  {
	int pid = *id;
	dp = particles[pid];
	set<int> dau = relevantParticles(particles,dp);
	result.insert(pid);
	result.insert(dau.begin(),dau.end());
      }
      return result;
    }
    // CASE3: if p is not stable AND has zero lifetime: forget about p and retrieve all relevant
    // decay particles of all daughters of p. In this case this step of recursion is just there for
    // collecting all relevant decay products of daughters (and return them).
    for (id=p->daughters.begin(); id!=p->daughters.end(); ++id)  {
      int pid = *id;
      dp = particles[pid];
      set<int> dau = relevantParticles(particles,dp);
      result.insert(dau.begin(),dau.end());
    }
  }
  // This logic sucks!
  // - What happens to decayed beauty particles with a displaced vertex, which are stable?
  // - What happend to anything stable comping NOT from the very primary vertex.....
  return result;
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(LCIOInputAction)
