// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Simple program to print all the materials in a detector on
//  a straight line between two given points
// 
//  Author     : F.Gaede, DESY
//
//====================================================================
#include "DD4hep/LCDD.h"
#include "DD4hep/TGeoUnits.h"

#include "DDRec/MaterialManager.h"

using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec;
using namespace DDSurfaces ;
using namespace tgeo ;

//=============================================================================

int main(int argc, char** argv ){
    
  if( argc != 8 ) {
    std::cout << " usage: print_materials compact.xml x0 y0 z0 x1 y1 z1 " << std::endl 
	      << "        -> prints the materials on a straight line between the two given points ( unit is cm) "  ;
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;

  std::stringstream sstr ;
  sstr << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] ;

  double x0,y0,z0,x1,y1,z1 ;
  sstr >> x0 ;
  sstr >> y0 ;
  sstr >> z0 ;
  sstr >> x1 ;
  sstr >> y1 ;
  sstr >> z1 ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );

  Vector3D p0( x0, y0, z0 ) ;
  Vector3D p1( x1, y1, z1 ) ;

  MaterialManager matMgr ;

  const MaterialVec& materials = matMgr.materials( p0 , p1  ) ;
	
  std::cout  << std::endl  << " #######  materials between the two  points : " << p0 << "*cm  and " << p1 << "*cm :  "  << std::endl ;

  for( unsigned i=0,n=materials.size();i<n;++i){

    std::cout <<  "      " << materials[i].first << "  \t  thickness: " <<   materials[i].second / tgeo::mm << " mm " << std::endl ;
  }

  std::cout << "############################################################################### "  << std::endl  << std::endl  ;
	
  return 0;
}

//=============================================================================
