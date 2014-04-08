#include "DD4hep/LCDD.h"
#include "DD4hep/TGeoUnits.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"

#include "DDRec/Surface.h"

// // #include "TGeoManager.h"
// //---- GEAR ----
// #include "gear/GEAR.h"
// #include "gearxml/GearXML.h"
// #include "gearimpl/Util.h"
// #include "gearimpl/ConstantBField.h"
// #include "gearimpl/GearMgrImpl.h"
// #include "geartgeo/TGeoGearPointProperties.h"
// #include "geartgeo/TGeoGearDistanceProperties.h"

#include <list>

using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec ;
using namespace DDSurfaces ;

using namespace tgeo ;

//=============================================================================

int main(int argc, char** argv ){
  
  if( argc < 2 ) {
    std::cout << " usage: test_surfaces compact.xml " << std::endl ;
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );

  //  gear::GearMgr* gearMgr = createGearMgr( lcdd ) ;


  DetElement world = lcdd.world() ;

  std::cout << " **** detector "  <<  lcdd.header().name() << " to Gear \n"
	    << "      Iterating over all DetElements: " << std::endl ;

  
  //------------------ breadth first tree traversal ---------
  std::list< DetElement > dets ;
  std::list< DetElement > daugs ; 
  std::list< DetElement > gdaugs ; 
  
  daugs.push_back( world ) ;
  
  while( ! daugs.empty() ) {
  
    for( std::list< DetElement >::iterator li=daugs.begin() ; li != daugs.end() ; ++li ){
      DetElement dau = *li ;
      DetElement::Children chMap = dau.children() ;
      for ( DetElement::Children::const_iterator it=chMap.begin() ; it != chMap.end() ; ++it ){
	DetElement de = (*it).second ;
	gdaugs.push_back( de ) ;
      }  
    }
    dets.splice( dets.end() , daugs ) ;
    daugs.splice( daugs.end() , gdaugs ) ;
  }
  //------------------ end tree traversal ---------
  
  std::cout << " **** # DetElements found  " << dets.size() << std::endl ;
  
  for( std::list< DetElement >::iterator li=dets.begin() ; li != dets.end() ; ++li ) {
    
    DetElement det = (*li) ;
    
    if( ! volSurfaceList(det)->empty() ) {

      std::cout <<  "     detector  " << det.name() << " id: " << det.id() << " has " <<  volSurfaceList(det)->size() << " surfaces "  << std::endl ; 

 
      // take first surface for now :
      VolSurface volSurf =  volSurfaceList(det)->front() ;
      
      Surface* surf = new Surface(  det,  volSurf ) ;

      std::cout << " ------------------------- " 
		<< " surface: " << *surf          << std::endl
		<< " ------------------------- "  << std::endl ;



      
    }

  }



  return 0;
}

//=============================================================================
