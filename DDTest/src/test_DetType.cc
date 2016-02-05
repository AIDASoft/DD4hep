#include "DD4hep/DDTest.h"
#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

#include "DD4hep/DetType.h"


using namespace std ;
using namespace DD4hep ;

// this should be the first line in your test
DDTest test = DDTest( "bitfield64" ) ; 

//=============================================================================

int main(int /* argc */, char** /* argv */ ){
    
  try{
    
    // ----- write your tests in here -------------------------------------

    test.log( "test DetType" );
    
    DetType type( DetType::TRACKER | DetType::STRIP | DetType::BARREL ) ;

    std::cout << type << std::endl ;

    test(  type.is( DetType::TRACKER ) ,  true , " is DetType::TRACKER " ) ; 

    test(  type.is( DetType::STRIP | DetType::BARREL ) ,  true , 
	   " is DetType::STRIP | DetType::BARREL" ) ; 

    test(  type.is( DetType::CALORIMETER ) ,  false , " DetType::CALORIMETER is false " ) ; 

    test(  type.isNot( DetType::SUPPORT | DetType::CHERENKOV ) ,  true , 
	   " is not DetType::SUPPORT | DetType::CHERENKOV " ) ;


    type.unset( DetType::STRIP | DetType::BARREL  )  ;
    
    std::cout << type << std::endl ;
    
    type.set( DetType::PIXEL | DetType::ENDCAP | DetType::VERTEX  )  ;
    
    std::cout << type << std::endl ;
   
    test(  type.isNot( DetType::STRIP | DetType::BARREL ) ,  true , 
	   " is not DetType::STRIP | DetType::BARREL " ) ;
    

    // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
