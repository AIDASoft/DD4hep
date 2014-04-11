#include "DD4hep/LCDD.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorSurfaces.h"
#include "DDRec/SurfaceManager.h"

// #include "DD4hep/TGeoUnits.h"
// #include "DD4hep/Detector.h"
// #include "DD4hep/Volumes.h"


using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec ;
using namespace DDSurfaces ;

//using namespace tgeo ;

//=============================================================================

int main(int argc, char** argv ){
  
  if( argc < 2 ) {
    std::cout << " usage: test_surfaces compact.xml " << std::endl ;
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );


  DetElement world = lcdd.world() ;


  // create a list of all surfaces in the detector:
  SurfaceManager surfMan(  world ) ;

  const SurfaceList& sL = surfMan.surfaceList() ;
  
  
  for( SurfaceList::const_iterator it = sL.begin() ; it != sL.end() ; ++it ){
    
    Surface& surf =  **it ;
    
    std::cout << " ------------------------- " 
	      << " surface: "  << surf          << std::endl
	      << " ------------------------- "  << std::endl ;
  }

  return 0;
}

//=============================================================================
