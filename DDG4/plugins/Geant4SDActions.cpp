// $Id: Geant4Field.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4Data.h"
#include "DD4hep/Printout.h"

using namespace std;

#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Geant4Mapping.h"
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"


namespace DD4hep {
  namespace Simulation   {

    typedef Geant4HitData::Contribution HitContribution;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<Geant4Tracker>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4Tracker>::defineCollections() {
      m_collectionID = defineCollection<Geant4Tracker::Hit>(m_sensitive.readout().name());
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4SensitiveAction<Geant4Tracker>::process(G4Step* step,G4TouchableHistory* /*hist*/ ) {
      typedef Geant4Tracker::Hit Hit;
      StepHandler h(step);
      Position prePos    = h.prePos();
      Position postPos   = h.postPos();
      Position direction = postPos - prePos;
      Position position  = mean_direction(prePos,postPos);
      double   hit_len   = direction.R();

      if ( step->GetTotalEnergyDeposit() < 1e-5 ) return true;

      if (hit_len > 0) {
	double new_len = mean_length(h.preMom(),h.postMom())/hit_len;
	direction *= new_len/hit_len;
      }
      print("Geant4Tracker","%s> Add hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f",
	    c_name(),step->GetTotalEnergyDeposit(),position.X(),position.Y(),position.Z());
      Hit* hit = new Hit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime());
      if ( hit )  {
	HitContribution contrib = Hit::extractContribution(step);
	hit->cellID        = cellID(step);
	hit->energyDeposit = contrib.deposit;
	hit->position      = position;
	hit->momentum      = direction;
	hit->length        = hit_len;
	collection(m_collectionID)->add(hit);
	mark(h.track);
	if ( 0 == hit->cellID )  {
	  hit->cellID        = volumeID( step ) ;
	  throw runtime_error("Invalid CELL ID for hit!");
	}
	print("Geant4Tracker","%s> Hit with deposit:%f  Pos:%f %f %f ID=%016X",
	      c_name(),step->GetTotalEnergyDeposit(),position.X(),position.Y(),position.Z(),
	      (void*)hit->cellID);
	Geant4TouchableHandler handler(step);
	print("Geant4Tracker","%s>     Geant4 path:%s",c_name(),handler.path().c_str());
	return true;
      }
      throw runtime_error("new() failed: Cannot allocate hit object");
    }
    typedef Geant4SensitiveAction<Geant4Tracker> Geant4TrackerAction;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<Calorimeter>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4Calorimeter>::defineCollections() {
      m_collectionID = defineCollection<Geant4Calorimeter::Hit>(m_sensitive.readout().name());
    }
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4SensitiveAction<Geant4Calorimeter>::process(G4Step* step,G4TouchableHistory*) {
      typedef Geant4Calorimeter::Hit Hit;
      StepHandler h(step);
      HitContribution contrib = Hit::extractContribution(step);
      HitCollection*  coll    = collection(m_collectionID);
      long long int   cell    = cellID(step);

      Hit* hit = coll->find<Hit>(CellIDCompare<Hit>(cell));
      if ( h.totalEnergy() < numeric_limits<double>::epsilon() )  {
	return true;
      }
      else if ( !hit ) {
	Geant4TouchableHandler handler(step);
	DDSegmentation::Vector3D pos = m_segmentation.position(cell);
	Position global = h.localToGlobal(pos);
	hit = new Hit(global);
	hit->cellID = cell;
	coll->add(hit);
	printM2("Geant4Calorimeter","%s> CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s",
		c_name(),contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str());
	if ( 0 == hit->cellID )  { // for debugging only!
	  hit->cellID = cellID(step);
	  throw runtime_error("Invalid CELL ID for hit!");
	}
      }
      hit->truth.push_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(step);
      return true;
    }
    typedef Geant4SensitiveAction<Geant4Calorimeter> Geant4CalorimeterAction;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<OpticalCalorimeter>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /// Helper class to define properti4es of optical calorimeters. UNTESTED
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    struct Geant4OpticalCalorimeter {};

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4OpticalCalorimeter>::defineCollections() {
      m_collectionID = defineCollection<Geant4Calorimeter::Hit>(m_sensitive.readout().name());
    }
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4SensitiveAction<Geant4OpticalCalorimeter>::process(G4Step* step,G4TouchableHistory*) {
      G4Track * track =  step->GetTrack();
      // check that particle is optical photon:
      if( track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() )  {
	return false;
      }
      else if ( track->GetCreatorProcess()->G4VProcess::GetProcessName() != "Cerenkov")  {
	track->SetTrackStatus(fStopAndKill);
	return false;
      }
      else {
	typedef Geant4Calorimeter::Hit Hit;
	StepHandler h(step);
	HitCollection*  coll    = collection(m_collectionID);
	HitContribution contrib = Hit::extractContribution(step);
	Position        pos     = h.prePos();
	Hit* hit = coll->find<Hit>(PositionCompare<Hit,Position>(pos));
	if ( !hit ) {
	  hit = new Hit(pos);
	  hit->cellID = volumeID(step);
	  coll->add(hit);
	  if ( 0 == hit->cellID )  {
	    hit->cellID = volumeID(step);
	    throw runtime_error("Invalid CELL ID for hit!");
	  }
	}
	hit->energyDeposit += contrib.deposit;
	hit->truth.push_back(contrib);
	track->SetTrackStatus(fStopAndKill); // don't step photon any further
	mark(h.track);
      	return true;
      }
    }
    typedef Geant4SensitiveAction<Geant4OpticalCalorimeter>  Geant4OpticalCalorimeterAction;

#if 0
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4SensitiveAction<TrackerCombine>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    struct TrackerCombine {
      Geant4TrackerHit  pre;
      Geant4TrackerHit  post;
      G4Track*          track;
      double            e_cut;
      int               current;
      TrackerCombine() : pre(), post(), track(0), e_cut(0.0), current(-1)  {
      }
      void start(G4Step* step, G4StepPoint* point)   {
	pre.storePoint(step,point);
	current = pre.truth.trackID;
	track = step->GetTrack();
	post = pre;
      }
      void update(G4Step* step) {
	post.storePoint(step,step->GetPostStepPoint());
	pre.truth.deposit += post.truth.deposit;
      }
      void clear()   {
	pre.truth.clear();
	current = -1;
	track = 0;
      }
      Geant4TrackerHit* extractHit(Geant4SensitiveDetector::HitCollection* c)   {
	if ( current == -1 || !track ) {
	  return 0;
	}
	else if ( pre.truth.deposit <= e_cut && !Geant4Hit::isGeantino(track) ) {
	  clear();
	  return 0;
	}
	Position pos = 0.5 * (pre.position + post.position);
	Momentum mom = 0.5 * (pre.momentum + post.momentum);
	double path_len = (post.position - pre.position).R();
	Geant4TrackerHit* hit = new Geant4TrackerHit(pre.truth.trackID,
						     pre.truth.pdgID,
						     pre.truth.deposit,
						     pre.truth.time);
	hit->position = pos;
	hit->momentum = mom;
	hit->length = path_len;
	clear();
	c->insert(hit);
	mark(h.track);
	return hit;
      }
    };

    /// Method invoked at the begining of each event. 
    template <> void Geant4SensitiveAction<TrackerCombine>::Initialize(G4HCofThisEvent* HCE) {
      userData.e_cut = m_sensitive.energyCutoff();
      this->Geant4SensitiveDetector::Initialize(HCE);
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> void Geant4SensitiveAction<TrackerCombine>::clear() {
      userData.clear();
      this->Geant4SensitiveDetector::clear();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> G4bool Geant4SensitiveAction<TrackerCombine>::ProcessHits(G4Step* step,G4TouchableHistory* ) {
      StepHandler h(step);
      bool return_code = false;

      if ( !userData.track || userData.current != h.track->GetTrackID() ) {
	return_code = userData.extractHit(collection(0)) != 0;
	userData.start(step, h.pre);
      }

      // ....update .....
      userData.update(step);

      void *prePV = h.volume(h.pre), *postPV = h.volume(h.post);
      if ( prePV != postPV ) {
	void* postSD = h.sd(h.post);
	return_code = userData.extractHit(collection(0)) != 0;
	if ( 0 != postSD )   {
	  void* preSD = h.sd(h.pre);
	  if ( preSD == postSD ) {
	    userData.start(step,h.post);
	  }
	}
      }
      else if ( userData.track->GetTrackStatus() == fStopAndKill ) {
	return_code = userData.extractHit(collection(0)) != 0;
      }
      return return_code;
    }
    typedef Geant4SensitiveAction<TrackerCombine>  Geant4TrackerCombine;
#endif


    typedef Geant4TrackerAction Geant4SimpleTrackerAction;
    typedef Geant4CalorimeterAction Geant4SimpleCalorimeterAction;
    typedef Geant4OpticalCalorimeterAction Geant4SimpleOpticalCalorimeterAction;
  }
}

using namespace DD4hep::Simulation;

#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE(Geant4TrackerAction)
DECLARE_GEANT4SENSITIVE(Geant4CalorimeterAction)
DECLARE_GEANT4SENSITIVE(Geant4OpticalCalorimeterAction)

// Need these factories for backwards compatibility
DECLARE_GEANT4SENSITIVE(Geant4SimpleTrackerAction)
DECLARE_GEANT4SENSITIVE(Geant4SimpleCalorimeterAction)
DECLARE_GEANT4SENSITIVE(Geant4SimpleOpticalCalorimeterAction)
