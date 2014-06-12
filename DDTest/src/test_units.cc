#include "DD4hep/DDTest.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/TGeoUnits.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <cmath>

using namespace std ;
using namespace DD4hep ;
using namespace Geometry;
using namespace tgeo ;

// this should be the first line in your test
DDTest test = DDTest( "units" ) ; 

//=============================================================================
/** Helper class for floating point comparisons using string representations */
class STR {
  STR() {} 
  float _val ;
  std::string _str ;
public:
  STR ( float val ) : _val( val ){
    std::stringstream s1 ; 
    s1 << _val ;
    _str = s1.str() ;
  }
  std::string str() const { return _str ; }
  
  bool operator==( const STR& s2) const {
    return this->str() == s2.str() ;
  }
};

std::ostream& operator<<(std::ostream& os , const STR& s) {
  os << s.str() ;
  return os ;
} 
//=============================================================================

int main(int argc, char** argv ){
    
  test.log( "test units" );
  
  if( argc < 2 ) {
    std::cout << " usage:  test_units units.xml " << std::endl ;
    exit(1) ;
  }

  try{
    
    // ----- write your tests in here -------------------------------------

    LCDD& lcdd = LCDD::getInstance();

    lcdd.fromCompact( argv[1] );


    // ======= deg and rad:
    double al0_rad = lcdd.constant<double>( "alpha_half_arc_deg" )  / rad ;
    double al1_rad = lcdd.constant<double>( "alpha_half_arc_rad" )  / rad ;

    test( STR(al0_rad), STR(al1_rad) , " alpha_half_arc in rad " ) ; 

    double al0_deg = lcdd.constant<double>( "alpha_half_arc_deg" )  / deg ;
    double al1_deg = lcdd.constant<double>( "alpha_half_arc_rad" )  / deg ;

    test( STR( al0_deg ) , STR( 180. )  , " alpha_half_arc_deg in deg is 180." ) ; 
    test( STR( al1_deg ) , STR( 180. )  , " alpha_half_arc_rad in deg is 180." ) ; 


    test( STR(al0_deg), STR(al1_deg) , " alpha_half_arc in deg " ) ; 


    //======== densities =========
    double rho1 = lcdd.constant<double>( "rho1" )  / g * cm3 ;
    double rho2 = lcdd.constant<double>( "rho2" )  / g * cm3 ;

    test( STR(rho1), STR(rho2) , " densities in [g/cm3]  " ) ; 

    rho1 = lcdd.constant<double>( "rho1" )  / kg * m3 ;
    rho2 = lcdd.constant<double>( "rho2" )  / kg * m3 ;
    
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
