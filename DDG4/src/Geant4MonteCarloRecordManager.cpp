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
#include "DDG4/Geant4TrackPersistency.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4MonteCarloRecordManager.h"
#include "DDG4/Geant4TrackHandler.h"
#include "DDG4/Geant4StepHandler.h"

#include "G4TrackingManager.hh"
#include "G4Track.hh"

using namespace DD4hep::Simulation;

/// Standard constructor
Geant4MonteCarloRecordManager::Geant4MonteCarloRecordManager(Geant4Context* context, const std::string& nam) 
  : Geant4Action(context,nam)  
{
  declareProperty("Collect",m_collectInfo=true);
}

/// Default destructor
Geant4MonteCarloRecordManager::~Geant4MonteCarloRecordManager()  {
}

/// Save G4Track data
void Geant4MonteCarloRecordManager::save(const Geant4TrackPersistency::TrackInfo& trk)   {
  if ( m_collectInfo )   {
    Geant4TrackHandler track(trk.track);
#if 0
    //    const G4Step* step = track.step();
    std::string proc = track.creatorName();
    float ene = trk.initialEnergy;
    float loss = ene-track.energy();
    G4ThreeVector pos = track.position();
    G4ThreeVector org = track.vertex();

    info("Track ID=%4d Parent:%4d  E:%7.2f MeV Loss:%7.2f MeV  "
	 "%-6s %-8s L:%7.2f Org:(%8.2f %8.2f %8.2f) Pos:(%8.2f %8.2f %8.2f)",
	 track.id(), track.parent(), ene, loss,
	 track.name().c_str(), ("["+proc+"]").c_str(), track.length(), 
	 org.x(),org.y(),org.z(),
	 pos.x(),pos.y(),pos.z());
#endif
  }
}

