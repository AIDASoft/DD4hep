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

enum { CREATED_HIT=1<<0, 
       PRIMARY=1<<1,
       LAST=1<<63
};

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

  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticleHandler::~Geant4ParticleHandler()  {
  InstanceCount::decrement(this);
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
  m_trackReasons[track->GetTrackID()] = CREATED_HIT;
}

class Particle {
public:
  int id;
  double px,py,pz;
  const G4ParticleDefinition *definition;
};

/// Event generation action callback
void Geant4ParticleHandler::operator()(G4Event* evt)  {
  typedef Geant4MonteCarloTruth _MC;
  printout(INFO,name(),"+++ Add EVENT extension of type Geant4ParticleHandler.....");
  context()->event().addExtension((_MC*)this, typeid(_MC), 0);
  m_trackReasons.clear();
  m_trackIDTree.clear();
  m_particleMap.clear();
  for(int i=0, count=-1; i<evt->GetNumberOfPrimaryVertex(); ++i)  {
    G4PrimaryVertex* v = evt->GetPrimaryVertex(i);
    for(int j=0; j<v->GetNumberOfParticle(); ++j)  {
      G4PrimaryParticle* p = v->GetPrimary(i);
      const G4ParticleDefinition* def = p->GetParticleDefinition();
      Particle* particle = new Particle();
      p->SetTrackID(++count);
      particle->px = p->GetPx();
      particle->py = p->GetPy();
      particle->pz = p->GetPz();
      particle->id = p->GetTrackID();
      particle->definition = def;
      printout(INFO,name(),"+++ Add Primary particle %d def:%p [%s, %s]",
	       particle->id, def, 
	       def ? def->GetParticleName().c_str() : "UNKNOWN",
	       def ? def->GetParticleType().c_str() : "UNKNOWN");
      m_particleMap.insert(make_pair(particle->id,particle));
      TrackIDTree::iterator i = m_trackIDTree.find(p->GetTrackID());
      if ( i == m_trackIDTree.end() ) m_trackIDTree.insert(make_pair(p->GetTrackID(),std::set<int>()));
      m_trackReasons[p->GetTrackID()] = PRIMARY;
    }
  }
}

/// Pre-event action callback
void Geant4ParticleHandler::beginEvent(const G4Event* )  {
}

/// Pre-event action callback
void Geant4ParticleHandler::endEvent(const G4Event* )  {
  m_particleMap.clear();
  m_trackIDTree.clear();
  m_trackReasons.clear();
}

/// User stepping callback
void Geant4ParticleHandler::step(const G4Step* step, G4SteppingManager* /* mgr */)   {
  typedef std::vector<const G4Track*> _Sec;
  const _Sec* sec=step->GetSecondaryInCurrentStep();
  if ( sec->size() > 0 )  {
    for(_Sec::const_iterator i=sec->begin(); i!=sec->end(); ++i)  {
    }
  }
}

/// Pre-track action callback
void Geant4ParticleHandler::begin(const G4Track* track)   {
  Geant4TrackHandler h(track);

  TrackIDTree::iterator i = m_trackIDTree.find(h.parent());
  printout(INFO,name(),"+++ Tracking preaction: %d par:%d [%s, %s] created by:%s",
	   h.id(),h.parent(),h.name().c_str(),h.type().c_str(),h.creatorName().c_str());
  m_trackIDTree.insert(make_pair(h.id(),std::set<int>()));
  m_trackReasons[h.id()] = 0;
  if ( i != m_trackIDTree.end() )  {
    (*i).second.insert(h.id());
    return;
  }
  printout(INFO,name(),"+++ Tracking preaction: UNKNOWN parent %d par:%d",h.id(),h.parent());
}

/// Post-track action callback
void Geant4ParticleHandler::end(const G4Track* track)   {
  int id = track->GetTrackID();
  int pid = track->GetParentID();
  printout(INFO,name(),"+++ Tracking postaction: %d par:%d",id,pid);
}
