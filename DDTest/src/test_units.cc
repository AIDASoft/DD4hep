#include "DD4hep/DDTest.h"

#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"
#include "STR.h"

#include <exception>
#include <iostream>
#include <assert.h>
#include <cmath>

using namespace std ;
using namespace dd4hep ;
using namespace detail;
using namespace dd4hep ;

// this should be the first line in your test
static DDTest test( "units" ) ; 

//=============================================================================
//=============================================================================

int main(int argc, char** argv ){
    
  test.log( "test units" );
  
  if( argc < 2 ) {
    std::cout << " usage:  test_units units.xml " << std::endl ;
    exit(1) ;
  }

  try{
    
    // ----- write your tests in here -------------------------------------

    Detector& description = Detector::getInstance();

    description.fromCompact( argv[1] );


    // ======= deg and rad:
    double al0_rad = description.constant<double>( "alpha_half_arc_deg" )  / rad ;
    double al1_rad = description.constant<double>( "alpha_half_arc_rad" )  / rad ;

    test( STR(al0_rad), STR(al1_rad) , " alpha_half_arc in rad " ) ; 

    double al0_deg = description.constant<double>( "alpha_half_arc_deg" )  / deg ;
    double al1_deg = description.constant<double>( "alpha_half_arc_rad" )  / deg ;

    test( STR( al0_deg ) , STR( 180. )  , " alpha_half_arc_deg in deg is 180." ) ; 
    test( STR( al1_deg ) , STR( 180. )  , " alpha_half_arc_rad in deg is 180." ) ; 


    test( STR(al0_deg), STR(al1_deg) , " alpha_half_arc in deg " ) ; 


    //======== densities =========
    double rho1 = description.constant<double>( "rho1" )  / g * cm3 ;
    double rho2 = description.constant<double>( "rho2" )  / g * cm3 ;

    test( STR(rho1), STR(rho2) , " densities in [g/cm3]  " ) ; 

    rho1 = description.constant<double>( "rho1" )  / kg * m3 ;
    rho2 = description.constant<double>( "rho2" )  / kg * m3 ;
    
    test( STR(rho1), STR(rho2) , " densities in [kg/cm3]  " ) ; 


    // --------------------------------------------------------------------


  } catch( exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

  return 0;
}

//=============================================================================
