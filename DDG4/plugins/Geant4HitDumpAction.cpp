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
#ifndef DD4HEP_DDG4_GEANT4HITDUMPACTION_H
#define DD4HEP_DDG4_GEANT4HITDUMPACTION_H

// Framework include files
#include "DDG4/Geant4EventAction.h"

// Forward declarations
class G4VHitsCollection;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4ParticleMap;
    
    /// Class to measure the energy of escaping tracks
    /** Class to measure the energy of escaping tracks of a detector using Geant 4
     * Measure escaping energy....
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HitDumpAction : public Geant4EventAction {
    public:
      typedef std::vector<std::string> CollectionNames;
      /// Property: collection names to be dumped 
      CollectionNames m_containers;

      /// Dump single container of hits
      void dumpCollection(G4VHitsCollection* hc);
      
    public:
      /// Standard constructor
      Geant4HitDumpAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4HitDumpAction();
      /// Geant4EventAction interface: Begin-of-event callback
      virtual void begin(const G4Event* event)  override;
      /// Geant4EventAction interface: End-of-event callback
      virtual void end(const G4Event* event)  override;
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif /* DD4HEP_DDG4_GEANT4HITDUMPACTION_H */

//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4HitCollection.h"

// Geant 4 includes
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim;

/// Standard constructor
Geant4HitDumpAction::Geant4HitDumpAction(Geant4Context* ctxt, const string& nam)
  : Geant4EventAction(ctxt, nam), m_containers{"*"}
{
  m_needsControl = true;
  declareProperty("Collections",m_containers);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4HitDumpAction::~Geant4HitDumpAction() {
  InstanceCount::decrement(this);
}

/// Geant4EventAction interface: Begin-of-event callback
void Geant4HitDumpAction::begin(const G4Event* /* event */)   {
}

/// Dump single container of hits
void Geant4HitDumpAction::dumpCollection(G4VHitsCollection* collection)  {
  Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(collection);
  string nam = collection->GetName();
  if ( coll )    {
    Geant4DataDump::CalorimeterHits cal_hits;
    Geant4DataDump::TrackerHits     trk_hits;
    Geant4DataDump dump(name());
    size_t nhits = coll->GetSize();
    for(size_t i=0; i<nhits; ++i)   {
      Geant4HitData* h = coll->hit(i);
      Geant4Tracker::Hit* trk_hit = dynamic_cast<Geant4Tracker::Hit*>(h);
      if ( 0 != trk_hit )   {
        trk_hits.push_back(trk_hit);
      }
      Geant4Calorimeter::Hit* cal_hit = dynamic_cast<Geant4Calorimeter::Hit*>(h);
      if ( 0 != cal_hit )   {
        cal_hits.push_back(cal_hit);
      }
    }
    if ( !trk_hits.empty() )
      dump.print(ALWAYS,nam,&trk_hits);
    if ( !cal_hits.empty() )
      dump.print(ALWAYS,nam,&cal_hits);
  }
}

/// Geant4EventAction interface: End-of-event callback
void Geant4HitDumpAction::end(const G4Event* event)    {
  G4HCofThisEvent* hce = event->GetHCofThisEvent();
  if ( hce )  {
    int nCol = hce->GetNumberOfCollections();
    bool all = !m_containers.empty() && m_containers[0] == "*";
    for (int i = 0; i < nCol; ++i) {
      G4VHitsCollection* hc = hce->GetHC(i);
      if ( all || find(m_containers.begin(),m_containers.end(),hc->GetName()) != m_containers.end() )
        dumpCollection(hc);
    }
    return;
  }
  warning("+++ [Event:%d] The value of G4HCofThisEvent is NULL.",event->GetEventID());
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4HitDumpAction)
