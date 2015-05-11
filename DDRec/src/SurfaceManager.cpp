#include "DDRec/SurfaceManager.h"

#include "DDRec/SurfaceHelper.h"
//#include "DDRec/DetectorSurfaces.h"
//#include "DD4hep/Detector.h"
#include "DD4hep/LCDD.h"
//#include "DD4hep/VolumeManager.h"

namespace DD4hep {
  
  using namespace Geometry ;

  namespace DDRec {
    

    SurfaceManager::SurfaceManager(){

      initialize() ;
    }
    
    SurfaceManager::~SurfaceManager(){
      // nothing to do
    }
    
    
    const SurfaceMap* SurfaceManager::map( const std::string name ) const {

      SurfaceMapsMap::const_iterator it = _map.find( name ) ;

      if( it != _map.end() ){

	return & it->second ;
      }

      return 0 ;
    }

    void SurfaceManager::initialize() {
      
      LCDD& lcdd = LCDD::getInstance();

      std::vector<std::string> types = lcdd.detectorTypes() ;

      for(unsigned i=0,N=types.size();i<N;++i){

	const std::vector<DetElement>& dets = lcdd.detectors( types[i] ) ;  

	for(unsigned j=0,M=dets.size();j<M;++j){

	  std::string name = dets[j].name() ;

	  SurfaceHelper surfH( dets[j] ) ;
	  
	  const SurfaceList& detSL = surfH.surfaceList() ;
  
	  for( SurfaceList::const_iterator it = detSL.begin() ; it != detSL.end() ; ++it ){
	    Surface* surf =  *it ;
	    
	    // enter surface into map for detector type
	    _map[ types[i] ].insert( std::make_pair(   surf->id() , surf )  ) ;

	    // enter surface into map for this detector
	    _map[ name ].insert( std::make_pair(   surf->id() , surf )  ) ;

	  }
	}
      }

    }




  } // namespace
}// namespace
