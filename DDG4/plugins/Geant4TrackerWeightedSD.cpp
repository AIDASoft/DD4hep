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
#include "DD4hep/DD4hepUnits.h"
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4EventAction.h"
#include "G4Event.hh"
#include "G4VSolid.hh"

#include <map>
#include <limits>
#include <sstream>

using namespace std;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    using namespace detail;

    /**
     *  \addtogroup Geant4SDActionPlugin
     *
     *  @{
     *  \package Geant4TrackerWeightedAction
     *
     *  \brief Sensitive detector meant for tracking detectors with multiple ways to combine steps
     *
     *
     *  \param integer HitPositionCombination
     *   -# Use energy weights to define the position of the energy deposit
     *   -# Set the hit position between the step pre and post point
     *   -# Set the hit position to the position of the step pre point
     *   -# Set the hit position to the position of the step post point
     *
     *  \param bool CollectSingleDeposits
     *   - If true each step is written out
     *
     * @}
     */
    /// Geant4 sensitive detector combining all deposits of one G4Track within one sensitive element.
    /**
     *  Geant4SensitiveAction<TrackerWeighted>
     *
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    struct TrackerWeighted {

      /// Enumeration to define the calculation of the position of the energy deposit
      enum  {
        POSITION_WEIGHTED  = 1, // Use energy weights to define the position of the energy deposit
        POSITION_MIDDLE    = 2, // Set the hit position between the step pre and post point
        POSITION_PREPOINT  = 3, // Set the hit position to the position of the step pre point
        POSITION_POSTPOINT = 4  // Set the hit position to the position of the step post point
      };
      
      Geant4Tracker::Hit    pre, post;
      Position              mean_pos;
      Geant4Sensitive*      sensitive            = 0;
      G4VSensitiveDetector* thisSD               = 0;
      G4VPhysicalVolume*    thisPV               = 0;
      double                distance_to_inside   = 0.0;
      double                distance_to_outside  = 0.0;
      double                mean_time            = 0.0;
      double                step_length          = 0.0;
      double                e_cut                = 0.0;
      int                   current              = -1;
      int                   parent               = 0;
      int                   combined             = 0;
      int                   hit_position_type    = POSITION_MIDDLE;
      int                   hit_flag             = 0;
      int                   g4ID                 = 0;
      EInside               last_inside          = kOutside;
      long long int         cell                 = 0;
      bool                  single_deposit_mode  = false;

      /// Default constructor
      TrackerWeighted() = default;

      /// Clear collected information and restart for new hit
      TrackerWeighted& clear()   {
        mean_pos.SetXYZ(0,0,0);
        distance_to_inside = 0;
        distance_to_outside = 0;
        mean_time = 0;
        step_length  = 0;
        thisPV = nullptr;
        post.clear();
        pre.clear();
        current  = -1;
        parent   = -1;
        combined =  0;
        cell     =  0;
        hit_flag =  0;
        g4ID     =  0;
        last_inside = kOutside;
        return *this;
      }

      /// Start a new hit
      TrackerWeighted& start(const G4Step* step, const G4StepPoint* point)   {
        if( DEBUG == printLevel() ) {
          std::cout<<" DEBUG: Geant4TrackerWeightedSD::start(const G4Step* step, const G4StepPoint* point) ...."<<std::endl;
          Geant4StepHandler h(step);
          dumpStep( h, step);
        }

        clear();
        pre.storePoint(step,point);
        pre.truth.deposit = 0.0;
        post.truth.deposit = 0.0;
        current = pre.truth.trackID;
        sensitive->mark(step->GetTrack());
        post.copyFrom(pre);
        parent = step->GetTrack()->GetParentID();
        g4ID = step->GetTrack()->GetTrackID();

        Geant4StepHandler startVolume(step);
        thisPV = startVolume.preVolume();

        return *this;
      }

      /// Update energy and track information during hit info accumulation
      TrackerWeighted& update(const G4Step* step)   {
        if( DEBUG == printLevel() ) {
          std::cout<<" DEBUG: Geant4TrackerWeightedSD::update(const G4Step* step) ...."<<std::endl;
          Geant4StepHandler h(step);
          dumpStep( h, step);
        }

        post.storePoint(step,step->GetPostStepPoint());
        Position mean    = (post.position+pre.position)*0.5;
        double   mean_tm = (post.truth.time+pre.truth.time)*0.5;
        pre.truth.deposit += post.truth.deposit;
        mean_pos.SetX(mean_pos.x()+mean.x()*post.truth.deposit);
        mean_pos.SetY(mean_pos.y()+mean.y()*post.truth.deposit);
        mean_pos.SetZ(mean_pos.z()+mean.z()*post.truth.deposit);
        mean_time += mean_tm*post.truth.deposit;
        step_length  += step->GetStepLength();
        if ( 0 == cell )   {
          cell = sensitive->cellID(step);
          if ( 0 == cell )  {
            cell = sensitive->volumeID(step) ;
            sensitive->except("+++ Invalid CELL ID for hit!");
          }
        }
        ++combined;
        return *this;
      }

      /// Helper function to decide if the hit has to be extracted and saved in the collection
      bool mustSaveTrack(const G4Track* tr)  const   {
        return current > 0 && current != tr->GetTrackID();
      }

      /// Extract hit information and add the created hit to the collection
      void extractHit(EInside ended)   {
        Geant4HitCollection* collection = sensitive->collection(0);
        extractHit(collection, ended);
      }

      TrackerWeighted& calc_dist_out(const G4VSolid* solid)    {
        Position v(pre.momentum.unit()), &p=post.position;
        double dist = solid->DistanceToOut(G4ThreeVector(p.X(),p.Y(),p.Z()),
                                           G4ThreeVector(v.X(),v.Y(),v.Z()));
        distance_to_outside = dist;
        return *this;
      }

      TrackerWeighted& calc_dist_in(const G4VSolid* solid)    {
        Position v(pre.momentum.unit()), &p=pre.position;
        double dist = solid->DistanceToOut(G4ThreeVector(p.X(),p.Y(),p.Z()),
                                           G4ThreeVector(v.X(),v.Y(),v.Z()));
        distance_to_inside = dist;
        return *this;
      }

      void extractHit(Geant4HitCollection* collection, EInside ended)   {
        if( DEBUG == printLevel() ) {
          std::cout<<" DEBUG: Geant4TrackerWeightedSD::extractHit(Geant4HitCollection* collection, EInside ended) ...."<<std::endl;
          std::cout<<" DEBUG: =================================================="<<std::endl;
        }

        double deposit  = pre.truth.deposit;
        if ( current != -1 )  {
          Position pos;
          Momentum mom  = 0.5 * (pre.momentum + post.momentum);
          double   time = deposit != 0 ? mean_time / deposit : mean_time;
          char     dist_in[64], dist_out[64];

          switch(hit_position_type)  {
          case POSITION_WEIGHTED:
            pos = deposit != 0 ? mean_pos / deposit : mean_pos;
            break;
          case POSITION_PREPOINT:
            pos = pre.position;
            break;
          case POSITION_POSTPOINT:
            pos = post.position;
            break;
          case POSITION_MIDDLE:
          default:
            pos = (post.position + pre.position) / 2.0;
            break;
          }

          if ( ended == kSurface || distance_to_outside < numeric_limits<float>::epsilon() )
            hit_flag |= Geant4Tracker::Hit::HIT_ENDED_SURFACE;
          else if ( ended == kInside )
            hit_flag |= Geant4Tracker::Hit::HIT_ENDED_INSIDE;
          else if ( ended == kOutside )
            hit_flag |= Geant4Tracker::Hit::HIT_ENDED_OUTSIDE;
          
          Geant4Tracker::Hit* hit = new Geant4Tracker::Hit(pre.truth.trackID,
                                                           pre.truth.pdgID,
                                                           deposit,time);
          hit->flag     = hit_flag;
          hit->position = pos;
          hit->momentum = mom;
          hit->length   = step_length;
          hit->cellID   = cell;
          hit->g4ID     = g4ID;

          dist_in[0] = dist_out[0] = 0;
          if ( !(hit_flag&Geant4Tracker::Hit::HIT_STARTED_SURFACE) )
            ::snprintf(dist_in,sizeof(dist_in)," [%.2e um]",distance_to_inside/CLHEP::um);
          if ( !(hit_flag&Geant4Tracker::Hit::HIT_ENDED_SURFACE) )
            ::snprintf(dist_out,sizeof(dist_out)," [%.2e um]",distance_to_outside/CLHEP::um);
          sensitive->print("+++ G4Track:%5d CREATE hit[%03d]:%3d deps E:"
                           " %.2e keV Pos:%7.2f %7.2f %7.2f [mm] Start:%s%s%s%s End:%s%s%s%s",
                           pre.truth.trackID,int(collection->GetSize()),
                           combined,pre.truth.deposit/CLHEP::keV,
                           pos.X()/CLHEP::mm,pos.Y()/CLHEP::mm,pos.Z()/CLHEP::mm,
                           ((hit_flag&Geant4Tracker::Hit::HIT_STARTED_SURFACE) ? "SURFACE" : ""),
                           ((hit_flag&Geant4Tracker::Hit::HIT_STARTED_OUTSIDE) ? "OUTSIDE" : ""),
                           ((hit_flag&Geant4Tracker::Hit::HIT_STARTED_INSIDE)  ? "INSIDE " : ""),
                           dist_in,
                           ((hit_flag&Geant4Tracker::Hit::HIT_ENDED_SURFACE)   ? "SURFACE" : ""),
                           ((hit_flag&Geant4Tracker::Hit::HIT_ENDED_OUTSIDE)   ? "OUTSIDE" : ""),
                           ((hit_flag&Geant4Tracker::Hit::HIT_ENDED_INSIDE)    ? "INSIDE " : ""),
                           dist_out);
          collection->add(hit);
        }
        clear();
      }

      /// Method for generating hit(s) using the information of G4Step object.
      G4bool process(const G4Step* step, G4TouchableHistory* )   {
        Geant4StepHandler h(step);
        if( DEBUG == printLevel() ) {
          std::cout<<" DEBUG: Geant4TrackerWeightedSD::process(const G4Step* step, G4TouchableHistory* ) ...."<<std::endl;
          dumpStep( h, step);
        }

        // std::cout << " process called - pre pos: " << h.prePos() << " post pos " << h.postPos() 
        // 	  << " edep: " << h.deposit() << std::endl ;

        G4VSolid*     preSolid    = h.solid(h.pre);
        G4VSolid*     postSolid   = h.solid(h.post);
        G4ThreeVector local_pre   = h.globalToLocalG4(h.prePosG4());
        G4ThreeVector local_post  = h.globalToLocalG4(h.postPosG4());
        EInside       pre_inside  = preSolid->Inside(local_pre);
        EInside       post_inside = postSolid->Inside(local_post);
     
        const void* postPV = h.postVolume();
        const void* prePV  = h.preVolume();
        const void* postSD = h.postSD();
        const void* preSD  = h.preSD();
        G4VSolid* solid = (preSD == thisSD) ? preSolid : postSolid;
        // Track went into new Volume, extracted the hit in prePV, then start a new hit in thisPV.
        if ( current == h.trkID() && thisPV != 0 && prePV != thisPV )  {
          if( DEBUG == printLevel() ) {
            std::cout<<" DEBUG: Geant4TrackerWeightedSD: if ( current == h.trkID() && thisPV != 0 && prePV != thisPV ),"
                     <<" Track went into new Volume, extracted the hit in prePV, then start a new hit in thisPV."
                     << std::endl;
          }
          extractHit(post_inside);
          start(step, h.pre);
        }
        // 1) Track killed inside SD: trace incomplete. This deposition must be added as well.
        else if ( current == h.trkID() && !h.trkAlive() )  {
          hit_flag |= Geant4Tracker::Hit::HIT_KILLED_TRACK;
          update(step).calc_dist_out(solid).extractHit(post_inside);
          return true;
        }
        // 2) Track leaving SD volume. Sensitive detector changed. Store hit.
        else if ( current == h.trkID() && postSD != thisSD )  {
          update(step).calc_dist_out(solid).extractHit(kOutside);
          return true;
        }
        // 3) Track leaving SD volume. Store hit.
        else if ( current == h.trkID() && postSD == thisSD && post_inside == kSurface )  {
          update(step).calc_dist_out(solid).extractHit(kSurface);
          return true;
        }
        // 4) Track leaving SD volume. Store hit.
        else if ( current == h.trkID() && postSD == thisSD && post_inside == kOutside )  {
          update(step).calc_dist_out(solid).extractHit(post_inside);
          return true;
        }
        // 5) Normal update: either intermediate deposition or track is going to be killed.
        else if ( current == h.trkID() && postSD == thisSD && post_inside == kInside )  {
          last_inside = post_inside;
          update(step).calc_dist_out(solid);
          return true;
        }
        // 6) Track from secondary created in SD volume. Store hit from previous. 
        // --> New hit will be created, to whom also this deposition belongs
        else if ( current != h.trkID() && current >= 0 )  {
          extractHit(last_inside);
        }

        // If the hit got extracted, a new one must be set up
        if ( current < 0 )  {
          EInside  inside  = pre_inside;
          // Track entering SD volume
          if ( preSD != thisSD )   {
            start(step, h.post);
            inside = post_inside;
            sensitive->print("++++++++++ Using POST step volume to start hit -- dubious ?");
          }
          else  {
            start(step, h.pre);
          }
          calc_dist_in(solid);
          if ( inside == kSurface )
            hit_flag |= Geant4Tracker::Hit::HIT_STARTED_SURFACE;
          else if ( inside == kInside )
            hit_flag |= Geant4Tracker::Hit::HIT_STARTED_INSIDE;
          else if ( inside == kOutside )
            hit_flag |= Geant4Tracker::Hit::HIT_STARTED_OUTSIDE;
          
          // New (secondary) track created by some process starting inside the volume
          if ( inside == kInside )  {
            hit_flag |= Geant4Tracker::Hit::HIT_SECONDARY_TRACK;
          }
        }

        // Update Data
        last_inside = post_inside;
        update(step);
        calc_dist_out(solid);

        // Track killed inside SD: trace incomplete. This deposition must be added as well.
        if ( !h.trkAlive() )  {
          hit_flag |= Geant4Tracker::Hit::HIT_KILLED_TRACK;
          extractHit(post_inside);
        }
        // Avoid danglich hits if the track leaves the sensitive volume
        else if ( post_inside == kSurface )  {
          extractHit(post_inside);
        }
        // Avoid danglich hits if the track leaves the sensitive volume
        else if ( thisSD == preSD && (preSD != postSD || prePV != postPV) )  {
          extractHit(post_inside);
        }
        // This should simply not happen!
        else if ( thisSD == postSD && (preSD != postSD || prePV != postPV) )  {
          sensitive->error("+++++ WRONG!!! Extract. How did we get here?");
          extractHit(post_inside);
        }
        // In single hit mode we MUST write the hit after update
        else if ( single_deposit_mode )  {
          extractHit(post_inside);
        }

        return true;
      }

      /// Post-event action callback
      void endEvent()   {
        // We need to add the possibly last added hit to the collection here.
        // otherwise the last hit would be assigned to the next event and the
        // MC truth would be screwed.
        //
        if ( current > 0 )   {
          Geant4HitCollection* coll = sensitive->collection(0);
          sensitive->print("++++++++++ Found dangling hit: Is the hit extraction logic correct?");
          extractHit(coll,last_inside);
        }
      }
      /// Pre event action callback
      void startEvent()   {
        thisSD = dynamic_cast<G4VSensitiveDetector*>(&sensitive->detector());
      }

      ///dumpStep
      void dumpStep(const Geant4StepHandler& h, const G4Step* s)  {
        std::stringstream str;
        str << " ----- step in detector " << h.sdName( s->GetPreStepPoint() )
            << " prePos  " << h.prePos()
            << " postPos " << h.postPos()
            << " preStatus  " << h.preStepStatus()
            << " postStatus  " << h.postStepStatus()
            << " preVolume " << h.volName( s->GetPreStepPoint() )
            << " postVolume " << h.volName( s->GetPostStepPoint() )
            << std::endl
            << "     momentum : "  << std::scientific
            <<  s->GetPreStepPoint()->GetMomentum()[0] << ", "
            <<  s->GetPreStepPoint()->GetMomentum()[1]<< ", "
            <<  s->GetPreStepPoint()->GetMomentum()[2]
            << " / "
            << s->GetPostStepPoint()->GetMomentum()[0] << ", "
            <<  s->GetPostStepPoint()->GetMomentum()[1] << ", "
            <<  s->GetPostStepPoint()->GetMomentum()[2]
            << ", PDG: " << s->GetTrack()->GetDefinition()->GetPDGEncoding();
        std::cout << str.str() << std::endl;
      }

      /// GFLash processing callback
      G4bool process(G4GFlashSpot* , G4TouchableHistory* ) {
        sensitive->except("GFlash action is not implemented for SD: %s", sensitive->c_name());
        return false;
      }
    };

    /// Initialization overload for specialization
    template <> void Geant4SensitiveAction<TrackerWeighted>::initialize() {
      declareProperty("HitPositionCombination", m_userData.hit_position_type);
      declareProperty("CollectSingleDeposits", m_userData.single_deposit_mode);
      m_userData.e_cut = m_sensitive.energyCutoff();
      m_userData.sensitive = this;
    }

    /// G4VSensitiveDetector interface: Method invoked at the begining of each event.
    template <> void Geant4SensitiveAction<TrackerWeighted>::begin(G4HCofThisEvent* /* hce */)   {
      m_userData.startEvent();
    }

    /// G4VSensitiveDetector interface: Method invoked at the end of each event.
    template <> void Geant4SensitiveAction<TrackerWeighted>::end(G4HCofThisEvent* /* hce */)   {
      m_userData.endEvent();
    }

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<TrackerWeighted>::defineCollections() {
      m_collectionID = declareReadoutFilteredCollection<Geant4Tracker::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> void Geant4SensitiveAction<TrackerWeighted>::clear(G4HCofThisEvent* /* hce */) {
      m_userData.clear();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> G4bool
    Geant4SensitiveAction<TrackerWeighted>::process(G4Step* step, G4TouchableHistory* history) {
      return m_userData.process(step, history);
    }
    typedef Geant4SensitiveAction<TrackerWeighted>  Geant4TrackerWeightedAction;
  }
}

using namespace dd4hep::sim;

#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE(Geant4TrackerWeightedAction)
