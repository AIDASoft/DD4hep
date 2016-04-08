// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4UserParticleHandler.h"

// Geant4 include files
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4TrackStatus.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "CLHEP/Units/SystemOfUnits.h"

// C/C++ include files
#include <set>
#include <stdexcept>
#include <algorithm>

using CLHEP::MeV;
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

typedef ReferenceBitMask<int> PropertyMask;

namespace {
  const G4PrimaryParticle* primary(int id, Geant4PrimaryMap::Primaries const& primaryMap)   {
    Geant4PrimaryMap::Primaries::const_iterator iprim =   primaryMap.begin();
    Geant4PrimaryMap::Primaries::const_iterator primEnd = primaryMap.end();
    for (; iprim != primEnd; ++iprim) {
      if( iprim->first->GetTrackID() == id ){
	return iprim->first;
      }
    }
    return 0;
  }
}

/// Standard constructor
Geant4ParticleHandler::Geant4ParticleHandler(Geant4Context* ctxt, const string& nam)
: Geant4GeneratorAction(ctxt,nam), Geant4MonteCarloTruth(), m_userHandler(0), m_primaryMap(0)
{
  InstanceCount::increment(this);
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
  declareProperty("MinimalKineticEnergy",m_kinEnergyCut = 100e0*CLHEP::MeV);
  declareProperty("MinDistToParentVertex",m_minDistToParentVertex = 2.2e-14*CLHEP::mm);//default tolerance for g4ThreeVector isNear
  m_needsControl = true;
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
bool Geant4ParticleHandler::adopt(Geant4Action* action)    {
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
void Geant4ParticleHandler::mark(const G4Step* step_value, int reason)   {
  if ( step_value )  {
    mark(step_value->GetTrack(),reason);
    return;
  }
  except("Cannot mark the G4Track if the step-pointer is invalid!", c_name());
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Step* step_value)   {
  if ( step_value )  {
    mark(step_value->GetTrack());
    return;
  }
  except("Cannot mark the G4Track if the step-pointer is invalid!", c_name());
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4ParticleHandler::mark(const G4Track* track)   {
  PropertyMask mask(m_currTrack.reason);
  mask.set(G4PARTICLE_CREATED_HIT);
  /// Check if the track origines from the calorimeter.
  // If yes, flag it, because it is a candidate for removal.
  G4LogicalVolume*       vol = track->GetVolume()->GetLogicalVolume();
  G4VSensitiveDetector*   g4 = vol->GetSensitiveDetector();
  Geant4ActionSD* sd = dynamic_cast<Geant4ActionSD*>(g4);
  string typ = sd ? sd->sensitiveType() : string();
  if ( typ == "calorimeter" )
    mask.set(G4PARTICLE_CREATED_CALORIMETER_HIT);
  else if ( typ == "tracker" )
    mask.set(G4PARTICLE_CREATED_TRACKER_HIT);
  else // Assume by default "tracker"
    mask.set(G4PARTICLE_CREATED_TRACKER_HIT);

  //Geant4ParticleHandle(&m_currTrack).dump4(outputLevel(),vol->GetName(),"hit created by particle");
}

/// Event generation action callback
void Geant4ParticleHandler::operator()(G4Event* event)  {
  typedef Geant4MonteCarloTruth _MC;
  debug("+++ Event:%d Add EVENT extension of type Geant4ParticleHandler.....",event->GetEventID());
  context()->event().addExtension((_MC*)this, typeid(_MC), 0);
  clear();
  /// Call the user particle handler
  if ( m_userHandler )  {
    m_userHandler->generate(event, this);
  }
}

/// User stepping callback
void Geant4ParticleHandler::step(const G4Step* step_value, G4SteppingManager* mgr)   {
  typedef vector<const G4Track*> _Sec;
  ++m_currTrack.steps;
  if ( (m_currTrack.reason&G4PARTICLE_ABOVE_ENERGY_THRESHOLD) )  {
    //
    // Tracks below the energy threshold are NOT stored.
    // If these tracks produce hits or are selected due to another signature,
    // this criterium will anyhow take precedence.
    //
    const _Sec* sec=step_value->GetSecondaryInCurrentStep();
    if ( not sec->empty() )  {
      PropertyMask(m_currTrack.reason).set(G4PARTICLE_HAS_SECONDARIES);
    }
  }
  /// Update of the particle using the user handler
  if ( m_userHandler )  {
    m_userHandler->step(step_value, mgr, m_currTrack);
  }
}

/// Pre-track action callback
void Geant4ParticleHandler::begin(const G4Track* track)   {
  Geant4TrackHandler h(track);
  double kine = h.kineticEnergy();
  G4ThreeVector m = h.momentum();
  const G4ThreeVector& v = h.vertex();
  int reason = (kine > m_kinEnergyCut) ? G4PARTICLE_ABOVE_ENERGY_THRESHOLD : 0;
  const G4PrimaryParticle* prim = primary(h.id(),m_primaryMap->primaryMap);
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
    m_currTrack.mask         = prim_part->mask;
    m_currTrack.status       = prim_part->status;
    m_currTrack.spin[0]      = prim_part->spin[0];
    m_currTrack.spin[1]      = prim_part->spin[1];
    m_currTrack.spin[2]      = prim_part->spin[2];
    m_currTrack.colorFlow[0] = prim_part->colorFlow[0];
    m_currTrack.colorFlow[1] = prim_part->colorFlow[1];
    m_currTrack.parents      = prim_part->parents;
    m_currTrack.daughters    = prim_part->daughters;
    m_currTrack.pdgID        = prim_part->pdgID;
    m_currTrack.mass         = prim_part->mass;
  }
  else  {
    m_currTrack.id           = m_globalParticleID;
    m_currTrack.reason       = reason;
    m_currTrack.mask         = 0;
    m_currTrack.status       = G4PARTICLE_SIM_CREATED;
    m_currTrack.spin[0]      = 0;
    m_currTrack.spin[1]      = 0;
    m_currTrack.spin[2]      = 0;
    m_currTrack.colorFlow[0] = 0;
    m_currTrack.colorFlow[1] = 0;
    m_currTrack.parents.clear();
    m_currTrack.daughters.clear();
    m_currTrack.pdgID        = h.trackDef()->GetPDGEncoding();
    m_currTrack.mass         = h.trackDef()->GetPDGMass();
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
  Geant4ParticleHandle ph(&m_currTrack);
  int g4_id = h.id();
  int track_reason = m_currTrack.reason;
  PropertyMask mask(m_currTrack.reason);
  // Update vertex end point and final momentum
  G4ThreeVector m = track->GetMomentum();
  const G4ThreeVector& p = track->GetPosition();
  ph->pex = m.x();
  ph->pey = m.y();
  ph->pez = m.z();
  ph->vex = p.x();
  ph->vey = p.y();
  ph->vez = p.z();


  // Set the simulator status bits
  PropertyMask simStatus(m_currTrack.status);

  // check if the last step ended on the worldVolume boundary
  const G4Step* theLastStep = track->GetStep();
  G4StepPoint* theLastPostStepPoint = NULL;
  if(theLastStep) theLastPostStepPoint = theLastStep->GetPostStepPoint();
  if( theLastPostStepPoint &&
      ( theLastPostStepPoint->GetStepStatus() == fWorldBoundary //particle left world volume
	//|| theLastPostStepPoint->GetStepStatus() == fGeomBoundary
      )
    ) {
    simStatus.set(G4PARTICLE_SIM_LEFT_DETECTOR);
  }

  if(track->GetKineticEnergy() <= 0.) {
    simStatus.set(G4PARTICLE_SIM_STOPPED);
  }

  /// Final update of the particle using the user handler
  if ( m_userHandler )  {
    m_userHandler->end(track, m_currTrack);
  }

  // These are candate tracks with a probability to be stored due to their properties:
  // - primary particle
  // - hits created
  // - secondaries
  // - above energy threshold
  // - to be kept due to creator process
  //
  if ( !mask.isNull() )   {
    m_equivalentTracks[g4_id] = g4_id;
    ParticleMap::iterator ip = m_particleMap.find(g4_id);
    if ( mask.isSet(G4PARTICLE_PRIMARY) )   {
      ph.dump2(outputLevel()-1,name(),"Add Primary",h.id(),ip!=m_particleMap.end());
    }
    // Create a new MC particle from the current track information saved in the pre-tracking action
    Particle* part = 0;
    if ( ip==m_particleMap.end() ) part = m_particleMap[g4_id] = new Particle();
    else part = (*ip).second;
    part->get_data(m_currTrack);
  }
  else   {
    // These are tracks without any special properties.
    //
    // We will not store them on the record, but have to memorise the
    // track identifier in order to restore the history for the created hits.
    int pid = m_currTrack.g4Parent;
    m_equivalentTracks[g4_id] = pid;
    // Need to find the last stored particle and OR this particle's mask
    // with the mask of the last stored particle
    TrackEquivalents::const_iterator iequiv, iend = m_equivalentTracks.end();
    ParticleMap::iterator ip;
    for(ip=m_particleMap.find(pid); ip == m_particleMap.end(); ip=m_particleMap.find(pid))  {
      if ((iequiv=m_equivalentTracks.find(pid)) == iend) break;  // ERROR
      pid = (*iequiv).second;
    }
    if ( ip != m_particleMap.end() )
      (*ip).second->reason |= track_reason;
    else
      ph.dumpWithVertex(outputLevel()+3,name(),"FATAL: No real particle parent present");
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
  for(ParticleMap::const_iterator iend=m_particleMap.end(), i=m_particleMap.begin(); i!=iend; ++i)  {
    Geant4ParticleHandle((*i).second).dump4(INFO,name(),tag);
  }
}

/// Post-event action callback
void Geant4ParticleHandler::endEvent(const G4Event* event)  {
  int count = 0;
  int level = outputLevel();
  do {
    if ( level <= VERBOSE ) dumpMap("Particle");
    debug("+++ Iteration:%d Tracks:%d Equivalents:%d",++count,m_particleMap.size(),m_equivalentTracks.size());
  } while( recombineParents() > 0 );

  if ( level <= VERBOSE ) dumpMap("Recombined");
  // Rebase the simulated tracks, so that they fit to the generator particles
  rebaseSimulatedTracks(0);
  if ( level <= VERBOSE ) dumpMap("Rebased");
  // Consistency check....
  checkConsistency();
  /// Call the user particle handler
  if ( m_userHandler )  {
    m_userHandler->end(event);
  }
  setVertexEndpointBit();

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
  for(TrackEquivalents::iterator ie=m_equivalentTracks.begin(),ie_end=m_equivalentTracks.end(); ie!=ie_end; ++ie)  {
    int g4_equiv = (*ie).first;
    while( (ipar=m_particleMap.find(g4_equiv)) == m_particleMap.end() )  {
      TrackEquivalents::const_iterator iequiv = m_equivalentTracks.find(g4_equiv);
      if ( iequiv == ie_end )  {
        break;  // ERROR !! Will be handled by printout below because ipar==end()
      }
      g4_equiv = (*iequiv).second;
    }
    TrackEquivalents::mapped_type equiv = (*ie).second;
    if ( ipar != m_particleMap.end() )   {
      equivalents[(*ie).first] = (*ipar).second->id;  // requires (1) !
      Geant4ParticleHandle p = (*ipar).second;
      const G4ParticleDefinition* def = p.definition();
      int pdg = int(fabs(def->GetPDGEncoding())+0.1);
      if ( pdg != 0 && pdg<36 && !(pdg > 10 && pdg < 17) && pdg != 22 )  {
        error("+++ ERROR: Geant4 particle for track:%d last known is:%d -- is gluon or quark!",equiv,g4_equiv);
      }
      pdg = int(fabs(p->pdgID)+0.1);
      if ( pdg != 0 && pdg<36 && !(pdg > 10 && pdg < 17) && pdg != 22 )  {
        error("+++ ERROR(2): Geant4 particle for track:%d last known is:%d -- is gluon or quark!",equiv,g4_equiv);
      }
    }
    else   {
      error("+++ No Equivalent particle for track:%d last known is:%d",equiv,g4_equiv);
    }
  }

  // (3) Compute the particle's parents and daughters.
  //     Replace the original Geant4 track with the
  //     equivalent particle still present in the record.
  for(iend=m_particleMap.end(), i=m_particleMap.begin(); i!=iend; ++i)  {
    Particle* p = (*i).second;
    if ( p->g4Parent > 0 )  {
      int equiv_id = equivalents[p->g4Parent];
      if ( (ipar=finalParticles.find(equiv_id)) != finalParticles.end() )  {
        Particle* q = (*ipar).second;
        q->daughters.insert(p->id);
        p->parents.insert(q->id);
      }
      else   {
        error("+++ Inconsistency in particle record: Geant4 parent %d "
              "of particle %d (equiv:%d) not in record!",
              p->g4Parent,p->id,equiv_id);
      }
    }
  }
  m_equivalentTracks = equivalents;
  m_particleMap = finalParticles;
}

/// Default callback to be answered if the particle should be kept if NO user handler is installed
bool Geant4ParticleHandler::defaultKeepParticle(Particle& particle)   {
  PropertyMask mask(particle.reason);
  bool secondaries    =  mask.isSet(G4PARTICLE_HAS_SECONDARIES);
  bool tracker_track  =  mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT);
  bool calo_track     =  mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT);
  bool hits_produced  =  mask.isSet(G4PARTICLE_CREATED_HIT);
  bool low_energy     = !mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD);

  /// Remove this track if it has not created a hit and the energy is below threshold
  if ( mask.isNull() || (secondaries && low_energy && !hits_produced) )  {
    return true;
  }
  /// Remove this track if the energy is below threshold. Reassign hits to parent.
  else if ( !hits_produced && low_energy )  {
    return true;
  }
  /// Remove this track if the origine is in the calorimeter. Reassign hits to parent.
  else if ( !tracker_track && calo_track && low_energy )  {
    return true;
  }
  else  {
    //printout(INFO,name(),"+++ Track: %d should be kept for no obvious reason....",id);
  }
  return false;
}

/// Clean the monte carlo record. Remove all unwanted stuff.
/// This is the core of the object executed at the end of each event action.
int Geant4ParticleHandler::recombineParents()  {
  set<int> remove;

  /// Need to start from BACK, to clean first the latest produced stuff.
  for(ParticleMap::reverse_iterator i=m_particleMap.rbegin(); i!=m_particleMap.rend(); ++i)  {
    Particle* p = (*i).second;
    PropertyMask mask(p->reason);
    // Allow the user to force the particle handling either by
    // or the reason mask with G4PARTICLE_KEEP_USER or
    // to set the reason mask to NULL in order to drop it.
    //
    // If the mask entry is set to G4PARTICLE_FORCE_KILL
    // or is set to NULL, the particle is ALWAYS removed
    //
    // Note: This may override all other decisions!
    bool remove_me = m_userHandler ? m_userHandler->keepParticle(*p) : defaultKeepParticle(*p);

    // Now look at the property mask of the particle
    if ( mask.isNull() || mask.isSet(G4PARTICLE_FORCE_KILL) )  {
      remove_me = true;
    }
    else if ( mask.isSet(G4PARTICLE_KEEP_USER) )  {
      /// If user decides it must be kept, it MUST be kept!
      mask.set(G4PARTICLE_KEEP_USER);
      continue;
    }
    else if ( mask.isSet(G4PARTICLE_PRIMARY) )   {
      /// Primary particles MUST be kept!
      continue;
    }
    else if ( mask.isSet(G4PARTICLE_KEEP_ALWAYS) )   {
      continue;
    }
    else if ( mask.isSet(G4PARTICLE_KEEP_PARENT) )  {
      //continue;
    }
    else if ( mask.isSet(G4PARTICLE_KEEP_PROCESS) )  {
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

    /// Remove this track from the list and also do the cleanup in the parent's children list
    if ( remove_me )  {
      int g4_id = (*i).first;
      ParticleMap::iterator ip = m_particleMap.find(p->g4Parent);
      remove.insert(g4_id);
      m_equivalentTracks[g4_id] = p->g4Parent;
      if ( ip != m_particleMap.end() )   {
        Particle* parent_part = (*ip).second;
        PropertyMask(parent_part->reason).set(mask.value());
        parent_part->steps += p->steps;
        parent_part->secondaries += p->secondaries;
        /// Update of the particle using the user handler
        if ( m_userHandler )  {
          m_userHandler->combine(*p, *parent_part);
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
    Geant4ParticleHandle p((*i).second);
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
    if ( !mask.isSet(G4PARTICLE_PRIMARY) && !status.anySet(G4PARTICLE_GEN_GENERATOR) )  {
      TrackEquivalents::const_iterator eq_it = m_equivalentTracks.find(p->g4Parent);
      bool in_map = false, in_parent_list = false;
      int parent_id = -1;
      if ( eq_it != m_equivalentTracks.end() )   {
        parent_id = (*eq_it).second;
        in_map = (j=m_particleMap.find(parent_id)) != m_particleMap.end();
        in_parent_list = p->parents.find(parent_id) != p->parents.end();
      }
      if ( !in_map || !in_parent_list )  {
        char parent_list[1024];
        parent_list[0] = 0;
        ++num_errors;
        p.dumpWithMomentum(ERROR,name(),"INCONSISTENCY");
        for(set<int>::const_iterator ip=p->parents.begin(); ip!=p->parents.end();++ip)
          ::snprintf(parent_list+strlen(parent_list),sizeof(parent_list)-strlen(parent_list),"%d ",*ip);
        error("+++ Particle:%d Parent %d (G4id:%d)  In record:%s In parent list:%s [%s]",
              p->id,parent_id,p->g4Parent,yes_no(in_map),yes_no(in_parent_list),parent_list);
      }
    }
  }

  if ( num_errors > 0 )  {
    except("+++ Consistency check failed. Found %d problems.",num_errors);
  }
}

void Geant4ParticleHandler::setVertexEndpointBit() {

  ParticleMap& pm = m_particleMap;
  ParticleMap::const_iterator iend, i;
  for(iend=pm.end(), i=pm.begin(); i!=iend; ++i)  {
    Particle* p = (*i).second;

    if( p->parents.empty() ) {
      continue;
    }

    Geant4Particle *parent(pm[ *p->parents.begin() ]);
    const double X( parent->vex - p->vsx );
    const double Y( parent->vey - p->vsy );
    const double Z( parent->vez - p->vsz );
    if( sqrt(X*X + Y*Y + Z*Z) > m_minDistToParentVertex ){
      PropertyMask(p->status).set(G4PARTICLE_SIM_PARENT_RADIATED);
    }
  }

}
