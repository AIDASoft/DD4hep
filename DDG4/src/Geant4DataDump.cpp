// $Id: $
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
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4DataDump.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

typedef detail::ReferenceBitMask<const int> PropertyMask;

/// Default constructor
Geant4DataDump::Geant4DataDump(const std::string& tag) : m_tag(tag) {
}

/// Standard destructor
Geant4DataDump::~Geant4DataDump()  {
}

/// Print a single particle to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, Geant4ParticleHandle p)  const  {
  PropertyMask mask(p->reason);
  int parent = p->parents.empty() ? -1 : *p->parents.begin();
  printout(level, m_tag, "   +++ TrackID: %6d %12d %6d %-7s %3s %5d %6s %8.3g %-4s %-7s %-7s %-3s",
           p->id,
           p->pdgID,
           parent,
           yes_no(mask.isSet(G4PARTICLE_PRIMARY)),
           yes_no(mask.isSet(G4PARTICLE_HAS_SECONDARIES)),
           int(p->daughters.size()),
           yes_no(mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD)),
           p.energy(),
           yes_no(mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT)),
           yes_no(mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT)),
           yes_no(mask.isSet(G4PARTICLE_KEEP_PROCESS)),
           mask.isSet(G4PARTICLE_KEEP_PARENT) ? "YES" : ""
           );
}

/// Print a single particle to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, int id, Geant4ParticleHandle p)  const  {
  PropertyMask mask(p->reason);
  int parent = p->parents.empty() ? -1 : *p->parents.begin();
  printout(level, m_tag, "   +++ TrackID: %6d [key:%d] %12d %6d %-7s %3s %5d %6s %8.3g %-4s %-7s %-7s %-3s",
           p->id, id,
           p->pdgID,
           parent,
           yes_no(mask.isSet(G4PARTICLE_PRIMARY)),
           yes_no(mask.isSet(G4PARTICLE_HAS_SECONDARIES)),
           int(p->daughters.size()),
           yes_no(mask.isSet(G4PARTICLE_ABOVE_ENERGY_THRESHOLD)),
           p.energy(),
           yes_no(mask.isSet(G4PARTICLE_CREATED_CALORIMETER_HIT)),
           yes_no(mask.isSet(G4PARTICLE_CREATED_TRACKER_HIT)),
           yes_no(mask.isSet(G4PARTICLE_KEEP_PROCESS)),
           mask.isSet(G4PARTICLE_KEEP_PARENT) ? "YES" : ""
           );
}

/// Print the particle container to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, const std::string& container, const Particles* parts)  const   {
  if ( parts )    {
    PrintLevel low_lvl = level == ALWAYS ? ALWAYS : PrintLevel(level-1);
    printout(level,m_tag,"+++ Track container: %-21s --------------- Track KEEP reasoning ---------------",container.c_str());
    printout(level,m_tag,"+++ # of Tracks:%6d          PDG Parent Primary Secondary Energy %-8s Calo Tracker Process/Par",
             int(parts->size()),"in [MeV]");
    for(Particle* p : *parts)   {
      Geant4ParticleHandle ph(p);
      print(low_lvl, ph);
    }
  }
}

/// Print the particle map to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, const Geant4ParticleMap* parts)  const {
  if ( parts )    {
    PrintLevel low_lvl = level == ALWAYS ? ALWAYS : PrintLevel(level-1);
    typedef Geant4ParticleMap::ParticleMap ParticleMap;
    const ParticleMap& pm = parts->particles();
    printout(level,m_tag,"+++ Geant4 Particle map %-18s --------------- Track KEEP reasoning ---------------","");
    printout(level,m_tag,"+++ # of Tracks:%6d          PDG Parent Primary Secondary Energy %-8s Calo Tracker Process/Par",
             int(pm.size()),"in [MeV]");
    for(const auto& p : pm)
      print(low_lvl, p.first, p.second);
  }
}

/// Print a single tracker hit to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, const TrackerHit* h)  const   {
  const SimpleHit::Contribution& t = h->truth;
  printout(level,m_tag,"   +++ Hit: Cell: %016llX Pos:(%9.3g,%9.3g,%9.3g) Len:%9.3g [mm] E:%9.3g MeV TrackID:%6d PDG:%12d dep:%9.3g time:%9.3g [ns]",
           h->cellID,h->position.x(),h->position.y(),h->position.z(),h->length,h->energyDeposit,t.trackID,t.pdgID,t.deposit,t.time);
}

/// Print the tracker hits container to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, const std::string& container, const TrackerHits* hits)  const  {
  if ( hits )    {
    PrintLevel low_lvl = level == ALWAYS ? ALWAYS : PrintLevel(level-1);
    printout(level,m_tag,"+++ Hit Collection: %s  # Tracker hits %d",container.c_str(),int(hits->size()));
    for(const TrackerHit* hit : *hits)
      print(low_lvl, hit);
  }
}

/// Print a calorimeter tracker hit to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, const CalorimeterHit* h)  const   {
  PrintLevel low_lvl = level == ALWAYS ? ALWAYS : PrintLevel(level-1);
  printout(level,m_tag,"   +++ Hit: Cell: %016llX Pos:(%9.3g,%9.3g,%9.3g) [mm] E:%9.3g MeV #Contributions:%3d",
           h->cellID,h->position.x(),h->position.y(),h->position.z(),h->energyDeposit,h->truth.size());
  int cnt=0;
  for(const SimpleHit::Contribution& c : h->truth)  {
    printout(low_lvl,m_tag,"       Contribution #%3d TrackID:%6d PDG:%12d %9.3g MeV %9.3g ns",
             cnt,c.trackID,c.pdgID,c.deposit,c.time);
    ++cnt;
  }
}

/// Print the calorimeter hits container to the output logging using the specified print level
void Geant4DataDump::print(PrintLevel level, const std::string& container, const CalorimeterHits* hits)  const   {
  if ( hits )    {
    PrintLevel low_lvl = level == ALWAYS ? ALWAYS : PrintLevel(level-1);
    printout(level,m_tag,"+++ Hit Collection: %s  # Calorimeter hits %d",container.c_str(),int(hits->size()));
    for(const CalorimeterHit* hit : *hits)
      print(low_lvl, hit);
    return;
  }
}
