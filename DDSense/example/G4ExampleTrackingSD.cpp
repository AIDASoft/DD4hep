#include "G4ExampleTrackingSD.h"


/** DD4hep namespace declaration */
namespace DD4hep {
  /** DDSense namespace declaration  */
  namespace Simulation  {
    
    
    // global instance
    G4ExampleTrackingSD _theG4ExampleTrackingSD ;


    G4ExampleTrackingSD::G4ExampleTrackingSD() : G4SensitiveDetector("G4ExampleTrackingSD" ), 
      G4VSensitiveDetector("G4ExampleTrackingSD") {}


    G4bool G4ExampleTrackingSD::ProcessHits(G4Step*aStep, G4TouchableHistory*ROhist) { 

      std::cout << "G4ExampleTrackingSD::ProcessHits " << std::endl ;

    } 

    G4SensitiveDetector* G4ExampleTrackingSD::newInstance() const { return new G4ExampleTrackingSD ; } 

      /** Implementation classes can overwrite this to initialize from named parameters
       */
    void G4ExampleTrackingSD::init( const ParameterMap&  parameters ) {

      G4SensitiveDetector::init( parameters ) ;
      
      // do sth. with parameters ...
    }


  } //namespace 
} //namespace 

