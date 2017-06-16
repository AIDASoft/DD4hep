//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author Markus Frank
//  \date   2015-10-14
//
//==========================================================================
#ifndef DD4HEP_DDG4_GEANT4SURFACETEST_H
#define DD4HEP_DDG4_GEANT4SURFACETEST_H

// Framework include files
//#include "Geant4SurfaceTest.h"
#include "DDG4/Geant4EventAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /** @class Geant4SurfaceTest Geant4SurfaceTest.h reco/Geant4SurfaceTest.h
     */
    class Geant4SurfaceTest : public Geant4EventAction     {
    public: 
      /// Standard constructor
      Geant4SurfaceTest(Geant4Context* context, const std::string& nam); 
      /// Destructor
      virtual ~Geant4SurfaceTest( );

      /// Begin-of-event callback
      virtual void begin(const G4Event* event);
      /// End-of-event callback
      virtual void end(const G4Event* event);

    protected:

    private:

    };
  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4SURFACETEST_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Factories.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4HitCollection.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorSurfaces.h"
#include "DDRec/SurfaceManager.h"
#include "DDRec/SurfaceHelper.h"

// Geant 4 includes
#include "G4VHitsCollection.hh"
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

// C/C++ include files
#include <stdexcept>
#include <map>
#include <sstream>

using namespace std;
using namespace dd4hep::DDRec;
using namespace dd4hep::detail;
using namespace dd4hep::sim;

DECLARE_GEANT4ACTION(Geant4SurfaceTest)

/// Standard constructor, initializes variables
Geant4SurfaceTest::Geant4SurfaceTest(Geant4Context* ctxt, const std::string& nam)
: Geant4EventAction(ctxt, nam)
{
  InstanceCount::increment(this);
}

/// Default Destructor
Geant4SurfaceTest::~Geant4SurfaceTest() {
  InstanceCount::decrement(this);
} 

/// Begin-of-event callback
void Geant4SurfaceTest::begin(const G4Event* event)  {
  if ( event )  {
  }
}

/// End-of-event callback
void Geant4SurfaceTest::end(const G4Event* evt)  {
  stringstream sst;
  Detector& description = context()->detectorDescription();
  SurfaceManager& surfMan = *description.extension< SurfaceManager >() ;
  const SurfaceMap& surfMap = *surfMan.map( "world" ) ;
  G4HCofThisEvent*  hce = evt->GetHCofThisEvent();

  if ( !hce )  {
    error("Event:%d: No Geant4 event record (HCE) found.",evt->GetEventID());
    return;
  }

  for(int j=0, nCol=hce->GetNumberOfCollections(); j<nCol; ++j)   {
    G4VHitsCollection* hc = hce->GetHC(j);
    Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(hc);

    if ( !(coll && coll->type().type == typeid(Geant4Tracker::Hit)) )   {
      continue;
    }

    SensitiveDetector sd = coll->sensitive()->sensitiveDetector();
    if ( !sd.isValid() )  {
      error("Event:%d: Collection:%s Invalid sensitive detector.",
            evt->GetEventID(),hc->GetName().c_str());
      continue;
    }
    IDDescriptor idDesc    = sd.readout().idSpec();
    const string idStr     = idDesc.fieldDescription();
    BitField64   idDecoder(idStr);
    info("Event: %d [%s: %d hits] Desc:%s",
         evt->GetEventID(),hc->GetName().c_str(),coll->GetSize(),idStr.c_str());
    for(size_t i=0, nHit=coll->GetSize(); i<nHit; ++i)    {
      Geant4Tracker::Hit* h = coll->hit(i);
      const Position& pos = h->position;
      int trackID = h->truth.trackID;
      idDecoder.setValue( h->cellID ) ;
      SurfaceMap::const_iterator si = surfMap.find(h->cellID);
      ISurface* surf = (si != surfMap.end() ?  si->second  : 0);
      
      // ===== test that we have a surface with the correct ID for every hit ======================
      if ( !surf )   {
        error("FAILED: Hit:%ld Track:%d No surface found cell id: %016llX",i,trackID,h->cellID);
        continue;
      }
      info("PASSED: Hit:%ld Track:%d Surface found for cell: %016llX",i,trackID,h->cellID);

      Vector3D hit_point(pos.x()*dd4hep::mm,pos.y()*dd4hep::mm,pos.z()*dd4hep::mm);	  
      bool isInside = surf->insideBounds(hit_point);
      string flag = "";
      if ( h->flag & Geant4Tracker::Hit::HIT_SECONDARY_TRACK ) flag += " TRK:SECONDARY";
      if ( h->flag & Geant4Tracker::Hit::HIT_KILLED_TRACK    ) flag += " TRK:KILLED";
      if ( h->flag & Geant4Tracker::Hit::HIT_NEW_TRACK       ) flag += " NEW_TRACK";
      if ( h->flag & Geant4Tracker::Hit::HIT_STARTED_OUTSIDE ) flag += " START:OUTSIDE";
      if ( h->flag & Geant4Tracker::Hit::HIT_STARTED_INSIDE  ) flag += " START:INSIDE";
      if ( h->flag & Geant4Tracker::Hit::HIT_STARTED_SURFACE ) flag += " START:SURFACE";
      if ( h->flag & Geant4Tracker::Hit::HIT_ENDED_OUTSIDE   ) flag += " END:OUTSIDE";
      if ( h->flag & Geant4Tracker::Hit::HIT_ENDED_INSIDE    ) flag += " END:INSIDE";
      if ( h->flag & Geant4Tracker::Hit::HIT_ENDED_SURFACE   ) flag += " END:SURFACE";

      sst.str("");
      if ( isInside )  {
        sst << hc->GetName() << " " << "PASSED: Track:" << trackID << " Hit:" << i
          //<< " Point " << hit_point 
            << " is ON SURFACE. "
            << "(FLAG:" << h->flag << flag << ").";
        print(sst.str().c_str());
      }
      else   {
        double dist = surf->distance(hit_point)/dd4hep::mm;
        sst.str("");
        sst << hc->GetName() << " FAILED: Track:" << trackID << " Hit:" << i 
            << " Point: " << hit_point;
        error(sst.str().c_str());
        //sst.str("");
        //sst <<   -> Surface:" <<  *surf;
        //error(sst.str().c_str());
        sst.str("");
        sst << "   -> Hit is NOT ON SURFACE (Flag:" << h->flag << flag << ")"
            << " Distance to surface:" << dist << " mm.";
        error(sst.str().c_str());
        // No need for further tests, if this one failed....
        continue;
      }
      // ====== Test to show that moved hit points are outside their surface
      hit_point = hit_point + 1e-3 * surf->normal() ;
      sst.str("") ;
      sst << "Track:" << trackID << " Hit:" << i << " Moved:" << hit_point;
      if ( (isInside=surf->insideBounds(hit_point)) )  {
        error("FAILED: %s is ON SURFACE (SHOULD NOT BE)",sst.str().c_str());
        continue;
      }
      info("PASSED: %s is NOT ON SURFACE (SHOULD NOT BE)",sst.str().c_str());
    }
  }
}
