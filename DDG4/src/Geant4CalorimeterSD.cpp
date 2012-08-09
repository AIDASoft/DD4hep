// $Id:$
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
#include "DD4hep/Objects.h"

/*
 *   DD4hep::Simulation namespace declaration
 */
namespace DD4hep {  namespace Simulation {
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ///               Geant4GenericSD<Calorimeter>
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    struct Calorimeter {};
    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool Geant4GenericSD<Calorimeter>::buildHits(G4Step* step,G4TouchableHistory*) {
      G4StepPoint*    pre     = step->GetPreStepPoint();
      G4StepPoint*    post    = step->GetPostStepPoint();
      G4ThreeVector   pos     = 0.5 * (pre->GetPosition() + post->GetPosition());
      HitContribution contrib = Geant4Hit::extractContribution(step);
      Position        position(pos.x(),pos.y(),pos.z());
      Geant4CalorimeterHit* hit=find(collection(0),HitPositionCompare<Geant4CalorimeterHit>(position));

      if ( !hit ) {
	collection(0)->insert(hit=new Geant4CalorimeterHit(position));
      }
      hit->truth.push_back(contrib);
      hit->energyDeposit += contrib.deposit;
      return true;
    }
    typedef Geant4GenericSD<Calorimeter> Geant4CalorimeterSD;
}}    // End namespace DD4hep::Simulation

DECLARE_GEANT4SENSITIVEDETECTOR(Geant4CalorimeterSD);
