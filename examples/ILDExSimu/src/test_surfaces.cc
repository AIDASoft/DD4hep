#include "DD4hep/LCDD.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorSurfaces.h"
#include "DDRec/SurfaceManager.h"
#include "DD4hep/DDTest.h"

#include "DD4hep/TGeoUnits.h"

#include "lcio.h"
#include "IO/LCReader.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimTrackerHit.h"
#include "UTIL/ILDConf.h"

#include <map>
#include <sstream>

using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec ;
using namespace DDSurfaces ;
using namespace lcio;


//using namespace tgeo ;
DDTest test = DDTest( "surfaces" ) ; 

//=============================================================================

int main(int argc, char** argv ){
  
  if( argc < 3 ) {
    std::cout << " usage: test_surfaces compact.xml lcio_file.slcio" << std::endl ;
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );


  DetElement world = lcdd.world() ;


  // create a list of all surfaces in the detector:
  SurfaceManager surfMan(  world ) ;

  const SurfaceList& sL = surfMan.surfaceList() ;

  // map of surfaces
  std::map< long64, Surface* > surfMap ;

  for( SurfaceList::const_iterator it = sL.begin() ; it != sL.end() ; ++it ){
    
    Surface* surf =  *it ;
    
    // std::cout << " ------------------------- " 
    // 	      << " surface: "  << *surf          << std::endl
    // 	      << " ------------------------- "  << std::endl ;

    
    surfMap[ surf->id() ] = surf ;

  }

  //---------------------------------------------------------------------
  //    open lcio file (created with ILDExSimu)
  //---------------------------------------------------------------------

  std::string lcioFileName = argv[2] ;

  LCReader* rdr = LCFactory::getInstance()->createLCReader() ;
  rdr->open( lcioFileName ) ;

  LCEvent* evt = 0 ;

  UTIL::BitField64 idDecoder( ILDCellID0::encoder_string ) ;


  std::vector< std::string > colNames ;
  colNames.push_back( "VXDCollection" ) ;
  colNames.push_back( "SITCollection" ) ;

  while( ( evt = rdr->readNextEvent() ) != 0 ){


    for(unsigned icol=0, ncol = colNames.size() ; icol < ncol ; ++icol ){

      LCCollection* col = evt->getCollection( colNames[ icol ] ) ;

      int nHit = col->getNumberOfElements() ;
      

      for(int i=0 ; i< nHit ; ++i){
	
	SimTrackerHit* sHit = (SimTrackerHit*) col->getElementAt(i) ;
	
	long64 id = sHit->getCellID0() ;
	
	idDecoder.setValue( id ) ;
	//      std::cout << " simhit with cellid : " << idDecoder << std::endl ;
	
	Surface* surf = surfMap[ id ] ;
	
	std::stringstream sst ;
	sst << " surface found for id : " << std::hex << id << std::dec  ;
	
	
	// ===== test that we have a surface with the correct ID for every hit ======================
	
	test( surf != 0 , true , sst.str() ) ; 
	

	if( surf != 0 ){
	  
	  Vector3D point( sHit->getPosition()[0]* tgeo::mm , sHit->getPosition()[1]* tgeo::mm ,  sHit->getPosition()[2]* tgeo::mm ) ;
	  
	  double dist = surf->distance( point ) ;
	  
	  bool isInside = surf->insideBounds( point )  ;
	  
	  
	  sst.str("") ;
	  sst << " point " << point << " is on surface " ;
	  
	  // ====== test that hit points are inside their surface ================================
	  
	  test( isInside , true , sst.str() ) ;
	  
	  // std::cout << " found surface with same id " <<  *surf << std::endl 
	  // 	  << " point : " << point 
	  // 	  << " is inside : " <<  isInside
	  // 	  << " distance from surface : " << dist/tgeo::mm << std::endl 
	  // 	  << std::endl ;

	  // ====== test that slightly moved hit points are inside their surface ================================
	  
	  Vector3D point2 = point + 1e-5 * surf->normal() ;
	  sst.str("") ;
	  sst << " point2 " << point2 << " is on surface " ;
	  isInside = surf->insideBounds( point2 )  ;
	  test( isInside , true , sst.str() ) ;
	  
	  // ====== test that moved hit points are outside their surface ================================
	  
	  Vector3D point3 = point + 1e-3 * surf->normal() ;
	  sst.str("") ;
	  sst << " point3 " << point3 << " is not on surface " ;
	  isInside = surf->insideBounds( point3)  ;
	  test( isInside , false , sst.str() ) ;
	  
	  
	  
	} else {
	  
	  std::cout << "ERROR:   no surface found for id: " << idDecoder << std::endl ;
	}
	
      }
      
    }
    
    
  }
  
  return 0;
}

//=============================================================================
