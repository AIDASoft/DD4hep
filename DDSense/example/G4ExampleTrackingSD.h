// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  @Author     : F.Gaede, DESY
//
//====================================================================

#ifndef DD4hep_Simulation_G4ExampleTrackingSD_h
#define DD4hep_Simulation_G4ExampleTrackingSD_h

#include "DDSense/G4SensitiveDetector.h"

#include "G4VSensitiveDetector.hh"

/** DD4hep namespace declaration */
namespace DD4hep {  
  /** DDSense namespace declaration  */  
  namespace Simulation {
    

    /** Trivial example SD thatn does not do anything - just for testing plugin mechanism...
     */
    class G4ExampleTrackingSD : public  G4SensitiveDetector, G4VSensitiveDetector {

    public:
      
      /** c'tor */
      G4ExampleTrackingSD() ;

     virtual G4SensitiveDetector* newInstance() const ;

      /** Implementation classes can overwrite this to initialize from named parameters
       */
      virtual void init( const ParameterMap&  parameters ) ; 

      virtual G4bool ProcessHits(G4Step*aStep, G4TouchableHistory*ROhist) ;

     /** d'tor */
      virtual ~G4ExampleTrackingSD() {}

    private:
       
    } ;
    
  } // namespace
} // namespace

#endif
