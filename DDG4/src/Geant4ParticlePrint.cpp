//==========================================================================
//  AIDA Detector description implementation 
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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4ParticlePrint.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4HitCollection.h"

// Geant4 include files
#include "G4Event.hh"

// C/C++ include files
#include <cstring>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

typedef detail::ReferenceBitMask<const int> PropertyMask;

/// Standard constructor
Geant4ParticlePrint::Geant4ParticlePrint(Geant4Context* ctxt, const std::string& nam)
  : Geant4EventAction(ctxt,nam)
{
  declareProperty("OutputType",m_outputType=3);
  declareProperty("PrintBegin",m_printBegin=false);
  declareProperty("PrintEnd",  m_printEnd=true);
  declareProperty("PrintGen",  m_printGeneration=true);
  declareProperty("PrintHits", m_printHits=false);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticlePrint::~Geant4ParticlePrint()  {
  InstanceCount::decrement(this);
}

/// Print particle table
void Geant4ParticlePrint::makePrintout(const G4Event* e) const  {
  Geant4ParticleMap* parts = context()->event().extension<Geant4ParticleMap>();
  if ( parts )   {
    const ParticleMap& particles = parts->particles();
    print("+++ ******** MC Particle Printout for event ID:%d ********",e->GetEventID());
    if ( (m_outputType&2) != 0 ) printParticleTree(e,particles);  // Tree printout....
    if ( (m_outputType&1) != 0 ) printParticles(0,particles);     // Table printout....
    return;
  }
  except("No Geant4MonteCarloTruth object present in the event structure to access the particle information!", c_name());
}

/// Generation action callback
void Geant4ParticlePrint::operator()(G4Event* e)  {
  if ( m_printGeneration ) makePrintout(e);
}

/// Pre-event action callback
void Geant4ParticlePrint::begin(const G4Event* e)  {
  if ( m_printBegin ) makePrintout(e);
}

/// Post-event action callback
void Geant4ParticlePrint::end(const G4Event* e)  {
  if ( m_printEnd ) makePrintout(e);
}

void Geant4ParticlePrint::printParticle(const std::string& prefix, const G4Event* e, Geant4ParticleHandle p) const   {
  char equiv[32];
  PropertyMask mask(p->reason);
  PropertyMask status(p->status);
  string proc_name = p.processName();
  string proc_type = p.processTypeName();
  int parent_id = p->parents.empty() ? -1 : *(p->parents.begin());

  equiv[0] = 0;
  if ( p->parents.end() == p->parents.find(p->g4Parent) )  {
    ::snprintf(equiv,sizeof(equiv),"/%d",p->g4Parent);
  }
  print("+++ %s ID:%7d %12s %6d%-7s %7s %3s %5d %3s %+.3e  %-4s %-7s %-3s %-3s %2d  [%s%s%s] %c%c%c%c",
        prefix.c_str(),
        p->id,
        p.particleName().c_str(),
        parent_id,equiv,
        yes_no(mask.isSet(G4PARTICLE_PRIMARY)),
        yes_no(mask.isSet(G4PARTICLE_HAS_SECONDARIES)),
        int(p->daughters.size()),
        yes_no(mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD)),
        p.energy(),
        yes_no(mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT)),
        yes_no(mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT)),
        yes_no(mask.isSet(G4PARTICLE_KEEP_PROCESS)),
        mask.isSet(G4PARTICLE_KEEP_PARENT) ? "YES" : "",
        p.numParent(),
        proc_name.c_str(),
        p->process ? "/" : "",
        proc_type.c_str(),
        status.isSet(G4PARTICLE_GEN_EMPTY) ? 'E' : '.',
        status.isSet(G4PARTICLE_GEN_STABLE) ? 'S' : '.',
        status.isSet(G4PARTICLE_GEN_DECAYED) ? 'D' : '.',
        status.isSet(G4PARTICLE_GEN_DOCUMENTATION) ? 'd' : '.',
        status.isSet(G4PARTICLE_GEN_BEAM) ? 'b' : '.',
        status.isSet(G4PARTICLE_GEN_HARDPROCESS) ? 'H' : '.'
        );
  if ( e && m_printHits )  {
    Geant4ParticleMap* truth = context()->event().extension<Geant4ParticleMap>();
    G4HCofThisEvent* hc = e->GetHCofThisEvent();
    for (int ihc=0, nhc=hc->GetNumberOfCollections(); ihc<nhc; ++ihc)   {
      G4VHitsCollection* c = hc->GetHC(ihc);
      Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(c);
      if ( coll )  {
        size_t nhits = coll->GetSize();
        for(size_t i=0; i<nhits; ++i)   {
          Geant4HitData* h = coll->hit(i);
          Geant4Tracker::Hit* trk_hit = dynamic_cast<Geant4Tracker::Hit*>(h);
          if ( 0 != trk_hit )   {
            Geant4HitData::Contribution& t = trk_hit->truth;
            int trackID = t.trackID;
            int trueID  = truth->particleID(trackID);
            if ( trueID == p->id )   {
              print("+++ %20s           %s[%d]  (%+.2e,%+.2e,%+.2e)[mm]","",c->GetName().c_str(),i,
                    trk_hit->position.x(),trk_hit->position.y(),trk_hit->position.z());
            }
          }
          Geant4Calorimeter::Hit* cal_hit = dynamic_cast<Geant4Calorimeter::Hit*>(h);
          if ( 0 != cal_hit )   {
            Geant4HitData::Contributions& contrib = cal_hit->truth;
            for(Geant4HitData::Contributions::iterator j=contrib.begin(); j!=contrib.end(); ++j)  {
              Geant4HitData::Contribution& t = *j;
              int trackID = t.trackID;
              int trueID  = truth->particleID(trackID);
              if ( trueID == p->id )   {
                print("+++ %20s           %s[%d]  (%+.2e,%+.2e,%+.2e)[mm]","",c->GetName().c_str(),i,
                      cal_hit->position.x(),cal_hit->position.y(),cal_hit->position.z());
              }
            }
          }
        }
      }
      else  {
        print("+++ Hit unknown hit collection type: %s --> %s",
              c->GetName().c_str(),typeName(typeid(*c)).c_str());
      }
    }
  }
}

/// Print record of kept particles
void Geant4ParticlePrint::printParticles(const G4Event* e, const ParticleMap& particles) const  {
  int num_energy = 0;
  int num_parent = 0;
  int num_process = 0;
  int num_primary = 0;
  int num_secondaries = 0;
  int num_calo_hits = 0;
  int num_tracker_hits = 0;

  print("+++ MC Particles #Tracks:%7d ParticleType Parent/Geant4 "
        "Primary Secondary Energy in [MeV] Calo Tracker Process/Par Details",
        int(particles.size()));
  for(ParticleMap::const_iterator i=particles.begin(); i!=particles.end(); ++i)  {
    Geant4ParticleHandle p = (*i).second;
    PropertyMask mask(p->reason);
    printParticle("MC Particle Track",e, p);
    num_secondaries += int(p->daughters.size());
    if ( mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD) ) ++num_energy;
    if ( mask.isSet(G4PARTICLE_PRIMARY) ) ++num_primary;
    if ( mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT) ) ++num_tracker_hits;
    if ( mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT) ) ++num_calo_hits;
    if ( mask.isSet(G4PARTICLE_KEEP_PARENT) ) ++num_parent;
    else if ( mask.isSet(G4PARTICLE_KEEP_PROCESS) ) ++num_process;
  }
  print("+++ MC Particles #Tracks:%7d ParticleType Parent/Geant4 "
        "Primary Secondary Energy          Calo Tracker Process/Par",
        int(particles.size()));
  print("+++ MC Particle Summary:                       %7d %10d %7d  %7d      %9d %5d %6d",
        num_primary, num_secondaries, num_energy,
        num_calo_hits,num_tracker_hits,num_process,num_parent);
}

void Geant4ParticlePrint::printParticleTree(const G4Event* e,
                                            const ParticleMap& particles,
                                            int level,
                                            Geant4ParticleHandle p)  const
{
  char txt[64];
  size_t len = sizeof(txt)-33; // Careful about overruns...
  // Ensure we do not overwrite the array
  if ( level>int(len)-3 ) level = len-3;

  ::snprintf(txt,sizeof(txt),"%5d ",level);
  ::memset(txt+6,' ',len-6);
  txt[len-1] = 0;
  txt[len-2] = '>';
  txt[level+6]='+';
  ::memset(txt+level+6+1,'-',len-level-3-6);

  printParticle(txt, e, p);
  const set<int>& daughters = p->daughters;
  // For all particles, the set of daughters must be contained in the record.
  for(set<int>::const_iterator id=daughters.begin(); id!=daughters.end(); ++id)   {
    int id_dau = *id;
    Geant4ParticleHandle dau = (*particles.find(id_dau)).second;
    printParticleTree(e, particles, level+1, dau);
  }
}

/// Print tree of kept particles
void Geant4ParticlePrint::printParticleTree(const G4Event* e, const ParticleMap& particles)  const  {
  print("+++ MC Particle Parent daughter relationships. [%d particles]",int(particles.size()));
  print("+++ MC Particles %12s #Tracks:%7d %-12s Parent%-7s "
        "Primary Secondary Energy %-8s Calo Tracker Process/Par  Details",
        "",int(particles.size()),"ParticleType","","in [MeV]");
  for(ParticleMap::const_iterator i=particles.begin(); i!=particles.end(); ++i)  {
    Geant4ParticleHandle p = (*i).second;
    PropertyMask mask(p->reason);
    if ( mask.isSet(G4PARTICLE_PRIMARY) ) printParticleTree(e, particles, 0, p);
  }
}
