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

    //std::cout << "Build type: " << det.buildType()<< std::endl;

    //check if a VALID geometry was created and check if the bfield is valid
    if(det.buildType() == BUILD_NONE){

      test( det.field().isValid(), 0, "The geometry is not valid and has invalid field!" );

    } else {

      test( det.field().isValid(), "The geometry is valid and has valid field!" );

    }

  } catch( exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
