#include "DDRec/SurfaceManager.h"

#include "DDRec/DetectorSurfaces.h"
#include "DD4hep/Detector.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/VolumeManager.h"

namespace DD4hep {
  
  using namespace Geometry ;

  namespace DDRec {
    

    SurfaceManager::SurfaceManager(DD4hep::Geometry::DetElement const& e) : _det(e) {

      initialize() ;
    }
    
    SurfaceManager::~SurfaceManager(){
      // nothing to do
    }
    
    
    void SurfaceManager::initialize() {
      
      // have to populate the volume manager once in order to have 
      // the volumeIDs attached to the DetElements
      LCDD& lcdd = LCDD::getInstance();
      static VolumeManager volMgr( lcdd  , "volMan" , lcdd.world() ) ;

 
      //------------------ breadth first tree traversal ---------
      std::list< DetElement > dets ;
      std::list< DetElement > daugs ; 
      std::list< DetElement > gdaugs ; 
      
      daugs.push_back( _det ) ;
      
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
      
      //      std::cout << " **** SurfaceManager::initialize() : # DetElements found  " << dets.size() << std::endl ;
      
      for( std::list< DetElement >::iterator li=dets.begin() ; li != dets.end() ; ++li ) {
	
	DetElement det = (*li) ;
	
	// create surfaces
	DetectorSurfaces ds( det ) ;
	
	const SurfaceList& detSL = ds.surfaceList() ;

	// and add copy them to this list
	_sL.insert( _sL.end(), detSL.begin(), detSL.end() );
      }
      
    }
    


  } // namespace
}// namespace
