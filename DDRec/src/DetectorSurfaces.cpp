#include "DDRec/DetectorSurfaces.h"

namespace dd4hep {
  namespace rec {
    


    DetectorSurfaces::DetectorSurfaces(dd4hep::DetElement const& e) : DetElement(e) , _sL( 0 ) {

      initialize() ;
    }
    
    DetectorSurfaces::~DetectorSurfaces(){
      // nothing to do: SurfaceList is added as extension
      // and is deleted automatically
    }
    
    
    void DetectorSurfaces::initialize() {
      
      DetElement det = *this ;
      
      const VolSurfaceList* vsL = volSurfaceList(det) ;

      try {
        _sL = det.extension< SurfaceList >() ;

      } catch(const std::exception& e) { 
        _sL = det.addExtension<SurfaceList >(  new SurfaceList( true )  ) ; 
      }

      if( ! vsL->empty() && _sL->empty() ) {  // only fill surfaces for this DetElement once
	
	// std::cout <<  "     detector  " << det.name() << " id: " << det.id() << " has " <<  vsL->size() << " surfaces "  << std::endl ;
	
	// std::cout << " ------------------------- " 
	// 	  << " 	DetectorSurfaces::initialize()  adding surfaces : " 
	// 	  << std::endl ;
	
	for( VolSurfaceList::const_iterator it = vsL->begin() ; it != vsL->end() ; ++it ){
	  
	  VolSurface volSurf =  *it ;
	  
	  Surface* surf = 0 ;
	  
	  if( volSurf.type().isCylinder() )
	    surf = new CylinderSurface(  det,  volSurf ) ;
	  
	  else if( volSurf.type().isCone() ) 
	    surf = new ConeSurface( det, volSurf ) ;
	  
	  else
	    surf = new Surface(  det,  volSurf ) ;
	
	  // std::cout << " ------------------------- " 
	  //   	    << " surface: "   << *surf        << std::endl
	  //   	    << " ------------------------- "  << std::endl ;
	  
	  _sL->push_back( surf ) ;
	  
	}


      }

    }
    


  } // namespace
}// namespace
