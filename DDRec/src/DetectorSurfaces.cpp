//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#include "DDRec/DetectorSurfaces.h"

namespace dd4hep {
  namespace rec {
    

    DetectorSurfaces::DetectorSurfaces(dd4hep::DetElement const& e) {

      initialize(e) ;
    }
    
    void DetectorSurfaces::initialize(dd4hep::DetElement const& det) {
      
      const VolSurfaceList* vsL = volSurfaceList(det) ;

      _sL = det.extension< SurfaceList >(false) ;
      if (! _sL) {
        _sL = det.addExtension<SurfaceList >(  new SurfaceList( true )  ) ;
      }

      if( ! _sL->empty() ) {  // only fill surfaces for this DetElement once
        return ;
      }
	
      // std::cout <<  "     detector  " << det.name() << " id: " << det.id() << " has " <<  vsL->size() << " surfaces "  << std::endl ;

      // std::cout << " ------------------------- "
      // 	  << " 	DetectorSurfaces::initialize()  adding surfaces : "
      // 	  << std::endl ;

      for( const auto& volSurf : *vsL ) {

        Surface* surf = nullptr ;

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


  } // namespace
}// namespace
