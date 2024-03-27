#include "DD4hep/DDTest.h"

#include "DD4hep/Shapes.h"

#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

//=============================================================================
int main(int /* argc */, char** /* argv */ ){

  // this should be the first line in your test
  dd4hep::DDTest test( "shapes" );
    
  try{
    
    // --------------------------------------------------------------------
    // Polycone

    // add single plane with addZPlanes
    dd4hep::Polycone polycone(0, 2*M_PI, {0, 0}, {1, 1}, {0, 1});
    polycone.addZPlanes({0}, {1}, {2});

    // --------------------------------------------------------------------
  }
  catch( std::exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }
  return 0;
}
//=============================================================================
