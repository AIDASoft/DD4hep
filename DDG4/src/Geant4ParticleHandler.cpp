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
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4MonteCarloRecordManager.h"
#include "DDG4/Geant4ParticleHandler.h"

#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4TrackStatus.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <set>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

typedef ReferenceBitMask<int> PropertyMask;

namespace {
  G4PrimaryParticle* primary(int id, const G4Event& evt)   {
    for(int i=0, ni=evt.GetNumberOfPrimaryVertex(); i<ni; ++i)  {
      G4PrimaryVertex* v = evt.GetPrimaryVertex(i);
      for(int j=0, nj=v->GetNumberOfParticle(); j<nj; ++j)  {
	G4PrimaryParticle* p = v->GetPrimary(j);
	if ( id == p->GetTrackID() )   {
	  return p;
	}
      }
    }
    return 0;
  }
}

/// Standard constructor
Geant4ParticleHandler::Geant4ParticleHandler(Geant4Context* context, const std::string& nam)
  : Geant4GeneratorAction(context,nam), Geant4MonteCarloTruth()
{
  //generatorAction().adopt(this);
  eventAction().callAtBegin(this,&Geant4ParticleHandler::beginEvent);
  eventAction().callAtEnd(this,&Geant4ParticleHandler::endEvent);
  trackingAction().callAtFinal(this,&Geant4ParticleHandler::end,CallbackSequence::FRONT);
  trackingAction().callUpFront(this,&Geant4ParticleHandler::begin,CallbackSequence::FRONT);
  steppingAction().call(this,&Geant4ParticleHandler::step);

  declareProperty("printEndTracking",m_printEndTracking = false);
  declareProperty("printStartTracking",m_printStartTracking = false);
  declareProperty("saveProcesses",m_processNames);
  declareProperty("minimalKineticEnergy",m_kinEnergyCut = 100e0*MeV);
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
void Geant4ParticleHandler::mark(const G4Track* track, int reason)   {
  if ( track )   {
    if ( reason != 0 )  {
      PropertyMask(m_currTrack.reason).set(reason);
      return;
    }
  }
  except("Cannot mark the G4Track if the pointer is invalid!", c_name());
}

/// Store a track produced in a step to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Step* step, int reason)   {
  if ( step )  {
    mark(step->GetTrack(),reason);
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
  PropertyMask mask(m_currTrack.reason);
  mask.set(G4PARTICLE_CREATED_HIT);
  /// Check if the track origines from the calorimeter. 
  // If yes, flag it, because it is a candidate fro removal.
  G4VPhysicalVolume* vol = track->GetVolume();
  if ( strstr(vol->GetName().c_str(),"cal") )  { // just for test!
    mask.set(G4PARTICLE_CREATED_CALORIMETER_HIT);
  }
  else if ( !mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT) )  {
    //printout(INFO,name(),"+++ Create TRACKER HIT: id=%d",m_currTrack.id);
    mask.set(G4PARTICLE_CREATED_TRACKER_HIT);
  }
}

/// Event generation action callback
void Geant4ParticleHandler::operator()(G4Event*)  {
  typedef Geant4MonteCarloTruth _MC;
  printout(INFO,name(),"+++ Add EVENT extension of type Geant4ParticleHandler.....");
  context()->event().addExtension((_MC*)this, typeid(_MC), 0);
  clear();
}

/// User stepping callback
void Geant4ParticleHandler::step(const G4Step* step, G4SteppingManager* /* mgr */)   {
  typedef std::vector<const G4Track*> _Sec;
  ++m_currTrack.steps;
  if ( m_currTrack.energy > m_kinEnergyCut )  {
    //
    // Tracks below the energy threshold are NOT stored.
    // If these tracks produce hits or are selected due to another signature, 
    // this criterium will anyhow take precedence.
    //
    const _Sec* sec=step->GetSecondaryInCurrentStep();
    if ( sec->size() > 0 )  {
      PropertyMask(m_currTrack.reason).set(G4PARTICLE_HAS_SECONDARIES);
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
  m_currTrack.reason     = (kine > m_kinEnergyCut) ? G4PARTICLE_ABOVE_ENERGY_THRESHOLD : 0;
  m_currTrack.energy     = kine;
  m_currTrack.g4Parent   = h.parent();
  m_currTrack.parent     = h.parent();
  m_currTrack.definition = h.trackDef();
  m_currTrack.process    = h.creatorProcess();
  m_currTrack.time       = h.globalTime();
  m_currTrack.vsx        = h.vertex().x();
  m_currTrack.vsy        = h.vertex().y();
  m_currTrack.vsz        = h.vertex().z();
  m_currTrack.vex        = 0.0;
  m_currTrack.vey        = 0.0;
  m_currTrack.vez        = 0.0;
  m_currTrack.psx        = m.x();
  m_currTrack.psy        = m.y();
  m_currTrack.psz        = m.z();
  m_currTrack.pex        = 0.0;
  m_currTrack.pey        = 0.0;
  m_currTrack.pez        = 0.0;
  // If the creator process of the track is in the list of process products to be kept, set the proper flag
  if ( m_currTrack.process )  {
    Processes::iterator i=find(m_processNames.begin(),m_processNames.end(),m_currTrack.process->GetProcessName());
    if ( i != m_processNames.end() )  {
      PropertyMask(m_currTrack.reason).set(G4PARTICLE_KEEP_PROCESS);
    }
  }
}

/// Post-track action callback
void Geant4ParticleHandler::end(const G4Track* track)   {
  Geant4TrackHandler h(track);
  int id = h.id();
  PropertyMask mask(m_currTrack.reason);
  G4PrimaryParticle* prim = primary(id,context()->event().event());

  if ( prim )   {
    m_currTrack.reason |= (G4PARTICLE_PRIMARY|G4PARTICLE_ABOVE_ENERGY_THRESHOLD);
    const G4ParticleDefinition* def = m_currTrack.definition;
    printout(INFO,name(),"+++ Add Primary particle %d def:%p [%s, %s] reason:%d",
	     id, def, 
	     def ? def->GetParticleName().c_str() : "UNKNOWN",
	     def ? def->GetParticleType().c_str() : "UNKNOWN", m_currTrack.reason);
  }

  if ( !mask.isNull() )   {
    // These are candate tracks with a probability to be stored due to their properties:
    // - primary particle
    // - hits created
    // - secondaries
    // - above energy threshold
    // - to be kept due to creator process 
    //
    // Update vertex end point and final momentum
    G4ThreeVector m = track->GetMomentum();
    m_currTrack.vex = h.vertex().x();
    m_currTrack.vey = h.vertex().y();
    m_currTrack.vez = h.vertex().z();
    m_currTrack.pex = m.x();
    m_currTrack.pey = m.y();
    m_currTrack.pez = m.z();

    // Create a new MC particle from the current track information saved in the pre-tracking action
    Particle* p = m_particleMap[id] = new Particle(m_currTrack);
    // Add this track to it's parents list of daughters
    ParticleMap::iterator ipar = m_particleMap.find(p->g4Parent);
    if ( ipar != m_particleMap.end() )  {
      Particle* p_par = (*ipar).second;
      p_par->daughters.insert(id);
    }
    m_equivalentTracks[id] = id;
#if 0
    /// Some printout
    const char* hit  = mask.isSet(G4PARTICLE_CREATED_HIT) ? "CREATED_HIT" : "";
    const char* sec  = mask.isSet(G4PARTICLE_HAS_SECONDARIES) ? "SECONDARIES" : "";
    const char* prim = mask.isSet(G4PARTICLE_PRIMARY) ? "PRIMARY" : "";
    printout(INFO,name(),"+++ Tracking postaction: %d/%d par:%d [%s, %s] created by:%s E:%e state:%s %s %s %s",
	     id,int(m_particleMap.size()),
	     h.parent(),h.name().c_str(),h.type().c_str(),h.creatorName().c_str(),
	     p->energy, h.statusName(), prim, hit, sec);
#endif
  }
  else   {
    // These are tracks without any special properties.
    //
    // We will not store them on the record, but have to memorise the 
    // track identifier in order to restore the history for the created hits.
    m_equivalentTracks[id] = h.parent();
  }
}

/// Pre-event action callback
void Geant4ParticleHandler::beginEvent(const G4Event* )  {
}

/// Post-event action callback
void Geant4ParticleHandler::endEvent(const G4Event* )  {
  int count = 0;
  do {
    printout(INFO,name(),"+++ Iteration:%d Tracks:%d Equivalents:%d",++count,m_particleMap.size(),m_equivalentTracks.size());
  } while( recombineParents() > 0 );
  // Update the particle's parents and replace the original Geant4 track with the
  // equivalent particle still present in the record.
  for(ParticleMap::const_iterator ipar, iend=m_particleMap.end(), i=m_particleMap.begin(); i!=iend; ++i)  {
    Particle* p = (*i).second;
    p->parent = equivalentTrack(p->g4Parent);
    if ( p->parent > 0 )  {
      ipar = m_particleMap.find(p->parent);
      if ( ipar != iend )   {
	set<int>& daughters = (*ipar).second->daughters;
	if ( daughters.find(p->id) == daughters.end() ) daughters.insert(p->id);
      }
    }
  }
  /// No we have to updte the map of equivalent tracks and assign the 'equivalentTrack' entry
  for(TrackEquivalents::iterator i=m_equivalentTracks.begin(); i!=m_equivalentTracks.end(); ++i)  {
    int& track_id = (*i).second;
    int  equiv_id = equivalentTrack(track_id);
    track_id = equiv_id;
  }
  // Consistency check....
  checkConsistency();
}

int Geant4ParticleHandler::recombineParents()  {
  set<int> remove;
  /// Need to start from BACK, to clean first the latest produced stuff.
  /// Otherwise the daughter list of the earlier produced tracks would not be empty!
  for(ParticleMap::reverse_iterator i=m_particleMap.rbegin(); i!=m_particleMap.rend(); ++i)  {    
    Particle* par = (*i).second;
    set<int>& daughters = par->daughters;
    if ( daughters.size() == 0 )   {
      PropertyMask mask(par->reason);
      int  id             =  par->id;
      bool secondaries    =  mask.isSet(G4PARTICLE_HAS_SECONDARIES);
      bool tracker_track  =  mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT);
      bool calo_track     =  mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT);
      bool hits_produced  =  mask.isSet(G4PARTICLE_CREATED_HIT);
      bool low_energy     = !mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD);
      bool keep_process   =  mask.isSet(G4PARTICLE_KEEP_PROCESS);
      bool keep_parent    =  mask.isSet(G4PARTICLE_KEEP_PARENT);
      int  parent_id      = par->g4Parent;
      bool remove_me      = false;

      /// Primary particles MUST be kept!
      if ( mask.isSet(G4PARTICLE_PRIMARY) )   {
	continue;
      }
      else if ( keep_parent )  {
	continue;
      }
      else if ( keep_process )  {
	ParticleMap::iterator ip = m_particleMap.find(parent_id);
	if ( ip != m_particleMap.end() )   {
	  Particle* parent_part = (*ip).second;
	  PropertyMask parent_mask(parent_part->reason);
	  if ( parent_mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD) )   {
	    parent_mask.set(G4PARTICLE_KEEP_PARENT);
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
	  PropertyMask(parent_part->reason).set(mask.value());
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

/// Check the record consistency
void Geant4ParticleHandler::checkConsistency()  const   {
  int num_errors = 0;

  /// First check the consistency of the particle map itself
  for(ParticleMap::const_iterator j, i=m_particleMap.begin(); i!=m_particleMap.end(); ++i)  {
    Particle* p = (*i).second;
    PropertyMask mask(p->reason);
    set<int>& daughters = p->daughters;
    // For all particles, the set of daughters must be contained in the record.
    for(set<int>::const_iterator id=daughters.begin(); id!=daughters.end(); ++id)   {
      int id_dau = *id;
      j = m_particleMap.find(id_dau);
      if ( j == m_particleMap.end() )   {
	++num_errors;
	printout(INFO,name(),"+++ Particle:%d Daughter %d is not in particle map!",p->id,id_dau);
      }
    }
    // For all particles except the primaries, the parent must be contained in the record.
    if ( !mask.isSet(G4PARTICLE_PRIMARY) )  {
      int id_par = p->parent;
      j = m_particleMap.find(id_par);
      if ( j == m_particleMap.end() )   {
	++num_errors;
	printout(INFO,name(),"+++ Particle:%d Parent %d is not in particle map!",p->id,id_par);
      }
    }
  }

  /// No we have to check the consistency of the map of equivalent tracks used to assign the
  /// proper MC particle to the created hits
  for(TrackEquivalents::const_iterator i=m_equivalentTracks.begin(); i!=m_equivalentTracks.end(); ++i)  {
    int track_id = (*i).second;
    int equiv_id = equivalentTrack(track_id);
    ParticleMap::const_iterator j = m_particleMap.find(equiv_id);
    if ( j == m_particleMap.end() )   {
      int g4_id = (*i).first;
      ++num_errors;
      printout(INFO,name(),"+++ Geant4 Track:%d Track:%d Equivalent track %d is not in particle map!",g4_id,track_id,equiv_id);
    }
  }

  if ( num_errors > 0 )  {
    printout(ERROR,name(),"+++ Consistency check failed. Found %d problems.",num_errors);
  }
}

/// Get proper equivalent track from the particle map according to the given geant4 track ID
int Geant4ParticleHandler::equivalentTrack(int g4_id)  const  {
  int equiv_id  = g4_id;
  if ( g4_id != 0 )  {
    ParticleMap::const_iterator ipar;
    while( (ipar=m_particleMap.find(equiv_id)) == m_particleMap.end() )  {
      TrackEquivalents::const_iterator iequiv = m_equivalentTracks.find(equiv_id);
      equiv_id = (iequiv == m_equivalentTracks.end()) ? -1 : (*iequiv).second;
      if ( equiv_id < 0 ) {
	printout(INFO,name(),"+++ No Equivalent particle for track:%d last known is:%d",g4_id,equiv_id);
	break;
      }
    }
  }
  return equiv_id;
}
