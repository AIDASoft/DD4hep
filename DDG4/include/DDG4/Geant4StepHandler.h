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
#ifndef DDG4_GEANT4STEPHANDLER_H
#define DDG4_GEANT4STEPHANDLER_H

// Framework include files
#include <DDG4/Geant4HitHandler.h>

// Geant4 include files
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VTouchable.hh"
#include "G4VSensitiveDetector.hh"
#include "G4EmSaturation.hh"
#include "G4Version.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

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
    class Geant4StepHandler : public Geant4HitHandler  {
    public:
      const G4Step* step;
      G4StepPoint*  pre;
      G4StepPoint*  post;
      bool applyBirksLaw;
      /// Inhibit default constructor
      Geant4StepHandler() = delete;
      /// Initializing constructor
      Geant4StepHandler(const G4Step* s)
	: Geant4HitHandler(s->GetTrack(), (s->GetPreStepPoint()->GetTouchableHandle())()),
	step(s)
      {
        pre = s->GetPreStepPoint();
        post = s->GetPostStepPoint();
        applyBirksLaw = false;
      }
      /// No copy constructor
      Geant4StepHandler(const Geant4StepHandler& copy) = delete;
      /// No move constructor
      Geant4StepHandler(Geant4StepHandler&& copy) = delete;
      /// Assignment operator inhibited. Should not be copied
      Geant4StepHandler& operator=(const Geant4StepHandler& copy) = delete;
      /// Move operator inhibited. Should not be copied
      Geant4StepHandler& operator=(Geant4StepHandler&& copy) = delete;
      
      /// Returns the step status (argument) in form of a string
      static const char* stepStatus(G4StepStatus status);
      /// Returns the pre-step status in form of a string
      const char* preStepStatus() const;
      /// Returns the post-step status in form of a string
      const char* postStepStatus() const;
      /// Returns total energy deposit
      double totalEnergy() const  {
        if(applyBirksLaw == true)
          return this->birkAttenuation();
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
      G4ThreeVector avgPositionG4() const  {
        const G4ThreeVector& p1 = pre->GetPosition();
        const G4ThreeVector& p2 = post->GetPosition();
        G4ThreeVector r = 0.5*(p2+p1);
        return r;
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
      double deposit() const  {
        return step->GetTotalEnergyDeposit();
      }
      double stepLength() const  {
        return step->GetStepLength();
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
      G4VSolid* solid(const G4StepPoint* p) const {
        return p->GetTouchableHandle()->GetSolid();
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
      bool isSensitive(const G4StepPoint* point) const {
        return point ? this->Geant4HitHandler::isSensitive(volume(point)) : false;
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
      /// Apply BirksLaw
      double birkAttenuation() const;
      /// Set applyBirksLaw to ture
      void doApplyBirksLaw(void) {
        applyBirksLaw = true;
      }
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4STEPHANDLER_H
