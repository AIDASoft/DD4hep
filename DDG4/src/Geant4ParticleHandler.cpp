// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
//#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4UserParticleHandler.h"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4TrackStatus.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include <set>
#include <stdexcept>
#include <algorithm>

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
Geant4ParticleHandler::Geant4ParticleHandler(Geant4Context* context, const string& nam)
  : Geant4GeneratorAction(context,nam), Geant4MonteCarloTruth(), m_userHandler(0), m_primaryMap(0)
{
  //generatorAction().adopt(this);
  eventAction().callAtBegin(this,&Geant4ParticleHandler::beginEvent);
  eventAction().callAtEnd(this,&Geant4ParticleHandler::endEvent);
  trackingAction().callAtFinal(this,&Geant4ParticleHandler::end,CallbackSequence::FRONT);
  trackingAction().callUpFront(this,&Geant4ParticleHandler::begin,CallbackSequence::FRONT);
  steppingAction().call(this,&Geant4ParticleHandler::step);
  m_globalParticleID = 0;
  declareProperty("PrintEndTracking",    m_printEndTracking = false);
  declareProperty("PrintStartTracking",  m_printStartTracking = false);
  declareProperty("KeepAllParticles",    m_keepAll = false);
  declareProperty("SaveProcesses",       m_processNames);
  declareProperty("MinimalKineticEnergy",m_kinEnergyCut = 100e0*MeV);
  InstanceCount::increment(this);
}

/// No default constructor
Geant4ParticleHandler::Geant4ParticleHandler() : Geant4GeneratorAction(0,"") {
}

/// Default destructor
Geant4ParticleHandler::~Geant4ParticleHandler()  {
  clear();
  releasePtr(m_userHandler);
  InstanceCount::decrement(this);
}

/// No assignment operator
Geant4ParticleHandler& Geant4ParticleHandler::operator=(const Geant4ParticleHandler&) { 
  return *this; 
}

/// Adopt the user particle handler
bool Geant4ParticleHandler::adopt(Geant4UserParticleHandler* action)    {
  if ( action )   {
    if ( !m_userHandler )  {
      Geant4UserParticleHandler* h = dynamic_cast<Geant4UserParticleHandler*>(action);
      if ( h )  {
	m_userHandler = h;
	m_userHandler->addRef();
	return true;
      }
      except("Cannot add an invalid user particle handler object [Invalid-object-type].", c_name());
    }
    except("Cannot add an user particle handler object [Object-exists].", c_name());
  }
  except("Cannot add an invalid user particle handler object [NULL-object].", c_name());
  return false;
}

/// Clear particle maps
void Geant4ParticleHandler::clear()  {
  releaseObjects(m_particleMap)();
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
    mask.set(G4PARTICLE_CREATED_TRACKER_HIT);
  }
}

/// Event generation action callback
void Geant4ParticleHandler::operator()(G4Event* event)  {
  typedef Geant4MonteCarloTruth _MC;
  info("+++ Event:%d Add EVENT extension of type Geant4ParticleHandler.....",event->GetEventID());
  context()->event().addExtension((_MC*)this, typeid(_MC), 0);
  clear();
  /// Call the user particle handler
  if ( m_userHandler )  {
    m_userHandler->generate(event, this);
  }
}

/// User stepping callback
void Geant4ParticleHandler::step(const G4Step* step, G4SteppingManager* mgr)   {
  typedef vector<const G4Track*> _Sec;
  Geant4StepHandler h(step);
  ++m_currTrack.steps;
  const G4ThreeVector& v = h.postPosG4();
  m_currTrack.vex = v.x();
  m_currTrack.vey = v.y();
  m_currTrack.vez = v.z();
  if ( h.trkKineEnergy() > m_kinEnergyCut )  {
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
  /// Update of the particle using the user handler
  if ( m_userHandler )  {
    m_userHandler->step(step, mgr, m_currTrack);
  }
}

/// Pre-track action callback
void Geant4ParticleHandler::begin(const G4Track* track)   {
  Geant4TrackHandler h(track);
  double kine = h.kineticEnergy();
  G4ThreeVector m = track->GetMomentum();
  const G4ThreeVector& v = h.vertex();
  int reason = (kine > m_kinEnergyCut) ? G4PARTICLE_ABOVE_ENERGY_THRESHOLD : 0;
  G4PrimaryParticle* prim = primary(h.id(),context()->event().event());
  Particle* prim_part = 0;


  if ( prim )   {
    Geant4PrimaryMap::Primaries::const_iterator iprim = m_primaryMap->primaryMap.find(prim);
    if ( iprim == m_primaryMap->primaryMap.end() )  {
      except("+++ Tracking preaction: Primary particle without generator particle!");
    }
    prim_part = (*iprim).second;
    reason |= (G4PARTICLE_PRIMARY|G4PARTICLE_ABOVE_ENERGY_THRESHOLD);
    m_particleMap[h.id()] = prim_part->addRef();
  }

  if ( prim_part )   {
    m_currTrack.id           = prim_part->id;
    m_currTrack.reason       = prim_part->reason|reason;
    m_currTrack.usermask     = prim_part->usermask;
    m_currTrack.status       = prim_part->status;
    m_currTrack.spin[0]      = prim_part->spin[0];
    m_currTrack.spin[1]      = prim_part->spin[1];
    m_currTrack.spin[2]      = prim_part->spin[2];
    m_currTrack.colorFlow[0] = prim_part->colorFlow[0];
    m_currTrack.colorFlow[1] = prim_part->colorFlow[1];
    m_currTrack.parents      = prim_part->parents;
    m_currTrack.daughters    = prim_part->daughters;
    m_currTrack.definition   = prim_part->definition;
    m_currTrack.pdgID        = prim_part->pdgID;
    m_currTrack.mass         = prim_part->mass;
  }
  else  {
    m_currTrack.id           = m_globalParticleID;
    m_currTrack.reason       = reason;
    m_currTrack.usermask     = 0;
    m_currTrack.status      |= G4PARTICLE_SIM_CREATED;
    m_currTrack.spin[0]      = 0;
    m_currTrack.spin[1]      = 0;
    m_currTrack.spin[2]      = 0;
    m_currTrack.colorFlow[0] = 0;
    m_currTrack.colorFlow[1] = 0;
    m_currTrack.parents.clear();
    m_currTrack.daughters.clear();    
    m_currTrack.definition   = h.trackDef();
    m_currTrack.pdgID        = h.trackDef()->GetPDGEncoding();
    m_currTrack.mass         = h.trackDef()->GetPDGMass();
    // Once the daughter gets simulated, the parent is already done....
    ParticleMap::iterator ipar = m_particleMap.find(h.parent());
    if ( ipar != m_particleMap.end() )  {
      Particle* p_par = (*ipar).second;
      m_currTrack.parents.insert(p_par->id);
    }
    ++m_globalParticleID;
  }
  m_currTrack.steps       = 0;
  m_currTrack.secondaries = 0;
  m_currTrack.g4Parent    = h.parent();
  m_currTrack.process     = h.creatorProcess();
  m_currTrack.time        = h.globalTime();
  m_currTrack.vsx         = v.x();
  m_currTrack.vsy         = v.y();
  m_currTrack.vsz         = v.z();
  m_currTrack.vex         = 0.0;
  m_currTrack.vey         = 0.0;
  m_currTrack.vez         = 0.0;
  m_currTrack.psx         = m.x();
  m_currTrack.psy         = m.y();
  m_currTrack.psz         = m.z();
  m_currTrack.pex         = 0.0;
  m_currTrack.pey         = 0.0;
  m_currTrack.pez         = 0.0;
  // If the creator process of the track is in the list of process products to be kept, set the proper flag
  if ( m_currTrack.process )  {
    Processes::iterator i=find(m_processNames.begin(),m_processNames.end(),m_currTrack.process->GetProcessName());
    if ( i != m_processNames.end() )  {
      PropertyMask(m_currTrack.reason).set(G4PARTICLE_KEEP_PROCESS);
    }
  }
  if ( m_keepAll )  {
    PropertyMask(m_currTrack.reason).set(G4PARTICLE_KEEP_ALWAYS);    
  }

  /// Initial update of the particle using the user handler
  if ( m_userHandler )  {
    m_userHandler->begin(track, m_currTrack);
  }
}

/// Post-track action callback
void Geant4ParticleHandler::end(const G4Track* track)   {
  Geant4TrackHandler h(track);
  int g4_id = h.id();
  PropertyMask mask(m_currTrack.reason);

  if ( !mask.isNull() )   {
    // These are candate tracks with a probability to be stored due to their properties:
    // - primary particle
    // - hits created
    // - secondaries
    // - above energy threshold
    // - to be kept due to creator process 
    //
    // Update vertex end point and final momentum
    Geant4ParticleHandle ph(&m_currTrack);
    G4ThreeVector m = track->GetMomentum();
    ph->pex = m.x();
    ph->pey = m.y();
    ph->pez = m.z();
    m_equivalentTracks[g4_id] = g4_id;
    // Add this track to it's parents list of daughters
    ParticleMap::iterator ipar = m_particleMap.find(ph->g4Parent);
    if ( ipar != m_particleMap.end() )  {
      Particle* p_par = (*ipar).second;
      p_par->daughters.insert(ph->id);
      ph->usermask |= p_par->usermask;
      G4ThreeVector dist(ph->vsx-p_par->vex,ph->vsy-p_par->vey,ph->vsz-p_par->vez);
      if ( dist.r() > 1e-15 )   {
	// Set flag that the end vertex of the parent is not the start vertex of this track....
      }
    }
    /// Final update of the particle using the user handler
    if ( m_userHandler )  {
      m_userHandler->begin(track, m_currTrack);
    }
    ParticleMap::iterator ip = m_particleMap.find(g4_id);
    if ( mask.isSet(G4PARTICLE_PRIMARY) )   {
      ph.dump2(outputLevel()-1,name(),"Add Primary",h.id(),ip!=m_particleMap.end());
    }
    // Create a new MC particle from the current track information saved in the pre-tracking action
    Particle* p = ip==m_particleMap.end() ? (m_particleMap[g4_id] = new Particle()) : (*ip).second;
    p->get_data(m_currTrack);
  }
  else   {
    // These are tracks without any special properties.
    //
    // We will not store them on the record, but have to memorise the 
    // track identifier in order to restore the history for the created hits.
    m_equivalentTracks[g4_id] = m_currTrack.g4Parent;
  }
}

/// Pre-event action callback
void Geant4ParticleHandler::beginEvent(const G4Event* event)  {
  Geant4PrimaryInteraction* interaction = context()->event().extension<Geant4PrimaryInteraction>();
  info("+++ Event %d Begin event action. Access event related information.",event->GetEventID());
  m_primaryMap = context()->event().extension<Geant4PrimaryMap>();
  m_globalParticleID = interaction->nextPID();
  m_particleMap.clear();
  m_equivalentTracks.clear();
  /// Call the user particle handler
  if ( m_userHandler )  {
    m_userHandler->begin(event);
  }
}

/// Debugging: Dump Geant4 particle map
void Geant4ParticleHandler::dumpMap(const char* tag)  const  {
  for(ParticleMap::const_iterator iend=m_particleMap.end(), i=m_particleMap.begin(); i!=iend; ++i)
    Geant4ParticleHandle((*i).second).dump1(INFO,name(),tag);
}

/// Post-event action callback
void Geant4ParticleHandler::endEvent(const G4Event* event)  {
  int count = 0;
  int level = outputLevel();
  do {
    if ( level <= VERBOSE ) dumpMap("Particle");
    print("+++ Iteration:%d Tracks:%d Equivalents:%d",++count,m_particleMap.size(),m_equivalentTracks.size());
  } while( recombineParents() > 0 );

  if ( level <= VERBOSE ) dumpMap("Recombined");
  // Rebase the simulated tracks, so that they fit to the generator particles
  rebaseSimulatedTracks(0);
  if ( level <= DEBUG ) dumpMap("Rebased");
  // Consistency check....
  checkConsistency();
  /// Call the user particle handler
  if ( m_userHandler )  {
    m_userHandler->end(event);
  }
  // Now export the data to the final record.
  Geant4ParticleMap* part_map = context()->event().extension<Geant4ParticleMap>();
  part_map->adopt(m_particleMap, m_equivalentTracks);
  m_primaryMap = 0;
  clear();
}

/// Rebase the simulated tracks, so that they fit to the generator particles
void Geant4ParticleHandler::rebaseSimulatedTracks(int )   {
  /// No we have to update the map of equivalent tracks and assign the 'equivalentTrack' entry
  TrackEquivalents equivalents, orgParticles;
  ParticleMap      finalParticles;
  ParticleMap::const_iterator ipar, iend, i;
  int count;

  Geant4PrimaryInteraction* interaction = context()->event().extension<Geant4PrimaryInteraction>();
  ParticleMap& pm = interaction->particles;

  // (1.0) Copy the pre-defined particle mapping for the simulated tracks
  //       It is assumed the mapping is ZERO based without holes.
  for(count = 0, iend=pm.end(), i=pm.begin(); i!=iend; ++i)  {
    Particle* p = (*i).second;
    orgParticles[p->id] = p->id;
    finalParticles[p->id] = p;
    if ( p->id > count ) count = p->id;
    if ( (p->reason&G4PARTICLE_PRIMARY) != G4PARTICLE_PRIMARY )  {
      p->addRef();
    }
  }
  // (1.1) Define the new particle mapping for the simulated tracks
  for(++count, iend=m_particleMap.end(), i=m_particleMap.begin(); i!=iend; ++i)  {
    Particle* p = (*i).second;
    if ( (p->reason&G4PARTICLE_PRIMARY) != G4PARTICLE_PRIMARY )  {
      //if ( orgParticles.find(p->id) == orgParticles.end() )  {
      orgParticles[p->id] = count;
      finalParticles[count] = p;
      p->id = count;
      ++count;
    }
  }
  // (2) Re-evaluate the corresponding geant4 track equivalents using the new mapping
  for(TrackEquivalents::iterator i=m_equivalentTracks.begin(),iend=m_equivalentTracks.end(); i!=iend; ++i)  {
    int g4_equiv = (*i).first;
    ParticleMap::const_iterator ipar;
    while( (ipar=m_particleMap.find(g4_equiv)) == m_particleMap.end() )  {
      TrackEquivalents::const_iterator iequiv = m_equivalentTracks.find(g4_equiv);
      if ( iequiv == iend )
	break;  // ERROR !! Will be handled by printout below because ipar==end()
      g4_equiv = (*iequiv).second;
    }
    if ( ipar != m_particleMap.end() )
      equivalents[(*i).first] = (*ipar).second->id;  // requires (1) !
    else
      error("+++ No Equivalent particle for track:%d last known is:%d",(*i).second,g4_equiv);
  }
  m_equivalentTracks = equivalents;
  equivalents.clear();

  // (3) Update the particle's parents and replace the original Geant4 track with the
  //     equivalent particle still present in the record.
  for(ParticleMap::const_iterator ipar, iend=m_particleMap.end(), i=m_particleMap.begin(); i!=iend; ++i)  {
    Particle* p = (*i).second;
    set<int> daughters = p->daughters;
    p->daughters.clear();
    for(set<int>::iterator id=daughters.begin(); id!=daughters.end(); ++id)
      p->daughters.insert(orgParticles[*id]);             // Requires (1)
    if ( 0 == (p->status&G4PARTICLE_GEN_HISTORY) )  {
      if ( p->g4Parent > 0 )  {
	p->parents.clear();
	p->parents.insert(equivalentTrack(p->g4Parent));  // Requires (2)
	ipar = m_particleMap.find(p->g4Parent);
	if ( ipar != iend )   {
	  set<int>& dau = (*ipar).second->daughters;
	  if ( dau.find(p->id) == dau.end() ) dau.insert(p->id);
	}
      }
    }
  }
  m_particleMap = finalParticles;
}

/// Clean the monte carlo record. Remove all unwanted stuff.
/// This is the core of the object executed at the end of each event action.
int Geant4ParticleHandler::recombineParents()  {
  int break_trackID = 38;
  set<int> remove;
  /// Need to start from BACK, to clean first the latest produced stuff.
  /// Otherwise the daughter list of the earlier produced tracks would not be empty!
  for(ParticleMap::reverse_iterator i=m_particleMap.rbegin(); i!=m_particleMap.rend(); ++i)  {    
    int g4_id = (*i).first;
    Particle* p = (*i).second;
    set<int>& daughters = p->daughters;
    if ( daughters.size() == 0 )   {
      PropertyMask mask(p->reason);
      int  id             =  p->id;
      bool secondaries    =  mask.isSet(G4PARTICLE_HAS_SECONDARIES);
      bool tracker_track  =  mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT);
      bool calo_track     =  mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT);
      bool hits_produced  =  mask.isSet(G4PARTICLE_CREATED_HIT);
      bool low_energy     = !mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD);
      bool keep_process   =  mask.isSet(G4PARTICLE_KEEP_PROCESS);
      bool keep_parent    =  mask.isSet(G4PARTICLE_KEEP_PARENT);
      bool remove_me      = false;

      if ( id == break_trackID )   {  // Used for debugging to set break point
	remove_me      = false;
      }

      /// Primary particles MUST be kept!
      if ( mask.isSet(G4PARTICLE_PRIMARY) )   {
	continue;
      }
      else if ( keep_parent )  {
	//continue;
      }
      else if ( keep_process )  {
	ParticleMap::iterator ip = m_particleMap.find(p->g4Parent);
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
	ParticleMap::iterator ip = m_particleMap.find(p->g4Parent);
	remove.insert(g4_id);
	m_equivalentTracks[g4_id] = p->g4Parent;
	if ( ip != m_particleMap.end() )   {
	  Particle* parent_part = (*ip).second;
	  PropertyMask(parent_part->reason).set(mask.value());
	  // Remove track from parent's list of daughters
	  parent_part->removeDaughter(id);
	  parent_part->steps += p->steps;
	  parent_part->secondaries += p->secondaries;
	  /// Update of the particle using the user handler
	  if ( m_userHandler )  {
	    m_userHandler->combine(*p, *parent_part);
	  }
	}
      }
    }
  }
  for(set<int>::const_iterator r=remove.begin(); r!=remove.end();++r)  {
    ParticleMap::iterator ir = m_particleMap.find(*r);
    if ( ir != m_particleMap.end() )  {
      (*ir).second->release();
      m_particleMap.erase(ir);
    }
  }
  return int(remove.size());
}

/// Check the record consistency
void Geant4ParticleHandler::checkConsistency()  const   {
  int num_errors = 0;

  /// First check the consistency of the particle map itself
  for(ParticleMap::const_iterator j, i=m_particleMap.begin(); i!=m_particleMap.end(); ++i)  {
    Particle* p = (*i).second;
    PropertyMask mask(p->reason);
    PropertyMask status(p->status);
    set<int>& daughters = p->daughters;
    // For all particles, the set of daughters must be contained in the record.
    for(set<int>::const_iterator id=daughters.begin(); id!=daughters.end(); ++id)   {
      int id_dau = *id;
      if ( (j=m_particleMap.find(id_dau)) == m_particleMap.end() )   {
	++num_errors;
	error("+++ Particle:%d Daughter %d is not in particle map!",p->id,id_dau);
      }
    }
    // We assume that particles from the generator have consistent parents
    // For all other particles except the primaries, the parent must be contained in the record.
    if ( !mask.isSet(G4PARTICLE_PRIMARY) && !status.isSet(G4PARTICLE_GEN_HISTORY) )  {
      int parent_id = equivalentTrack(p->g4Parent);
      bool in_map = (j=m_particleMap.find(parent_id)) != m_particleMap.end();
      bool in_parent_list = p->parents.find(parent_id) != p->parents.end();
      char parent_list[1024];
      parent_list[0] = 0;
      if ( !in_map || !in_parent_list )  {
	++num_errors;
	for(set<int>::const_iterator ip=p->parents.begin(); ip!=p->parents.end();++ip)
	  ::snprintf(parent_list+strlen(parent_list),sizeof(parent_list)-strlen(parent_list),"%d ",*ip);
	error("+++ Particle:%d Parent %d (G4id:%d)  In record:%s In parent list:%s [%s]",
		 p->id,parent_id,p->g4Parent,yes_no(in_map),yes_no(in_parent_list),parent_list);
      }
    }
  }

  /// No we have to check the consistency of the map of equivalent tracks used to assign the
  /// proper MC particle to the created hits
  for(TrackEquivalents::const_iterator i=m_equivalentTracks.begin(), iend=m_equivalentTracks.end(); i!=iend; ++i)  {
    int g4_id    = (*i).first;
    int equiv_id = equivalentTrack(g4_id);
    if ( equiv_id < 0 )  {
      ++num_errors;
    }
  }

  if ( num_errors > 0 )  {
    except("+++ Consistency check failed. Found %d problems.",num_errors);
  }
}

/// Get proper equivalent track from the particle map according to the given geant4 track ID
int Geant4ParticleHandler::equivalentTrack(int g4_id)  const  {
  int equiv_id  = g4_id;
  if ( g4_id != 0 )  {
    TrackEquivalents::const_iterator iequiv = m_equivalentTracks.find(equiv_id);
    return (iequiv == m_equivalentTracks.end()) ? -1 : (*iequiv).second;
  }
  return -1;
}

/// Access the equivalent track id (shortcut to the usage of TrackEquivalents)
int Geant4ParticleHandler::particleID(int track, bool) const    {
  return equivalentTrack(track);
}
