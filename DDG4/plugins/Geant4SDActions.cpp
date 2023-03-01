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
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4FastSimHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  
  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    namespace {
      struct Geant4VoidSensitive {};
    }
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<Geant4VoidSensitive>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package Geant4VoidSensitiveAction
     * \brief Void Sensitive detector action to skip the processing of a detector
     *        without changing the entire DDG4 setup.
     *
     * @}
     */

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4VoidSensitive>::defineCollections()    {
      m_collectionID = -1;
    }

    /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4VoidSensitive>::process(const G4Step*       /* step */,
							G4TouchableHistory* /* hist */) {
      return true;
    }
    
    /// GFLASH/FastSim interface: Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4VoidSensitive>::processFastSim(const Geant4FastSimSpot* /* spot */,
							       G4TouchableHistory*      /* hist */) {
      return true;
    }
    typedef Geant4SensitiveAction<Geant4VoidSensitive> Geant4VoidSensitiveAction;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<Geant4Tracker>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package Geant4TrackerAction
     * \brief Sensitive detector meant for tracking detectors, will produce one hit per step
     *
     * @}
     */

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4Tracker>::defineCollections()    {
      m_collectionID = declareReadoutFilteredCollection<Geant4Tracker::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4Tracker>::process(const G4Step* step,G4TouchableHistory* /* hist */) {
      typedef Geant4Tracker::Hit Hit;
      Geant4StepHandler h(step);
      Position  prePos    = h.prePos();
      Position  postPos   = h.postPos();
      Position  direction = postPos - prePos;
      Position  pos       = mean_direction(prePos, postPos);
      Direction mom       = 0.5 * (h.preMom() + h.postMom());
      double    hit_len   = direction.R();

      // if (hit_len > 0) {
      //   double new_len = mean_length(h.preMom(),h.postMom())/hit_len;
      //   direction *= new_len/hit_len;
      // }
      Hit* hit = new Hit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime(), hit_len, pos, mom);
      hit->truth  = Hit::extractContribution(step);
      hit->cellID = cellID(step);
      collection(m_collectionID)->add(hit);
      mark(h.track);
      if ( 0 == hit->cellID )  {
        hit->cellID = volumeID(step);
        except("+++ Invalid CELL ID for hit!");
      }
      print("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            step->GetTotalEnergyDeposit(), pos.X(), pos.Y(), pos.Z(), (void*)hit->cellID);
      Geant4TouchableHandler handler(step);
      print("    Geant4 path:%s",handler.path().c_str());
      return true;
    }
    
    /// GFlash/FastSim interface: Method for generating hit(s) using the information of Geant4FastSimSpot object.
    template <> bool
    Geant4SensitiveAction<Geant4Tracker>::processFastSim(const Geant4FastSimSpot* spot,
							 G4TouchableHistory* /* hist */)
    {
      typedef Geant4Tracker::Hit Hit;
      Geant4FastSimHandler h(spot);
      Hit* hit = new Hit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime(),
			 0e0, h.avgPosition(), h.momentum());
      hit->cellID        = cellID(h.touchable(), h.avgPositionG4());
      collection(m_collectionID)->add(hit);
      mark(h.track);
      if ( 0 == hit->cellID )  {
        hit->cellID = volumeID(h.touchable());
        except("+++ Invalid CELL ID for hit!");
      }
      print("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            h.deposit(),hit->position.X(),hit->position.Y(),hit->position.Z(),(void*)hit->cellID);
      Geant4TouchableHandler handler(h.touchable());
      print("    Geant4 path:%s",handler.path().c_str());
      return true;
    }

    typedef Geant4SensitiveAction<Geant4Tracker> Geant4TrackerAction;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<Geant4PhotonCounter>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package Geant4OpticalTrackerAction
     * \brief Sensitive detector meant for photon detectors, will produce one hit per step
     *        for regular particles, but absorb optical photons fully on their first hit
     *
     * @}
     */

    /// Helper class to define properties of optical trackers. UNTESTED
    struct Geant4OpticalTracker {};

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4OpticalTracker>::defineCollections()    {
      m_collectionID = declareReadoutFilteredCollection<Geant4Tracker::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4OpticalTracker>::process(const G4Step* step,G4TouchableHistory* /* hist */) {
      G4Track * track =  step->GetTrack();
      typedef Geant4Tracker::Hit Hit;
      Geant4StepHandler h(step);
      Position  prePos    = h.prePos();
      Position  postPos   = h.postPos();
      Position  direction = postPos - prePos;
      Position  pos       = mean_direction(prePos, postPos);
      Direction mom       = 0.5 * ( h.preMom() + h.postMom() ) ;
      double    tim       = h.track->GetGlobalTime();
      double    hit_len   = direction.R();
      
      Hit* hit = new Hit(h.trkID(), h.trkPdgID(), h.deposit(), tim, hit_len, pos, mom);
      hit->truth         = Hit::extractContribution(step);
      hit->energyDeposit = hit->truth.deposit;
      hit->cellID        = cellID(step);
      if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        track->SetTrackStatus(fStopAndKill);
      }
      collection(m_collectionID)->add(hit);
      mark(h.track);
      if ( 0 == hit->cellID )  {
        hit->cellID      = volumeID( step ) ;
        except("+++ Invalid CELL ID for hit!");
      }
      print("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            step->GetTotalEnergyDeposit(), pos.X(),pos.Y(), pos.Z(), (void*)hit->cellID);
      Geant4TouchableHandler handler(step);
      print("    Geant4 path:%s",handler.path().c_str());
      return true;
    }

    /// GFlash/FastSim interface: Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4OpticalTracker>::processFastSim(const Geant4FastSimSpot* spot,
							        G4TouchableHistory* /* hist */)
    {
      typedef Geant4Tracker::Hit Hit;
      Geant4FastSimHandler h(spot);
      Hit* hit = new Hit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime(),
			 0e0, h.avgPosition(), h.momentum());
      hit->cellID        = cellID(h.touchable(), h.avgPositionG4());
      collection(m_collectionID)->add(hit);
      mark(h.track);
      if ( 0 == hit->cellID )  {
        hit->cellID      = volumeID(h.touchable());
        except("+++ Invalid CELL ID for hit!");
      }
      print("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            h.deposit(),hit->position.X(),hit->position.Y(),hit->position.Z(),(void*)hit->cellID);
      Geant4TouchableHandler handler(h.touchable());
      print("    Geant4 path:%s",handler.path().c_str());
      return true;
    }
    typedef Geant4SensitiveAction<Geant4OpticalTracker> Geant4OpticalTrackerAction;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<Calorimeter>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /** \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package Geant4CalorimeterAction
     *
     * \brief Sensitive detector meant for calorimeters

     *
     * @}
     */
    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4Calorimeter>::defineCollections() {
      m_collectionID = declareReadoutFilteredCollection<Geant4Calorimeter::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4Calorimeter>::process(const G4Step* step,G4TouchableHistory*) {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4StepHandler    h(step);
      HitContribution      contrib = Hit::extractContribution(step);
      Geant4HitCollection* coll    = collection(m_collectionID);
      VolumeID cell = 0;

      try {
        cell = cellID(step);
      } catch(std::runtime_error &e) {
        std::stringstream out;
        out << std::setprecision(20) << std::scientific;
        out << "ERROR: " << e.what()  << std::endl;
        out << "Position: "
            << "Pre (" << std::setw(24) << step->GetPreStepPoint()->GetPosition() << ") "
            << "Post (" << std::setw(24) << step->GetPostStepPoint()->GetPosition() << ") "
            << std::endl;
        out << "Momentum: "
            << " Pre (" <<std::setw(24) << step->GetPreStepPoint() ->GetMomentum()  << ") "
            << " Post (" <<std::setw(24) << step->GetPostStepPoint()->GetMomentum() << ") "
            << std::endl;
        std::cout << out.str();
        return true;
      }

      //Hit* hit = coll->find<Hit>(CellIDCompare<Hit>(cell));
      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
        Geant4TouchableHandler handler(step);
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);
        Position global = h.localToGlobal(pos);
        hit = new Hit(global);
        hit->cellID = cell;
        coll->add(cell, hit);
        printM2("%s> CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s  [%s]",
                c_name(),contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str(),
                coll->GetName().c_str());
        if ( 0 == hit->cellID )  { // for debugging only!
          hit->cellID = cellID(step);
          except("+++ Invalid CELL ID for hit!");
        }
      }
      hit->truth.emplace_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(h.track);
      return true;
    }
    /// GFlash/FastSim interface: Method for generating hit(s) using the information of Geant4FastSimSpot object.
    template <> bool
    Geant4SensitiveAction<Geant4Calorimeter>::processFastSim(const Geant4FastSimSpot* spot,
							     G4TouchableHistory* /* hist */)
    {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4FastSimHandler h(spot);
      HitContribution      contrib = Hit::extractContribution(spot);
      Geant4HitCollection* coll    = collection(m_collectionID);
      VolumeID cell = 0;
      
      try {
        cell = cellID(h.touchable(), h.avgPositionG4());
      } catch(std::runtime_error &e) {
        std::stringstream out;
        out << std::setprecision(20) << std::scientific;
        out << "ERROR: " << e.what()  << std::endl;
        out << "Position: (" << std::setw(24) << h.avgPositionG4() << ") " << std::endl;
        out << "Momentum: (" << std::setw(24) << h.momentumG4() << ") " << std::endl;
        std::cout << out.str();
        return true;
      }
      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
	Geant4TouchableHandler   handler(h.touchable());
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);
        Position global = h.localToGlobal(pos);
        hit = new Hit(global);
        hit->cellID = cell;
        coll->add(cell, hit);
        printM2("%s> CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s  [%s]",
                c_name(),contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str(),
                coll->GetName().c_str());
        if ( 0 == hit->cellID )  { // for debugging only!
          hit->cellID = cellID(h.touchable(), h.avgPositionG4());
          except("+++ Invalid CELL ID for hit!");
        }
      }
      hit->truth.emplace_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(h.track);
      return true;
    }

    typedef Geant4SensitiveAction<Geant4Calorimeter> Geant4CalorimeterAction;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<OpticalCalorimeter>
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package Geant4OpticalCalorimeterAction
     *
     * \brief Sensitive detector meant for optical calorimeters
     *
     * @}
     */
    /// Helper class to define properties of optical calorimeters. UNTESTED
    struct Geant4OpticalCalorimeter {};

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4OpticalCalorimeter>::defineCollections() {
      m_collectionID = declareReadoutFilteredCollection<Geant4Calorimeter::Hit>();
    }
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4OpticalCalorimeter>::process(const G4Step* step,G4TouchableHistory*) {
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
        Geant4StepHandler h(step);
        Geant4HitCollection*  coll    = collection(m_collectionID);
        HitContribution       contrib = Hit::extractContribution(step);
        Position              pos     = h.prePos();
        Hit* hit = coll->find<Hit>(PositionCompare<Hit,Position>(pos));
        if ( !hit ) {
          hit = new Hit(pos);
          hit->cellID = volumeID(step);
          coll->add(hit);
          if ( 0 == hit->cellID )  {
            hit->cellID = volumeID(step);
            except("+++ Invalid CELL ID for hit!");
          }
        }
        hit->energyDeposit += contrib.deposit;
        hit->truth.emplace_back(contrib);
        track->SetTrackStatus(fStopAndKill); // don't step photon any further
        mark(h.track);
        return true;
      }
    }

    /// GFlash/FastSim interface: Method for generating hit(s) using the information of Geant4FastSimSpot object.
    template <> bool
    Geant4SensitiveAction<Geant4OpticalCalorimeter>::processFastSim(const Geant4FastSimSpot* spot,
								    G4TouchableHistory* /* hist */)
    {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4FastSimHandler   h(spot);
      const G4Track* track = h.track;
      if( track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition() )  {
        return false;
      }
      else if ( track->GetCreatorProcess()->G4VProcess::GetProcessName() != "Cerenkov")  {
        return false;
      }
      else {
        Geant4HitCollection* coll = collection(m_collectionID);
        HitContribution   contrib = Hit::extractContribution(spot);
        Position          pos     = h.avgPosition();
        Hit* hit = coll->find<Hit>(PositionCompare<Hit,Position>(pos));
        if ( !hit ) {
          hit = new Hit(pos);
          hit->cellID = volumeID(h.touchable());
          coll->add(hit);
          if ( 0 == hit->cellID )  {
            hit->cellID = volumeID(h.touchable());
            except("+++ Invalid CELL ID for hit!");
          }
        }
        hit->energyDeposit += contrib.deposit;
        hit->truth.emplace_back(contrib);
        mark(h.track);
        return true;
      }
    }
    typedef Geant4SensitiveAction<Geant4OpticalCalorimeter>  Geant4OpticalCalorimeterAction;


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //               Geant4SensitiveAction<ScintillatorCalorimeter>
    //               For scintillator with Geant4 BirksLaw effect
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    /* \addtogroup Geant4SDActionPlugin
     *
     * @{
     * \package Geant4ScintillatorCalorimeterAction
     * \brief Sensitive detector meant for scintillator calorimeters
     *
     * This sensitive action will apply Birks' law to the energy deposits
     *
     * @}
     */
    /// Class to implement the standard sensitive detector for scintillator calorimeters
    struct Geant4ScintillatorCalorimeter {};

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4ScintillatorCalorimeter>::defineCollections() {
      m_collectionID = declareReadoutFilteredCollection<Geant4Calorimeter::Hit>();
    }
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<Geant4ScintillatorCalorimeter>::process(const G4Step* step,G4TouchableHistory*) {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4StepHandler    h(step);
      HitContribution      contrib = Hit::extractContribution(step,true);
      Geant4HitCollection* coll    = collection(m_collectionID);
      VolumeID cell = 0;
      try {
        cell = cellID(step);
      } catch(std::runtime_error &e) {
        std::stringstream out;
        out << std::setprecision(20) << std::scientific;
        out << "ERROR: " << e.what()  << std::endl;
        out << "Position: "
            << "Pre (" << std::setw(24) << step->GetPreStepPoint()->GetPosition() << ") "
            << "Post (" << std::setw(24) << step->GetPostStepPoint()->GetPosition() << ") "
            << std::endl;
        out << "Momentum: "
            << " Pre (" <<std::setw(24) << step->GetPreStepPoint() ->GetMomentum()  << ") "
            << " Post (" <<std::setw(24) << step->GetPostStepPoint()->GetMomentum() << ") "
            << std::endl;
        std::cout << out.str();
        return true;
      }
      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
        Geant4TouchableHandler handler(step);
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);
        Position global = h.localToGlobal(pos);
        hit = new Hit(global);
        hit->cellID = cell;
        coll->add(cell, hit);
        printM2("CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s",
                contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str());
        if ( 0 == hit->cellID )  { // for debugging only!
          hit->cellID = cellID(step);
          except("+++ Invalid CELL ID for hit!");
        }
      }
      hit->truth.emplace_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(h.track);
      return true;
    }

    /// GFlash/FastSim interface: Method for generating hit(s) using the information of Geant4FastSimSpot object.
    template <> bool
    Geant4SensitiveAction<Geant4ScintillatorCalorimeter>::processFastSim(const Geant4FastSimSpot* spot,
									 G4TouchableHistory* /* hist */)
    {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4FastSimHandler h(spot);
      HitContribution         contrib = Hit::extractContribution(spot);
      Geant4HitCollection*    coll    = collection(m_collectionID);
      VolumeID cell = 0;
      
      try {
        cell = cellID(h.touchable(), h.avgPositionG4());
      } catch(std::runtime_error &e) {
        std::stringstream out;
        out << std::setprecision(20) << std::scientific;
        out << "ERROR: " << e.what()  << std::endl;
        out << "Position: (" << std::setw(24) << h.avgPositionG4() << ") " << std::endl;
        out << "Momentum: (" << std::setw(24) << h.momentumG4() << ") " << std::endl;
        std::cout << out.str();
        return true;
      }
      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
	Geant4TouchableHandler   handler(h.touchable());
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);
        Position global = h.localToGlobal(pos);
        hit = new Hit(global);
        hit->cellID = cell;
        coll->add(cell, hit);
        printM2("CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s",
                contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str());
        if ( 0 == hit->cellID )  { // for debugging only!
          hit->cellID = cellID(h.touchable(), h.avgPositionG4());
          except("+++ Invalid CELL ID for hit!");
        }
      }
      hit->truth.emplace_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(h.track);
      return true;
    }

    typedef Geant4SensitiveAction<Geant4ScintillatorCalorimeter> Geant4ScintillatorCalorimeterAction;

    /**
     *
     * \addtogroup Geant4SDActionPlugin
     * @{
     * \package Geant4TrackerCombineAction
     *
     * \brief Sensitive detector meant for tracking detectors will combine
     * multiple steps of the same track in the same sensitive volume into a
     * single hit

     *
     * @}
     */
    /// Geant4 sensitive detector combining all deposits of one G4Track within one sensitive element.
    /**
     *  Geant4SensitiveAction<TrackerCombine>
     *
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    struct TrackerCombine {
      Geant4Tracker::Hit pre, post;
      Position           mean_pos;
      Geant4Sensitive*   sensitive { nullptr };
      G4VPhysicalVolume* firstSpotVolume  { nullptr };
      double             mean_time {  0e0 };
      double             e_cut     {  0e0 };
      int                current   {   -1 };
      int                combined  {    0 };
      long long int      cell      {    0 };

      TrackerCombine() : pre(), post()   {
      }

      /// Start a new hit
      void start_collecting(const G4Track* track)   {
        pre.truth.deposit = 0.0;
        current = pre.truth.trackID;
        sensitive->mark(track);
        mean_pos.SetXYZ(0,0,0);
        mean_time = 0;
        post.copyFrom(pre);
        combined = 0;
        cell = 0;
      }
      void start(const G4Step* step, const G4StepPoint* point)   {
        pre.storePoint(step,point);
	start_collecting(step->GetTrack());
	firstSpotVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
      }
      void start(const Geant4FastSimSpot* spot)   {
        pre.storePoint(spot);
	start_collecting(spot->primary);
	firstSpotVolume = spot->volume();
      }

      /// Update energy and track information during hit info accumulation
      void update_collected_hit(const G4VTouchable* h, G4ThreeVector&& global)    {
        pre.truth.deposit += post.truth.deposit;
        mean_pos.SetX(mean_pos.x()+post.position.x()*post.truth.deposit);
        mean_pos.SetY(mean_pos.y()+post.position.y()*post.truth.deposit);
        mean_pos.SetZ(mean_pos.z()+post.position.z()*post.truth.deposit);
        mean_time += post.truth.time*post.truth.deposit;
        if ( 0 == cell )   {
          cell = sensitive->cellID(h, global);
          if ( 0 == cell )  {
            cell = sensitive->volumeID(h) ;
            sensitive->except("+++ Invalid CELL ID for hit!");
          }
        }
        ++combined;
      }
      void update(const Geant4StepHandler& h) {
        post.storePoint(h.step, h.post);
	update_collected_hit(h.preTouchable(), h.avgPositionG4()); // Compute cellID
      }
      void update(const Geant4FastSimHandler& h)   {
        post.storePoint(h.spot);
	update_collected_hit(h.touchable(), h.avgPositionG4());       // Compute cellID
      }

      /// Clear collected information and restart for new hit
      void clear()  {
        mean_pos.SetXYZ(0,0,0);
        mean_time = 0;
        post.clear();
        pre.clear();
        current = -1;
        combined = 0;
        cell = 0;
	firstSpotVolume = nullptr;
      }

      /// Helper function to decide if the hit has to be extracted and saved in the collection
      bool mustSaveTrack(const G4Track* tr)  const   {
        return current > 0 && current != tr->GetTrackID();
      }

      /// Extract hit information and add the created hit to the collection
      void extractHit(Geant4HitCollection* collection)   {
        if ( current == -1 ) {
          return;
        }
        double   depo = pre.truth.deposit;
        double   time = depo != 0 ? mean_time / depo : mean_time;
        Position pos  = depo != 0 ? mean_pos  / depo : mean_pos;
        Momentum mom  = 0.5 * (pre.momentum + post.momentum);
        double   path_len = (post.position - pre.position).R();
        Geant4Tracker::Hit* hit = new Geant4Tracker::Hit(pre.truth.trackID, pre.truth.pdgID,
                                                         depo, time, path_len, pos, mom);
        hit->cellID   = cell;
        collection->add(hit);
        sensitive->printM2("+++ TrackID:%6d [%s] CREATE hit combination with %2d deposit(s):"
                           " %e MeV  Pos:%8.2f %8.2f %8.2f",
                           pre.truth.trackID,sensitive->c_name(),combined,pre.truth.deposit/CLHEP::MeV,
                           pos.X()/CLHEP::mm,pos.Y()/CLHEP::mm,pos.Z()/CLHEP::mm);
        clear();
      }


      /// Method for generating hit(s) using the information of G4Step object.
      G4bool process(const G4Step* step, G4TouchableHistory* ) {
        Geant4StepHandler h(step);

	// std::cout << " process called - pre pos: " << h.prePos() << " post pos " << h.postPos() 
	// 	  << " edep: " << h.deposit() << std::endl ;

        void *prePV = h.volume(h.pre), *postPV = h.volume(h.post);

        Geant4HitCollection* coll = sensitive->collection(0);
        /// If we are handling a new track, then store the content of the previous one.
        if ( mustSaveTrack(h.track) )  {
          extractHit(coll);
        }
        /// Initialize the deposits of the next hit.
        if ( current < 0 )  {
          start(step, h.pre);
        }
        /// ....update .....
        update(h);

        if ( prePV != postPV ) {
          void* postSD = h.sd(h.post);
          extractHit(coll);
          if ( 0 != postSD )   {
            void* preSD = h.sd(h.pre);
            if ( preSD == postSD ) {
              start(step,h.post);
            }
          }
        }
        else if ( h.track->GetTrackStatus() == fStopAndKill ) {
          extractHit(coll);
        }
        return true;
      }

      /// Method for generating hit(s) using the information of fast simulation spot object.
      G4bool process(const Geant4FastSimSpot* spot, G4TouchableHistory* ) {
	Geant4FastSimHandler h(spot);
        G4VPhysicalVolume*   prePV = firstSpotVolume, *postPV = h.volume();
        Geant4HitCollection* coll  = sensitive->collection(0);
        /// If we are handling a new track, then store the content of the previous one.
        if ( mustSaveTrack(h.track) )  {
          extractHit(coll);
        }
        /// Initialize the deposits of the next hit.
        if ( current < 0 )  {
          start(spot);
        }
        /// ....update .....
        update(h);

        if ( firstSpotVolume && prePV != postPV )   {
          void* postSD = h.sd();
          extractHit(coll);
          if ( 0 != postSD )   {
            void* preSD = prePV ? prePV->GetLogicalVolume()->GetSensitiveDetector() : nullptr;
            if ( preSD == postSD ) {
              start(spot);
            }
          }
        }
        else if ( h.track->GetTrackStatus() == fStopAndKill ) {
          extractHit(coll);
        }
	return true;
      }

      /// Post-event action callback
      void endEvent(const G4Event* /* event */)   {
        // We need to add the possibly last added hit to the collection here.
        // otherwise the last hit would be assigned to the next event and the
        // MC truth would be screwed.
        //
        // Alternatively the 'update' method would become rather CPU consuming,
        // beacuse the extract action would have to be recalculated over and over.
        if ( current > 0 )   {
          Geant4HitCollection* coll = sensitive->collection(0);
          extractHit(coll);
        }
      }
    };

    /// Initialization overload for specialization
    template <> void Geant4SensitiveAction<TrackerCombine>::initialize() {
      eventAction().callAtEnd(&m_userData,&TrackerCombine::endEvent);
      m_userData.e_cut = m_sensitive.energyCutoff();
      m_userData.sensitive = this;
    }

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<TrackerCombine>::defineCollections() {
      m_collectionID = declareReadoutFilteredCollection<Geant4Tracker::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> void Geant4SensitiveAction<TrackerCombine>::clear(G4HCofThisEvent*) {
      m_userData.clear();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> G4bool
    Geant4SensitiveAction<TrackerCombine>::process(const G4Step* step, G4TouchableHistory* history) {
      return m_userData.process(step, history);
    }

    /// GFlash/FastSim interface: Method for generating hit(s) using the information of Geant4FastSimSpot object.
    template <> bool
    Geant4SensitiveAction<TrackerCombine>::processFastSim(const Geant4FastSimSpot* spot,
							  G4TouchableHistory*      history)    {
      return m_userData.process(spot, history);
    }

    typedef Geant4SensitiveAction<TrackerCombine>  Geant4TrackerCombineAction;
    typedef Geant4TrackerAction                    Geant4SimpleTrackerAction;
    typedef Geant4CalorimeterAction                Geant4SimpleCalorimeterAction;
    typedef Geant4OpticalCalorimeterAction         Geant4SimpleOpticalCalorimeterAction;
  }
}

using namespace dd4hep::sim;

#include "DDG4/Factories.h"
// Special void entry point
DECLARE_GEANT4SENSITIVE(Geant4VoidSensitiveAction)
// Standard factories used for simulation
DECLARE_GEANT4SENSITIVE(Geant4TrackerAction)
DECLARE_GEANT4SENSITIVE(Geant4OpticalTrackerAction)
DECLARE_GEANT4SENSITIVE(Geant4TrackerCombineAction)
DECLARE_GEANT4SENSITIVE(Geant4CalorimeterAction)
DECLARE_GEANT4SENSITIVE(Geant4OpticalCalorimeterAction)
DECLARE_GEANT4SENSITIVE(Geant4ScintillatorCalorimeterAction)

// Need these factories for backwards compatibility
DECLARE_GEANT4SENSITIVE(Geant4SimpleTrackerAction)
DECLARE_GEANT4SENSITIVE(Geant4SimpleCalorimeterAction)
DECLARE_GEANT4SENSITIVE(Geant4SimpleOpticalCalorimeterAction)
