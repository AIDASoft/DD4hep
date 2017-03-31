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
#ifndef DD4HEP_DDG4_GEANT4HITTRUTHHANDLER_H
#define DD4HEP_DDG4_GEANT4HITTRUTHHANDLER_H

// Framework include files
#include "DDG4/Geant4EventAction.h"

// Forward declarations
class G4VHitsCollection;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

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
    class Geant4HitTruthHandler : public Geant4EventAction {
    public:
      typedef std::vector<std::string> CollectionNames;
      /// Dump single container of hits
      void handleCollection(Geant4ParticleMap* truth, G4VHitsCollection* hc);
      
    public:
      /// Standard constructor
      Geant4HitTruthHandler(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4HitTruthHandler();
      /// Geant4EventAction interface: Begin-of-event callback
      virtual void begin(const G4Event* event)  override;
      /// Geant4EventAction interface: End-of-event callback
      virtual void end(const G4Event* event)  override;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif /* DD4HEP_DDG4_GEANT4HITTRUTHHANDLER_H */

//====================================================================
//  AIDA Detector description implementation for LCD
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
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4HitTruthHandler::Geant4HitTruthHandler(Geant4Context* ctxt, const string& nam)
  : Geant4EventAction(ctxt, nam)
{
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4HitTruthHandler::~Geant4HitTruthHandler() {
  InstanceCount::decrement(this);
}

/// Geant4EventAction interface: Begin-of-event callback
void Geant4HitTruthHandler::begin(const G4Event* /* event */)   {
}

/// Dump single container of hits
void Geant4HitTruthHandler::handleCollection(Geant4ParticleMap* truth, G4VHitsCollection* collection)  {
  Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(collection);
  if ( coll )    {
    size_t nhits = coll->GetSize();
    for(size_t i=0; i<nhits; ++i)   {
      Geant4HitData* h = coll->hit(i);
      Geant4Tracker::Hit* trk_hit = dynamic_cast<Geant4Tracker::Hit*>(h);
      if ( 0 != trk_hit )   {
        if ( truth )  {
          Geant4HitData::Contribution& t = trk_hit->truth;
          int trackID = t.trackID;
          t.trackID = truth->particleID(trackID);
        }
      }
      Geant4Calorimeter::Hit* cal_hit = dynamic_cast<Geant4Calorimeter::Hit*>(h);
      if ( 0 != cal_hit )   {
        if ( truth )  {
          Geant4HitData::Contributions& c = cal_hit->truth;
          for(Geant4HitData::Contributions::iterator j=c.begin(); j!=c.end(); ++j)  {
            Geant4HitData::Contribution& t = *j;
            int trackID = t.trackID;
            t.trackID = truth->particleID(trackID);
          }
        }
      }
    }
  }
}

/// Geant4EventAction interface: End-of-event callback
void Geant4HitTruthHandler::end(const G4Event* event)    {
  G4HCofThisEvent* hce = event->GetHCofThisEvent();
  if ( hce )  {
    int nCol = hce->GetNumberOfCollections();
    Geant4ParticleMap* truth = context()->event().extension<Geant4ParticleMap>(false);
    if ( truth && !truth->isValid() )  {
      truth = 0;
      printout(WARNING,name(),"+++ [Event:%d] No valid MC truth info present. "
               "Is a Particle handler installed ?",event->GetEventID());
    }
    for (int i = 0; i < nCol; ++i) {
      G4VHitsCollection* hc = hce->GetHC(i);
      handleCollection(truth, hc);
    }
    return;
  }
  warning("+++ [Event:%d] The value of G4HCofThisEvent is NULL. No collections saved!",
          event->GetEventID());
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4HitTruthHandler)

