#include "DDRec/SurfaceManager.h"

#include "DDRec/SurfaceHelper.h"
#include "DD4hep/VolumeManager.h"

#include <sstream>

namespace dd4hep {
  
  using namespace DDSurfaces ;

  namespace rec {
    


    SurfaceManager::SurfaceManager(LCDD& theDetector){

      // have to make sure the volume manager is populated once in order to have
      // the volumeIDs attached to the DetElements

      VolumeManager::getVolumeManager(theDetector);

      initialize(theDetector) ;
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

    void SurfaceManager::initialize(LCDD& theDetector) {
      
      const std::vector<std::string>& types = theDetector.detectorTypes() ;

      for(unsigned i=0,N=types.size();i<N;++i){

	const std::vector<DetElement>& dets = theDetector.detectors( types[i] ) ;  

	for(unsigned j=0,M=dets.size();j<M;++j){

	  std::string name = dets[j].name() ;

	  SurfaceHelper surfH( dets[j] ) ;
	  
	  const SurfaceList& detSL = surfH.surfaceList() ;
  
	  // add an empty map for this detector in case there are no surfaces attached 
	  _map.insert(  std::make_pair( name , SurfaceMap() ) )  ;

	  for( SurfaceList::const_iterator it = detSL.begin() ; it != detSL.end() ; ++it ){
	    ISurface* surf =  *it ;
	    
	    // enter surface into map for this detector
	    _map[ name ].insert( std::make_pair( surf->id(), surf )  ) ;

	    // enter surface into map for detector type
	    _map[ types[i] ].insert( std::make_pair( surf->id(), surf )  ) ;

	    // enter surface into world map 
	    _map[ "world" ].insert( std::make_pair( surf->id(), surf )  ) ;

	  }
	}
      }

    }

    std::string SurfaceManager::toString() const {
      
      std::stringstream sstr ;
       
      sstr << "--------  SurfaceManager contains the following maps : --------- " << std::endl ;
 
      for( SurfaceMapsMap::const_iterator mi = _map.begin() ; mi != _map.end() ; ++mi ) {
	
	sstr << "  key: " <<  mi->first << " \t number of surfaces : " << mi->second.size() << std::endl ; 
      }
      sstr << "---------------------------------------------------------------- " << std::endl ;

      return sstr.str() ;
    }


  } // namespace
}// namespace
