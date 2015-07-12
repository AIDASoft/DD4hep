// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_GEANT4STEPHANDLER_H
#define DD4HEP_GEANT4STEPHANDLER_H

// Framework include files
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VTouchable.hh"
#include "G4VSensitiveDetector.hh"
#include "G4EmSaturation.hh"
#include "G4Version.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations;
    class Geant4StepHandler;

    /// Helper class to ease the extraction of information from a G4Step object.
    /**
     *
     * Helper/utility class to easily access Geant4 step information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4StepHandler {
    public:
      const G4Step* step;
      G4StepPoint* pre;
      G4StepPoint* post;
      G4Track* track;
      bool applyBirksLaw;
      Geant4StepHandler(const G4Step* s)
        : step(s) {
        pre = s->GetPreStepPoint();
        post = s->GetPostStepPoint();
        track = s->GetTrack();
        applyBirksLaw = false;
      }
      G4ParticleDefinition* trackDef() const {
        return track->GetDefinition();
      }
      int trkPdgID() const {
        return track->GetDefinition()->GetPDGEncoding();
      }
      /// Returns the step status (argument) in form of a string
      static const char* stepStatus(G4StepStatus status);
      /// Returns the pre-step status in form of a string
      const char* preStepStatus() const;
      /// Returns the post-step status in form of a string
      const char* postStepStatus() const;
      /// Returns total energy deposit
      double totalEnergy() const  {
        if(applyBirksLaw == true)
          return BirkAttenuation(step);
        else
          return step->GetTotalEnergyDeposit();
      }
      /// Returns the pre-step position
      Position prePos() const {
        const G4ThreeVector& p = pre->GetPosition();
        return Position(p.x(), p.y(), p.z());
      }
      /// Returns the pre-step position as a G4ThreeVector
      const G4ThreeVector& prePosG4() const {
        return pre->GetPosition();
      }
      /// Returns the post-step position
      Position postPos() const {
        const G4ThreeVector& p = post->GetPosition();
        return Position(p.x(), p.y(), p.z());
      }
      /// Returns the post-step position as a G4ThreeVector
      const G4ThreeVector& postPosG4() const {
        return post->GetPosition();
      }
      /// Average position vector of the step.
      Position avgPosition() const  {
        const G4ThreeVector& p1 = pre->GetPosition();
        const G4ThreeVector& p2 = post->GetPosition();
        G4ThreeVector r = 0.5*(p2+p1);
        return Position(r.x(),r.y(),r.z());
      }
      /// Direction calculated from the post- and pre-position ofthe step
      Position direction()  const  {
        const G4ThreeVector& p1 = pre->GetPosition();
        const G4ThreeVector& p2 = post->GetPosition();
        G4ThreeVector r = (p2-p1);
        double len = r.r();
        if ( len > 1e-15 )  {
          r /= len;
          return Position(r.x(),r.y(),r.z());
        }
        return Position();
      }
      Momentum preMom() const {
        const G4ThreeVector& p = pre->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      Momentum postMom() const {
        const G4ThreeVector& p = post->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      Momentum trkMom() const {
        const G4ThreeVector& p = track->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      double deposit() const  {
        return step->GetTotalEnergyDeposit();
      }
      int trkID() const  {
        return track->GetTrackID();
      }
      double trkTime() const  {
        return track->GetGlobalTime();
      }
      double trkEnergy() const  {
        return track->GetTotalEnergy();
      }
      double trkKineEnergy() const  {
        return track->GetKineticEnergy();
      }
      const G4VTouchable* preTouchable() const {
        return pre->GetTouchable();
      }
      const G4VTouchable* postTouchable() const {
        return post->GetTouchable();
      }
      const char* volName(const G4StepPoint* p, const char* undefined = "") const {
        G4VPhysicalVolume* v = volume(p);
        return v ? v->GetName().c_str() : undefined;
      }
      G4VPhysicalVolume* volume(const G4StepPoint* p) const {
        return p->GetTouchableHandle()->GetVolume();
      }
      G4VPhysicalVolume* physvol(const G4StepPoint* p) const {
        return p->GetPhysicalVolume();
      }
      G4LogicalVolume* logvol(const G4StepPoint* p) const {
        G4VPhysicalVolume* pv = physvol(p);
        return pv ? pv->GetLogicalVolume() : 0;
      }
      G4VSensitiveDetector* sd(const G4StepPoint* p) const {
        G4LogicalVolume* lv = logvol(p);
        return lv ? lv->GetSensitiveDetector() : 0;
      }
      const char* sdName(const G4StepPoint* p, const char* undefined = "") const {
        G4VSensitiveDetector* s = sd(p);
        return s ? s->GetName().c_str() : undefined;
      }
      bool isSensitive(const G4LogicalVolume* lv) const {
        return lv ? (0 != lv->GetSensitiveDetector()) : false;
      }
      bool isSensitive(const G4VPhysicalVolume* pv) const {
        return pv ? isSensitive(pv->GetLogicalVolume()) : false;
      }
      bool isSensitive(const G4StepPoint* point) const {
        return point ? isSensitive(volume(point)) : false;
      }
      G4VPhysicalVolume* preVolume() const {
        return volume(pre);
      }
      G4VSensitiveDetector* preSD() const {
        return sd(pre);
      }
      G4VPhysicalVolume* postVolume() const {
        return volume(post);
      }
      G4VSensitiveDetector* postSD() const {
        return sd(post);
      }
      bool firstInVolume() const  {
        return step->IsFirstStepInVolume();
      }
      bool lastInVolume() const  {
        return step->IsLastStepInVolume();
      }
      /// Coordinate transformation to global coordinates.
      /** Note: Positions are in units of MM! */
      Position localToGlobal(const Position& local)  const;
      /// Coordinate transformation to global coordinates.
      /** Note: DDSegmentation points are units in CM! Conversion done inside! */
      Position localToGlobal(const DDSegmentation::Vector3D& local)  const;
      /// Coordinate transformation to global coordinates in MM
      Position localToGlobal(const G4ThreeVector& local)  const;
      /// Coordinate transformation to global coordinates in MM
      Position localToGlobal(double x, double y, double z)  const;
      /// Apply BirksLaw
      double BirkAttenuation(const G4Step* aStep) const
      {
        double energyDeposition = aStep->GetTotalEnergyDeposit();
        double length = aStep->GetStepLength();
        double niel   = aStep->GetNonIonizingEnergyDeposit();
        const G4Track* trk = aStep->GetTrack();
        const G4ParticleDefinition* particle = trk->GetDefinition();
        const G4MaterialCutsCouple* couple = trk->GetMaterialCutsCouple();
#if G4VERSION_NUMBER >= 1001
        G4EmSaturation* emSaturation = new G4EmSaturation(0);
#else
        G4EmSaturation* emSaturation = new G4EmSaturation();
#endif
        double engyVis = emSaturation->VisibleEnergyDeposition(particle,
                                                               couple,
                                                               length,
                                                               energyDeposition,
                                                               niel);
        delete emSaturation; 
        return engyVis;
      }
      /// Set applyBirksLaw to ture
      void doApplyBirksLaw(void) {
        applyBirksLaw = true;
      }
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4STEPHANDLER_H
