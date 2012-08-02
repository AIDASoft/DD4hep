// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4SENSITIVEDETECTOR_H
#define DD4HEP_GEANT4SENSITIVEDETECTOR_H

// Framework include files
#include "DD4hep/Readout.h"
#include "DD4hep/Detector.h"

// Geant 4 include files
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VSensitiveDetector.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  namespace Geometry {
    struct LCDD;
  };

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    struct Geant4SensitiveDetector : public G4VSensitiveDetector  {
      
      typedef Geometry::LCDD       LCDD;
      typedef Geometry::Readout    Readout;
      typedef Geometry::DetElement DetElement;

      /// Reference to the detector description object
      LCDD&      m_lcdd;
      /// Reference to the detector element describing this sensitive element
      DetElement m_detector;
      /// Reference to the readout structure
      Readout    m_readout;

      public:
      /// Constructor
      Geant4SensitiveDetector(const std::string& name, DetElement detector, LCDD& lcdd);

      /// Standard destructor
      virtual ~Geant4SensitiveDetector();

      /** Method invoked at the begining of each event. 
       *  The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void Initialize(G4HCofThisEvent* HCE);
      /// Method invoked at the end of each event. 
      virtual void EndOfEvent(G4HCofThisEvent* HCE);

      /// Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist) = 0;

      /// Method is invoked if the event abortion is occured.
      virtual void clear();
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4SENSITIVEDETECTOR_H
