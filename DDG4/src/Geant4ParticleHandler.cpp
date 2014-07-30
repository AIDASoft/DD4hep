// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4MonteCarloRecordManager.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4StepHandler.h"

#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4TrackStatus.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"

#include <set>
#include <stdexcept>

using namespace std;
using DD4hep::InstanceCount;
using namespace DD4hep;
using namespace DD4hep::Simulation;


static double m_kinEnergyCut = 500e0;

enum { CREATED_HIT = 1<<0, 
       PRIMARY_PARTICLE = 1<<1,
       SECONDARIES = 1<<2,
       ENERGY = 1<<3,
       KEEP_PROCESS = 1<<4,
       KEEP_PARENT = 1<<5,
       CREATED_CALORIMETER_HIT = 1<<6,
       CREATED_TRACKER_HIT = 1<<7,

       LAST_NOTHING = 1<<31
};


template <typename T> class BitMask  {
public:
  T& refMask;
  BitMask(T& m) : refMask(m) {}
  T value() const  {
    return refMask;
  }
  void set(const T& m)   {
    refMask |= m;
  }
  bool isSet(const T& m)   {
    return (refMask&m) == m;
  }
  bool testBit(int bit) const  {
    T m = T(1)<<bit;
    return isSet(m);
  }
  bool isNull() const { 
    return refMask == 0; 
  }
};
template <typename T> inline BitMask<T> bitMask(T& m)  { return BitMask<T>(m); }
template <typename T> inline BitMask<const T> bitMask(const T& m)  { return BitMask<const T>(m); }

/// Copy constructor
Geant4ParticleHandler::Particle::Particle(const Particle& c)
  : id(c.id), parent(c.parent), theParent(c.theParent), reason(c.reason),
    vx(c.vx), vy(c.vy), vz(c.vz), 
    px(c.px), py(c.py), pz(c.pz), 
    energy(c.energy), time(c.time),
    process(c.process), definition(c.definition),
    daughters(c.daughters)
{
  InstanceCount::increment(this);
}

/// Default constructor
Geant4ParticleHandler::Particle::Particle()
  : id(0), parent(0), theParent(0), reason(0),
    vx(0.0), vy(0.0), vz(0.0), 
    px(0.0), py(0.0), pz(0.0), 
    energy(0.0), time(0.0),
    process(0), definition(0),
    daughters()
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticleHandler::Particle::~Particle()  {
  InstanceCount::decrement(this);
}

/// Remove daughter from set
void Geant4ParticleHandler::Particle::removeDaughter(int id_daughter)  {
  set<int>::iterator j = daughters.find(id_daughter);
  if ( j != daughters.end() ) daughters.erase(j);
}

/// Standard constructor
Geant4ParticleHandler::Geant4ParticleHandler(Geant4Context* context, const std::string& nam)
  : Geant4GeneratorAction(context,nam), Geant4MonteCarloTruth()
{
  //generatorAction().adopt(this);
  eventAction().callAtBegin(this,&Geant4ParticleHandler::beginEvent);
  eventAction().callAtFinal(this,&Geant4ParticleHandler::endEvent);
  trackingAction().callAtFinal(this,&Geant4ParticleHandler::end,CallbackSequence::FRONT);
  trackingAction().callUpFront(this,&Geant4ParticleHandler::begin,CallbackSequence::FRONT);
  steppingAction().call(this,&Geant4ParticleHandler::step);

  declareProperty("PrintEndTracking",m_printEndTracking = false);
  declareProperty("PrintStartTracking",m_printStartTracking = false);
  declareProperty("SaveProcesses",m_processNames);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticleHandler::~Geant4ParticleHandler()  {
  clear();
  InstanceCount::decrement(this);
}

/// Clear particle maps
void Geant4ParticleHandler::clear()  {
  for(ParticleMap::iterator i=m_particleMap.begin(); i!=m_particleMap.end(); ++i)
    delete (*i).second;
  m_particleMap.clear();
  m_equivalentTracks.clear();
}

/// Mark a Geant4 track to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Track* track, bool created_hit)   {
  mark(track);  // Does all the checking...
  if ( created_hit )  {
  }
}

/// Store a track produced in a step to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Step* step, bool created_hit)   {
  if ( step )  {
    mark(step->GetTrack(),created_hit);
    return;
  }
  except("Cannot mark the G4Track if the step-pointer is invalid!", c_name());
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Step* step)   {
  if ( step )  {
    mark(step->GetTrack());
    return;
  }
  except("Cannot mark the G4Track if the step-pointer is invalid!", c_name());
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Track* track)   {
  BitMask<int> mask(m_currTrack.reason);
  mask.set(CREATED_HIT);
  /// Check if the track origines from the calorimeter. 
  // If yes, flag it, because it is a candidate fro removal.
  G4VPhysicalVolume* vol = track->GetVolume();
  if ( strstr(vol->GetName().c_str(),"cal") )  { // just for test!
    mask.set(CREATED_CALORIMETER_HIT);
  }
  else if ( !mask.isSet(CREATED_TRACKER_HIT) )  {
    //printout(INFO,name(),"+++ Create TRACKER HIT: id=%d",m_currTrack.id);
    mask.set(CREATED_TRACKER_HIT);
  }
}

/// Check the record consistency
void Geant4ParticleHandler::checkConsistency()  const   {
}

/// Event generation action callback
void Geant4ParticleHandler::operator()(G4Event*)  {
  typedef Geant4MonteCarloTruth _MC;
  printout(INFO,name(),"+++ Add EVENT extension of type Geant4ParticleHandler.....");
  context()->event().addExtension((_MC*)this, typeid(_MC), 0);
  clear();
}

/// Pre-event action callback
void Geant4ParticleHandler::beginEvent(const G4Event* )  {
  for(ParticleMap::const_iterator i=m_particleMap.begin(); i!=m_particleMap.end(); ++i)  {
    //Particle* part = (*i).second;
    //int equiv_id  = part->parent;
  }
}

int Geant4ParticleHandler::recombineParents()  {
  set<int> remove;
  /// Need to start from BACK, to clean first the latest produced stuff.
  /// Otherwise the daughter list of the earlier produced tracks would not be empty!
  for(ParticleMap::reverse_iterator i=m_particleMap.rbegin(); i!=m_particleMap.rend(); ++i)  {    
    Particle* par = (*i).second;
    set<int>& daughters = par->daughters;
    if ( daughters.size() == 0 )   {
      BitMask<const int> mask(par->reason);
      int  id             =  par->id;
      bool secondaries    =  mask.isSet(SECONDARIES);
      bool tracker_track  =  mask.isSet(CREATED_TRACKER_HIT);
      bool calo_track     =  mask.isSet(CREATED_CALORIMETER_HIT);
      bool hits_produced  =  mask.isSet(CREATED_HIT);
      bool low_energy     = !mask.isSet(ENERGY);
      bool keep_process   =  mask.isSet(KEEP_PROCESS);
      bool keep_parent    =  mask.isSet(KEEP_PARENT);
      int  parent_id      = par->parent;
      bool remove_me      = false;

      /// Primary particles MUST be kept!
      if ( mask.isSet(PRIMARY_PARTICLE) )   {
	continue;
      }
      else if ( keep_parent )  {
	continue;
      }
      else if ( keep_process )  {
	ParticleMap::iterator ip = m_particleMap.find(parent_id);
	if ( ip != m_particleMap.end() )   {
	  Particle* parent_part = (*ip).second;
	  BitMask<const int> parent_mask(parent_part->reason);
	  if ( parent_mask.isSet(ENERGY) )   {
	    parent_part->reason |= KEEP_PARENT;
	    continue;
	  }
	}
	// Low energy stuff. Remove it. Reassign to parent.
	//remove_me = true;
      }

      /// Remove this track if it has not created a hit and the energy is below threshold
      if ( mask.isNull() || (secondaries && low_energy && !hits_produced) )  {
	remove_me = true;
      }
      /// Remove this track if the energy is below threshold. Reassign hits to parent.
      else if ( !hits_produced && low_energy )  {
	remove_me = true;
      }
      /// Remove this track if the origine is in the calorimeter. Reassign hits to parent.
      else if ( !tracker_track && calo_track && low_energy )  {
	remove_me = true;
      }
      else  {
	//printout(INFO,name(),"+++ Track: %d should be kept for no obvious reason....",id);
      }

      /// Remove this track from the list and also do the cleanup in the parent's children list
      if ( remove_me )  {
	ParticleMap::iterator ip = m_particleMap.find(parent_id);
	remove.insert(id);
	m_equivalentTracks[id] = parent_id;
	if ( ip != m_particleMap.end() )   {
	  Particle* parent_part = (*ip).second;
	  bitMask(parent_part->reason).set(mask.value());
	  // Remove track from parent's list of daughters
	  parent_part->removeDaughter(id);
	}
      }
    }
  }
  for(set<int>::const_iterator r=remove.begin(); r!=remove.end();++r)  {
    ParticleMap::iterator ir = m_particleMap.find(*r);
    if ( ir != m_particleMap.end() )  {
      delete (*ir).second;
      m_particleMap.erase(ir);
    }
  }
  printout(INFO,name(),"+++ Size of track container:%d",int(m_particleMap.size()));
  return int(remove.size());
}

/// Pre-event action callback
void Geant4ParticleHandler::endEvent(const G4Event* )  {
  int count = 0;
  do {
    printout(INFO,name(),"+++ Iteration:%d Tracks:%d Equivalents:%d",++count,m_particleMap.size(),m_equivalentTracks.size());
  } while( recombineParents() > 0 );

  for(ParticleMap::const_iterator ipar, i=m_particleMap.begin(); i!=m_particleMap.end(); ++i)  {
    Particle* part = (*i).second;
    int equiv_id  = part->parent;
    if ( equiv_id != 0 )  {
      while( (ipar=m_particleMap.find(equiv_id)) == m_particleMap.end() )  {
	TrackEquivalents::const_iterator iequiv = m_equivalentTracks.find(equiv_id);
	equiv_id = (iequiv == m_equivalentTracks.end()) ? -1 : (*iequiv).second;
	if ( equiv_id < 0 ) {
	  printout(INFO,name(),"+++ No Equivalent particle for track:%d last known is:%d",part->id,equiv_id);
	  break;
	}
      }
      if ( equiv_id>0 && equiv_id != part->parent ) part->theParent = equiv_id;
    }
  }
  checkConsistency();
  printParticles();
}

/// Print record of kept particles
void Geant4ParticleHandler::printParticles()  {
  char equiv[32];
  ParticleMap::const_iterator ipar;
  printout(INFO,name(),"+++ MC Particles #Tracks:%6d %-12s Parent%-7s "
	   "Primary Secondaries Energy %9s Calo Tracker Process",
	   int(m_particleMap.size()),"ParticleType","","in [MeV]");
  for(ParticleMap::const_iterator i=m_particleMap.begin(); i!=m_particleMap.end(); ++i)  {
    Particle* part = (*i).second;
    BitMask<const int> mask(part->reason);
    equiv[0] = 0;
    if ( part->parent != part->theParent )  {
      ::snprintf(equiv,sizeof(equiv),"/%d",part->theParent);
    }
    printout(INFO,name(),"+++ MC Particle TrackID: %6d %-12s %6d%-7s "
	     "%-7s %-11s %-7s %8.3g %-4s %-7s %-7s [%s/%s]",
	     part->id,
	     part->definition->GetParticleName().c_str(),
	     part->parent,equiv,
	     yes_no(mask.isSet(PRIMARY_PARTICLE)),
	     yes_no(mask.isSet(SECONDARIES)),
	     yes_no(mask.isSet(ENERGY)),
	     part->energy,
	     yes_no(mask.isSet(CREATED_CALORIMETER_HIT)),
	     yes_no(mask.isSet(CREATED_TRACKER_HIT)),
	     yes_no(mask.isSet(KEEP_PROCESS)),
	     part->process ? part->process->GetProcessName().c_str() : "???",
	     part->process ? G4VProcess::GetProcessTypeName(part->process->GetProcessType()).c_str() : ""
	     );
  }
  printout(INFO,"Summary","+++ MC Particles #Tracks:%6d %-12s Parent%-7s "
	   "Primary Secondaries Energy %9s Calo Tracker Process",
	   int(m_particleMap.size()),"ParticleType","","in [MeV]");
}

/// User stepping callback
void Geant4ParticleHandler::step(const G4Step* step, G4SteppingManager* /* mgr */)   {
  typedef std::vector<const G4Track*> _Sec;
  if ( m_currTrack.energy > m_kinEnergyCut )  {
    //
    // Tracks below the energy threshold are NOT stored.
    // If these tracks produce hits or are selected due to another signature, 
    // this criterium will anyhow take precedence.
    //
    const _Sec* sec=step->GetSecondaryInCurrentStep();
    if ( sec->size() > 0 )  {
      bitMask(m_currTrack.reason).set(SECONDARIES);
    }
  }
}

/// Pre-track action callback
void Geant4ParticleHandler::begin(const G4Track* track)   {
  Geant4TrackHandler h(track);
  double kine = h.kineticEnergy();
  G4ThreeVector m = track->GetMomentum();

  // Extract here all the information from the start of the tracking action
  // which we will need later to create a proper MC particle.
  m_currTrack.id         = h.id();
  m_currTrack.reason     = (kine > m_kinEnergyCut) ? ENERGY : 0;
  m_currTrack.energy     = kine;
  m_currTrack.parent     = h.parent();
  m_currTrack.theParent  = h.parent();
  m_currTrack.definition = h.trackDef();
  m_currTrack.process    = h.creatorProcess();
  m_currTrack.time       = h.globalTime();
  m_currTrack.vx         = h.vertex().x();
  m_currTrack.vy         = h.vertex().y();
  m_currTrack.vz         = h.vertex().z();
  m_currTrack.px         = m.x();
  m_currTrack.py         = m.y();
  m_currTrack.pz         = m.z();
  // If the creator process of the track is in the list of process products to be kept, set the proper flag
  if ( m_currTrack.process )  {
    Processes::iterator i=find(m_processNames.begin(),m_processNames.end(),m_currTrack.process->GetProcessName());
    if ( i != m_processNames.end() )  {
      bitMask(m_currTrack.reason).set(KEEP_PROCESS);
    }
  }
}

static G4PrimaryParticle* primary(int id, const G4Event& evt)   {
  for(int i=0, ni=evt.GetNumberOfPrimaryVertex(); i<ni; ++i)  {
    G4PrimaryVertex* v = evt.GetPrimaryVertex(i);
    for(int j=0, nj=v->GetNumberOfParticle(); j<nj; ++j)  {
      G4PrimaryParticle* p = v->GetPrimary(i);
      if ( id == p->GetTrackID() )   {
	return p;
      }
    }
  }
  return 0;
}

/// Post-track action callback
void Geant4ParticleHandler::end(const G4Track* track)   {
  Geant4TrackHandler h(track);
  int id = h.id();
  BitMask<const int> mask(m_currTrack.reason);
  G4PrimaryParticle* prim = primary(id,context()->event().event());

  if ( prim )   {
    m_currTrack.reason |= (PRIMARY_PARTICLE|ENERGY);
    const G4ParticleDefinition* def = m_currTrack.definition;
    printout(INFO,name(),"+++ Add Primary particle %d def:%p [%s, %s] reason:%d",
	     id, def, 
	     def ? def->GetParticleName().c_str() : "UNKNOWN",
	     def ? def->GetParticleType().c_str() : "UNKNOWN", m_currTrack.reason);
  }

  if ( !mask.isNull() )   {
    // These are candate tracks with a probability to be stored due to their properties:
    // - no primary particle
    // - no hits created
    // - no secondaries
    // - not above energy threshold
    //
    // Create a new MC particle from the current track information saved in the pre-tracking action
    Particle* p = m_particleMap[id] = new Particle(m_currTrack);
    // Add this track to it's parents list of daughters
    ParticleMap::iterator ipar = m_particleMap.find(p->parent);
    if ( ipar != m_particleMap.end() )  {
      (*ipar).second->daughters.insert(id);
    }
#if 0
    /// Some printout
    const char* hit  = mask.isSet(CREATED_HIT) ? "CREATED_HIT" : "";
    const char* sec  = mask.isSet(SECONDARIES) ? "SECONDARIES" : "";
    const char* prim = mask.isSet(PRIMARY_PARTICLE) ? "PRIMARY" : "";
    printout(INFO,name(),"+++ Tracking postaction: %d/%d par:%d [%s, %s] created by:%s E:%e state:%s %s %s %s",
	     id,int(m_particleMap.size()),
	     h.parent(),h.name().c_str(),h.type().c_str(),h.creatorName().c_str(),
	     p->energy, h.statusName(), prim, hit, sec);
#endif
  }
  else   {
    // These are tracks without any special properties.
    //
    // We will not store them on the record, but have to memorise the track identifier
    // in order to restore the history for the created hits.
    m_equivalentTracks[id] = h.parent();
  }
}
