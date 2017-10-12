#include "DD4hep/DDTest.h"
#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

#include "DDSegmentation/BitFieldCoder.h"


using namespace std ;
using namespace dd4hep ;
using namespace DDSegmentation ;

// this should be the first line in your test
static DDTest test( "bitfield64" ) ; 

//=============================================================================

int main(int /* argc */, char** /* argv */ ){
    
  try{
    
    // ----- write your tests in here -------------------------------------

    test.log( "test bitfieldcoder" );


    // initialize with a string that uses all 64 bits :
    const BitFieldCoder bf("system:5,side:-2,layer:9,module:8,sensor:8,x:32:-16,y:-16" ) ;

    // set some 'random' values to bf2 
    long64 field = 0  ;
    
    bf.set( field, "layer",  373 );
    bf.set( field, "module", 254 );
    bf.set( field, "sensor", 202 );
    bf.set( field, "side",   1 );
    bf.set( field, "system", 30 );
    bf.set( field, "x",      -310 );
    bf.set( field, "y",      -16710 );


    test(  field , long64(0xbebafecacafebabeUL)  , " same value 0xbebafecacafebabeUL from individual initialization " ); 


    // make a copy for testing the access
    const BitFieldCoder bf2 = bf ;

    test( bf2.get( field, "layer") ,  373 , " acces field value: layer" );
    test( bf2.get( field, "module"),  254 , " acces field value: module" );
    test( bf2.get( field, "sensor"),  202 , " acces field value: sensor" );
    test( bf2.get( field, "side"),    1   , " acces field value: side" );
    test( bf2.get( field, "system"),  30  , " acces field value: system" );
    test( bf2.get( field, "x"),      -310 , " acces field value: x" );
    test( bf2.get( field, "y"),    -16710 , " acces field value: y" );




    test( bf2.get( field, bf2.index( "layer")) ,  373 , " acces field value: layer" );
    test( bf2.get( field, bf2.index( "module")),  254 , " acces field value: module" );
    test( bf2.get( field, bf2.index( "sensor")),  202 , " acces field value: sensor" );
    test( bf2.get( field, bf2.index( "side")),    1   , " acces field value: side" );
    test( bf2.get( field, bf2.index( "system")),  30  , " acces field value: system" );
    test( bf2.get( field, bf2.index( "x")),      -310 , " acces field value: x" );
    test( bf2.get( field, bf2.index( "y")),    -16710 , " acces field value: y" );


    // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
