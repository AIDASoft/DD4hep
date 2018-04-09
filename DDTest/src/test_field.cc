#include "DD4hep/DDTest.h"

#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"

#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

using namespace std ;
using namespace dd4hep ;

// this should be the first line in your test
static DDTest test( "field" ) ; 

//=============================================================================

int main(int /* argc */, char** /* argv */ ){

  // ----- detector field test 

  test.log( "field test" );
    
  try{
    
    // ----- write your tests in here -------------------------------------

    Detector& det = Detector::getInstance();

    //computing geometry bfield
    double bfield(0.0);
    const double position[3]={0,0,0}; // position to calculate magnetic field at (the origin in this case)
    double magneticFieldVector[3]={0,0,0}; // initialise object to hold magnetic field
    det.field().magneticField(position,magneticFieldVector); // get the magnetic field vector from DD4hep
    bfield = magneticFieldVector[2]/dd4hep::tesla; // z component at (0,0,0)

    test( det.field().isValid(), "The detector has valid field" );

    if ( det.field().isValid() ) {

      // the most important thing is that bfield is not zero
      test( bfield != 0.0, "The field has non-zero value" );

      // if is needed, it is also possible to test a specific value of bfield (in Tesla)
      //test( bfield, 4.0, "The field has value 4.0" );

    }

    // --------------------------------------------------------------------


  } catch( exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
