#include "DD4hep/DDTest.h"
#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

//=============================================================================
int main(int /* argc */, char** /* argv */ ){

  // this should be the first line in your test
  dd4hep::DDTest test( "example" );
    
  try{
    
    // ----- write your tests in here -------------------------------------

    test.log( "example test" );

    // ----- example test for testing two expressions for equality:

    test( "Example", "Example", "example test - string comparison " ); // this test will pass

    //test( "Example", "BadExample", "example test - string comparison " ); //  this test will fail

    double pi_d = M_PI ;
    float  pi_f = M_PI ;
    // ----- example for test of an expresssion - not using equality

    test(  1.1 * pi_f > pi_d   , " example comparison:  1.1* float(M_PI)  > double( M_PI) " ) ;  

    // ... guess which of these will pass ...
    // test(  pi_f == pi_d  , " example comparison:  float(M_PI) == double( M_PI) " ) ;  
    // test(  pi_f <  pi_d  , " example comparison:  float(M_PI)  < double( M_PI) " ) ;  
    // test(  pi_f >  pi_d  , " example comparison:  float(M_PI)  > double( M_PI) " ) ;  

    // --------------------------------------------------------------------
  }
  catch( std::exception &e ){
    test.log( e.what() );
    test.error( "exception occurred" );
  }
  return 0;
}
//=============================================================================
