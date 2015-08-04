#include "DD4hep/DDTest.h"
#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

#include "DDSegmentation/BitField64.h"


using namespace std ;
using namespace DD4hep ;
using namespace DDSegmentation ;

// this should be the first line in your test
DDTest test = DDTest( "bitfield64" ) ; 

//=============================================================================

int main(int /* argc */, char** /* argv */ ){
    
  try{
    
    // ----- write your tests in here -------------------------------------

    test.log( "test bitfield64" );


    // initialize with a string that uses all 64 bits :
    BitField64 bf("system:5,side:-2,layer:9,module:8,sensor:8,x:32:-16,y:-16" ) ;

    BitField64 bf2( bf.fieldDescription() ) ;
    BitField64 bf3( bf.fieldDescription() ) ;


    test(  bf.getValue() , long64(0x0) , " initialized with 0 " ); 

    //    std::cout  << " bf value : " << bf << std::endl ;
    
    bf.setValue( 0xbebafecacafebabe ) ;

    //    std::cout  << " bf value : " << bf << std::endl ;

    test(  bf.getValue() , long64( 0xbebafecacafebabeUL ) , 
	   " initialized with 0xbebafecacafebabeUL - compare as signed " ); 

    test(  (unsigned long) bf.getValue()   , 0xbebafecacafebabeUL  , 
	   " initialized with 0xbebafecacafebabeUL - compare as unsigned " ); 


    // set some 'random' values to bf2 

    bf2["layer"]   = 373 ;
    bf2["module"]  = 254 ;
    bf2["sensor"]  = 202 ;
    bf2["side"]    = 1 ;
    bf2["system"]  = 30 ;
    bf2["x"]       = -310 ;
    bf2["y"]       = -16710 ;


    test(  bf.getValue() , bf2.getValue()  , " same value 0xbebafecacafebabeUL from individual initialization " ); 

    // check for setting high and low words indiviually :

    bf3.setValue(  bf.lowWord() , bf.highWord() ) ; 

    test(  bf3.getValue() , bf2.getValue()  , " same value 0xbebafecacafebabeUL from stting low and hiigh word " ); 


    // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
