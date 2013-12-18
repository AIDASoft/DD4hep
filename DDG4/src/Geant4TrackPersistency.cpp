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
#include "DDG4/Geant4TrackPersistency.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4MonteCarloRecordManager.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4StepHandler.h"

#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4TrackStatus.hh"
#include <set>
#include <stdexcept>

using namespace std;
using DD4hep::InstanceCount;
using namespace DD4hep;
using namespace DD4hep::Simulation;



#include <map>
namespace DD4hep {
  namespace Simulation {

    struct Track;
    struct Vertex;
    typedef std::vector<int>      TrackIDs;
    typedef std::vector<Track*>   Tracks;
    typedef std::vector<Vertex*>  Vertices;
    typedef Geant4TrackPersistency::FourMomentum FourMomentum;

    struct Vertex  {
      G4ThreeVector position;
      Tracks        outgoing;
      Track*        incoming;
      int           refCount;
      Vertex() : incoming(0), refCount(0) {
	InstanceCount::increment(this);
      }
      virtual ~Vertex() {
	InstanceCount::decrement(this);
      }
      void addRef()  {
	++refCount;
      }
      int release()  {
	int tmp = --refCount;
	if ( tmp < 1 )  {
	  delete this;
	}
	return tmp;
      }
    };

    typedef std::map<int,void*> TrackMap;
    typedef std::map<const void*,Track*> TrackMap2;

    struct Track  {
      enum State { 
	KEEP=0x1,
	CREATED_HIT=0x2,
	VALID_G4TRACK=0x4,
	TRACKED_G4TRACK=0x8
      };

      TrackMap2                   secondaries;
      TrackIDs                    contained;
      FourMomentum                momentum;
      Vertices                    daughters;
      Vertex*                     vertex;
      Track*                      parent;
      int                         id;
      int                         refCount;
      int                         flag;
      const G4VProcess*           process;
      const G4ParticleDefinition* def;

      /// Standard constructor
      Track() : vertex(0), parent(0), id(0), refCount(0), flag(0), process(0), def(0)  
      {
	InstanceCount::increment(this);
      }
      virtual ~Track() 
      {
	InstanceCount::decrement(this);
      }
      bool toBeKept()  const          {   return (flag&KEEP) == KEEP; }
      bool createdHit()  const        {   return (flag&CREATED_HIT) == CREATED_HIT; }
      bool validG4Track() const       {   return (flag&VALID_G4TRACK) == VALID_G4TRACK; }
      bool wasTracked() const         {   return (flag&TRACKED_G4TRACK) == TRACKED_G4TRACK; }
    };
  }
}


void collectContained(const Track* trk, std::map<int,int>& equiv)   {
  if ( !trk->contained.empty() )  {
    for(TrackIDs::const_iterator i=trk->contained.begin(); i!=trk->contained.end(); ++i)
      equiv[*i] = trk->id;
  }
  for(TrackMap2::const_iterator j=trk->secondaries.begin(); j!=trk->secondaries.end(); ++j)
    collectContained((*j).second,equiv);
}

void printTrack(int flg, const char* msg);
void printTree(const Track* trk,const char* msg, int cnt);
void printProperties(const Track* trk);
void printSecondaries(const Track* trk);
void printTrack(const Track* trk, int flg=0, const char* msg="Interesting");
/// Release track object
void releaseTrack(Geant4TrackPersistency* p, Track* trk);
bool cleanTree(Geant4TrackPersistency* p, const string& proc, Track* trk);

bool keepTrackTree(const string& proc, Track* trk)     {
  // We have to check if any of the daughters has the KEEP flag ON.
  // If YES, this track may not be deleted, otherwise, we can release the track
  string pnam = trk->process ? trk->process->GetProcessName().c_str() : "---";
  if ( pnam == proc ) trk->flag &= ~(Track::KEEP);
  if ( (trk->flag&Track::KEEP) == Track::KEEP ) return true;
  for(TrackMap2::const_iterator j=trk->secondaries.begin(); j!=trk->secondaries.end(); ++j)
    if ( keepTrackTree(proc,(*j).second) ) return true;
  return false;
}

bool cleanTree(Geant4TrackPersistency* p, const string& proc, Track* trk)     {
  bool remove = false;
  for(TrackMap2::iterator j=trk->secondaries.begin(); j!=trk->secondaries.end(); ++j)
    remove |= cleanTree(p,proc,(*j).second);

  if ( remove )  {
    string pnam = trk->process ? trk->process->GetProcessName().c_str() : "---";
    if ( remove || pnam == proc )   {
      bool keep = keepTrackTree(proc,trk);
      if ( !keep )   {
	releaseTrack(p,trk);
	return true;
      }
    }
  }
  return false;
}

void printTree(const Track* trk,const char* msg, int cnt)     {
  char text[64];
  ::sprintf(text," %-8d |  ",cnt);
  printf(" %-6d ",cnt);
  ::sprintf(text,"|  ");
  std::string m(text);
  m += msg;
  printTrack(trk, 1, msg);
  int c = 0;
  for(TrackMap2::const_iterator j=trk->secondaries.begin(); j!=trk->secondaries.end(); ++j)
    printTree((*j).second,m.c_str(),++c);
}

void printProperties(const Track* trk)     {
  const char* pnam = trk->process ? trk->process->GetProcessName().c_str() : "---";
  int pid  = trk->parent ? trk->parent->id : 0;
  int sec  = int(trk->secondaries.size());
  int vtx  = 0;
  for(Vertices::const_iterator i=trk->daughters.begin(); i!=trk->daughters.end();++i)
    vtx += (*i)->outgoing.size();
  if ( vtx != sec )  {
    ::printf(" ++++ ERROR! ++++ ");
  }
  ::printf("Track %4d Parent:%4d %-8s %-6s Sec:%3d [%3d] Cont:%3d Keep:%s Tracked:%s Hit:%s",
	   trk->id, pid, pnam, trk->def->GetParticleName().c_str(), sec, vtx, 
	   int(trk->contained.size()),
	   yes_no(trk->toBeKept()), yes_no(trk->wasTracked()),
	   yes_no(trk->createdHit()));
}

void printContained(const Track* trk)  {
  const TrackIDs& ids = trk->contained;
  if ( !ids.empty() )  {
    ::printf(" Equiv(%d):",int(ids.size()));
    for(TrackIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) ::printf(" %d",*i);
  }
}
 
void printSecondaries(const Track* trk)  {
 char text[256];
  int ndau = 1;
  for(TrackMap2::const_iterator j=trk->secondaries.begin(); j!=trk->secondaries.end(); ++j, ++ndau)  {
    ::sprintf(text,"  ---> Daughter [%3d]",ndau);
    ::printTrack((*j).second,2,text);
  }
}
void printParents(const Track* trk)   {
  char text[256];
  int npar = 1;
  for(Track* p=trk->parent; p; p=p->parent, ++npar)   {
    ::sprintf(text,"  ---> Parent [%3d]",npar);
    printTrack(p,4,text);
  }
}

void printTrack(const Track* trk, int flg, const char* msg)  {
  printf("%s> ",msg);
  printProperties(trk);
  if ( (flg&1)==1 ) printContained(trk);
  printf("\n");
  if ( (flg&2)==2 ) printSecondaries(trk);
  if ( (flg&4)==4 ) printParents(trk);
}

void referenceTrack(Track* trk)  {
  if ( trk )  {
    ++trk->refCount;
    referenceTrack(trk->parent);
  }
}

/// Release track object
void releaseTrack(Geant4TrackPersistency* p, Track* trk)   {
  int cnt = --trk->refCount;
  Track* par = trk->parent;
  if ( cnt < 1 )  {
    int id = trk->id;
    Vertex* v = trk->vertex;
    if ( v )  {
      if ( par )   {
	Tracks& trks = v->outgoing;
	for(Tracks::iterator it=trks.begin(); it != trks.end(); ++it)  {
	  if ( *it == trk ) { trks.erase(it); break; }
	}
	if ( trks.size() == 0 )   {
	  Vertices::iterator iv=find(par->daughters.begin(),par->daughters.end(),v);
	  if ( iv != par->daughters.end() ) {
	    par->daughters.erase(iv);
	  }
	}
	for(TrackMap2::iterator is=par->secondaries.begin(); is!=par->secondaries.end(); ++is)  {
	  if ( (*is).second == trk ) {
	    par->secondaries.erase(is);
	    break;
	  }
	}
	par->contained.push_back(id);
      }
      v->release();
    }
    std::for_each(trk->daughters.begin(),trk->daughters.end(),releasePtr<Vertex>);
    delete trk;

    TrackMap::iterator m = p->m_trackMap.find(id);
    if ( m != p->m_trackMap.end() ) p->m_trackMap.erase(m);
    //printf(" ---> DT: REMOVE track %p [%s] %d  size:%d\n", (void*)trk, pn, id, int(p->m_trackMap.size()));
  }
  if ( par )  {
    releaseTrack(p, par);
  }
}

/// Initialize track data
Track* initTrack(const G4Track* track, Track* trk, Track* par)   {
  G4ThreeVector mom = track->GetMomentum();
  trk->momentum.SetPxPyPzE(mom.x(),mom.y(),mom.z(),track->GetTotalEnergy());
  trk->process = track->GetCreatorProcess();
  trk->def = track->GetParticleDefinition();
  trk->id = track->GetTrackID();
  trk->parent = par;
  if ( par )  {
    par->secondaries[track] = trk;
  }
  referenceTrack(trk);
  return trk;
}

void deleteTrackTree(Track* trk)   {
  Vertex* v = trk->vertex;
  if ( v )  {
    //if ( v->refCount == 2 ) --v->refCount;
    v->release();
  }
  for(TrackMap2::iterator j=trk->secondaries.begin(); j!=trk->secondaries.end(); ++j)
    deleteTrackTree((*j).second);
  delete trk;
}

/// Standard constructor
Geant4TrackPersistency::TrackInfo::TrackInfo() : G4VUserTrackInformation() {
  this->manager = 0;
  this->track   = 0;
  this->store   = false;
  this->createdHit = false;
  this->initialEnergy = 0;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TrackPersistency::TrackInfo::~TrackInfo()   {
  InstanceCount::decrement(this);
}

/// Clear alkl stored information for this track
void Geant4TrackPersistency::TrackInfo::set(const G4Track* trk, void* inf)   {
  this->track = trk;
  if ( trk )   {
    G4ThreeVector mom = trk->GetMomentum();
    this->initialMomentum.SetPxPyPzE(mom.x(),mom.y(),mom.z(),trk->GetTotalEnergy());
    this->initialEnergy = trk->GetTotalEnergy();
    this->info = inf;
  }
  else  {
    this->initialMomentum.SetPxPyPzE(0e0,0e0,0e0,0e0);
    this->initialEnergy = 0;
    this->createdHit = false;
    this->store      = false;
    this->info       = 0;
  }
}

/// Standard constructor
Geant4TrackPersistency::Geant4TrackPersistency(Geant4Context* context, const std::string& nam)
  : Geant4Action(context,nam), Geant4MonteCarloTruth(), m_current()
{
  m_current.manager = this;
  eventAction().callAtBegin(this,&Geant4TrackPersistency::beginEvent);
  eventAction().callAtFinal(this,&Geant4TrackPersistency::endEvent);
  trackingAction().callAtFinal(this,&Geant4TrackPersistency::end,CallbackSequence::FRONT);
  trackingAction().callUpFront(this,&Geant4TrackPersistency::begin,CallbackSequence::FRONT);
  steppingAction().call(this,&Geant4TrackPersistency::step);

  declareProperty("PrintEndTracking",m_printEndTracking = false);
  declareProperty("PrintStartTracking",m_printStartTracking = false);

  InstanceCount::increment(this);
}

/// Default destructor
Geant4TrackPersistency::~Geant4TrackPersistency()  {
  InstanceCount::decrement(this);
}

/// Mark a Geant4 track to be kept for later MC truth analysis
void Geant4TrackPersistency::mark(const G4Track* track, bool created_hit)   {
  mark(track);  // Does all the checking...
  if ( created_hit )  {
    Track* trk = (Track*)m_current.info;
    trk->flag |= Track::CREATED_HIT;
    m_current.createdHit = created_hit;
  }
}

/// Store a track produced in a step to be kept for later MC truth analysis
void Geant4TrackPersistency::mark(const G4Step* step, bool created_hit)   {
  if ( step )  {
    mark(step->GetTrack(),created_hit);
    return;
  }
  except("Cannot mark the G4Track if the step-pointer is invalid!", c_name());
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4TrackPersistency::mark(const G4Step* step)   {
  if ( step )  {
    mark(step->GetTrack());
    return;
  }
  except("Cannot mark the G4Track if the step-pointer is invalid!", c_name());
}

/// Mark a Geant4 track of the step to be kept for later MC truth analysis
void Geant4TrackPersistency::mark(const G4Track* track)   {
  if ( 0 == m_current.track )  {
    except("Miconfigured program. The tracking preaction was never called for the Geant4TrackPersistency");
  }
  else if ( track == m_current.track )  {
    m_current.store = true;
    Track* trk = (Track*)m_current.info;
    trk->flag |=  Track::KEEP;
    referenceTrack(trk);
    return;
  }
  except("Call to Geant4TrackPersistency with inconsistent G4Track pointer: %p <> %p",
	 track,m_current.track);
}

/// Pre-event action callback
void Geant4TrackPersistency::beginEvent(const G4Event* )  {
  m_trackMap.clear();
}

/// Pre-event action callback
void Geant4TrackPersistency::endEvent(const G4Event* )  {
  int cnt = 1;
  printf("---------------------------- End of Event: %d tracks kept ----------------------------\n",
	 int(m_trackMap.size()));
  for (TrackMap::iterator m=m_trackMap.begin(); m != m_trackMap.end(); ++m, ++cnt)  {
    Track* trk = (Track*)(*m).second;
    if ( trk->parent == 0 )  {
      printTree(trk,"+--",1);
      cleanTree(this,"eBrem",trk);
      break;
    }
  }

  printf("---------------------------- End of Event: %d tracks kept ----------------------------\n",
	 int(m_trackMap.size()));
  for(TrackMap::iterator m=m_trackMap.begin(); m != m_trackMap.end(); ++m, ++cnt)  {
    Track* trk = (Track*)(*m).second;
    if ( trk->parent == 0 )  {
      printTree(trk,"+--",1);
      cleanTree(this,"eIoni",trk);
      break;
    }
  }

  printf("---------------------------- End of Event: %d tracks kept ----------------------------\n",
	 int(m_trackMap.size()));
  for(TrackMap::iterator m=m_trackMap.begin(); m != m_trackMap.end(); ++m, ++cnt)  {
    Track* trk = (Track*)(*m).second;
    if ( trk->parent == 0 )  {
      printTree(trk,"+--",1);
      deleteTrackTree(trk);
      break;
    }

    //char text[64];
    //::sprintf(text,"Intersting [%3d]",cnt);
    //trk->print(7,text);
  }
  m_trackMap.clear();
}

/// User stepping callback
void Geant4TrackPersistency::step(const G4Step* step, G4SteppingManager* /* mgr */)   {
  typedef std::vector<const G4Track*> _Sec;
  const _Sec* sec=step->GetSecondaryInCurrentStep();
  if ( sec->size() > 0 )  {
    Track*  t = (Track*)m_current.info;
    Vertex* v = new Vertex();
    v->incoming = t;
    t->daughters.push_back(v);

    for(_Sec::const_iterator i=sec->begin(); i!=sec->end(); ++i)  {
      Track* secondary = initTrack(*i,new Track(),t);
      secondary->vertex = v;
      secondary->vertex->addRef();
      v->outgoing.push_back(secondary);
    }
  }
}

/// Pre-track action callback
void Geant4TrackPersistency::begin(const G4Track* track)   {
  G4VUserTrackInformation* info = &m_current;
  int pid = track->GetParentID();
  Track* trk = 0;

  if ( pid == 0 )  {
    trk = (Track*)initTrack(track,new Track(),0);
    trk->vertex = new Vertex();
    trk->vertex->incoming  = 0;
    trk->vertex->position  = track->GetPosition();
    m_trackMap.insert(make_pair(trk->id,trk));
  }
  else   {
    Track* par = (Track*)m_trackMap[pid];
    trk = par->secondaries[track];
    trk->id = track->GetTrackID();
    trk->vertex->incoming = par;
    trk->vertex->position = track->GetPosition();
    m_trackMap.insert(make_pair(trk->id,trk));
  }

  if ( m_printStartTracking )  {
    Geant4TrackHandler t(track);
    std::string   proc = t.creatorName();
    G4ThreeVector pos  = t.position();

    this->info("START Track %p ID=%4d Parent:%4d  E:%7.2f MeV  "
	       "%-6s %-8s L:%7.2f Pos:(%8.2f %8.2f %8.2f)  Ref:%d",track,
	       t.id(), t.parent(), t.energy(),
	       t.name().c_str(), ("["+proc+"]").c_str(), t.length(), 
	       pos.x(),pos.y(),pos.z(),trk->refCount);
  }

  trk->flag |= Track::VALID_G4TRACK;
  m_current.set(track,trk);
  trackMgr()->SetUserTrackInformation(info);

  Geant4TrackHandler t(track);
  std::string   proc = t.creatorName();
  if ( proc == "conv" )   {
    mark(track);
  }
  if ( proc == "eBrem" )   {
    mark(track);
  }
  else if ( trk->id < 5 ) 
    mark(track);
}

/// Post-track action callback
void Geant4TrackPersistency::end(const G4Track* track)   {
  Track* trk = (Track*)m_current.info;

  trk->flag |=   Track::TRACKED_G4TRACK;
  trk->flag &= ~(Track::VALID_G4TRACK);
  if ( m_printEndTracking )  {
    Geant4TrackHandler t(track);
    std::string   proc = t.creatorName();
    G4ThreeVector pos  = t.position();
    
    this->info("END   Track %p ID=%4d Parent:%4d  E:%7.2f MeV  "
	       "%-6s %-8s L:%7.2f Pos:(%8.2f %8.2f %8.2f) Ref:%d Keep:%s Tracked:%s Hit:%s",
	       track,t.id(), t.parent(), t.energy(),
	       t.name().c_str(), ("["+proc+"]").c_str(), t.length(), 
	       pos.x(),pos.y(),pos.z(),trk->refCount,
	       yes_no(trk->toBeKept()), yes_no(trk->wasTracked()),
	       yes_no(trk->createdHit()));
  }
  /// If required save Track record...
  if ( m_current.store )   {
    mcRecordMgr().save(m_current);
  }
  m_current.set(0,0);
  trackMgr()->SetUserTrackInformation(0);
  // All done. Check if the track can be released/deleted
  releaseTrack(this,trk);
}
