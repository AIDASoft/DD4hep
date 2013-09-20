// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4SensitiveDetector_inline.h"
#include "DDG4/Factories.h"

/*
 *   DD4hep::Simulation namespace declaration
 */
namespace DD4hep {  namespace Simulation {

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<TrackerCombine>
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
	return hit;
      }
    };

    /// Method invoked at the begining of each event. 
    template <> void Geant4GenericSD<TrackerCombine>::Initialize(G4HCofThisEvent* HCE) {
      userData.e_cut = m_sensitive.energyCutoff();
      this->Geant4SensitiveDetector::Initialize(HCE);
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> void Geant4GenericSD<TrackerCombine>::clear() {
      userData.clear();
      this->Geant4SensitiveDetector::clear();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> G4bool Geant4GenericSD<TrackerCombine>::ProcessHits(G4Step* step,G4TouchableHistory* ) {
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
    //    typedef Geant4GenericSD<TrackerCombine>  Geant4TrackerCombine;
    typedef SimpleG4SDFactory< Geant4GenericSD<TrackerCombine> >  Geant4TrackerCombine;
}}    // End namespace DD4hep::Simulation

DECLARE_GEANT4SENSITIVEDETECTOR(Geant4TrackerCombine)
