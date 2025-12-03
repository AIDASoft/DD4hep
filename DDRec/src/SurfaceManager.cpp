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
#include "DDRec/SurfaceManager.h"

#include "DDRec/SurfaceHelper.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

#include <sstream>

namespace dd4hep {
  namespace rec {

    SurfaceManager::SurfaceManager(const Detector& theDetector) : _theDetector(theDetector) {

      // have to make sure the volume manager is populated once in order to have
      // the volumeIDs attached to the DetElements

      VolumeManager::getVolumeManager(theDetector);

    }
    
    
    const SurfaceMap* SurfaceManager::map( const std::string& name ) const {

      std::call_once( _initializedFlag, &SurfaceManager::initialize, this, _theDetector ) ;

      SurfaceMapsMap::const_iterator it = _map.find( name ) ;

      if( it != _map.end() ){

        return & it->second ;
      }

      return nullptr ;
    }

    void SurfaceManager::initialize(const Detector& description) const {
      
      for(const auto& type : description.detectorTypes()) {

        const std::vector<DetElement>& dets = description.detectors( type ) ;

        for(const auto& det : dets) {

          const std::string& name = det.name() ;

          SurfaceHelper surfH( det ) ;
	  
          const SurfaceList& detSL = surfH.surfaceList() ;
  
          // add an empty map for this detector in case there are no surfaces attached 
          _map.emplace(name , SurfaceMap());

          for( auto* surf : detSL ) {
            // enter surface into map for this detector
            _map[ name ].emplace(surf->id(), surf );

            // enter surface into map for detector type
            _map[ type ].emplace(surf->id(), surf );

            // enter surface into world map 
            _map[ "world" ].emplace(surf->id(), surf );

          }
        }
      }

      printout(INFO,"SurfaceManager","%s" , description.extension<SurfaceManager>()->toString().c_str() );

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
