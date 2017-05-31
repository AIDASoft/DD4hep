
#include "DDRec/CellIDPositionConverter.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/objects/VolumeManagerInterna.h"

namespace DD4hep {
  namespace DDRec {

    using Geometry::DetElement;
    using Geometry::LCDD;
    using Geometry::PlacedVolume;
    using Geometry::Readout;
    using Geometry::Solid;
    using Geometry::VolumeManager;
    using Geometry::Volume;
    using Geometry::SensitiveDetector;
    using Geometry::Position;
    using Geometry::Segmentation;
    using std::set;



    const DD4hep::Geometry::VolumeManagerContext*
    CellIDPositionConverter::findContext(const CellID& cellID) const {
      return _volumeManager.lookupContext( cellID ) ;
    }
    

    Position CellIDPositionConverter::position(const CellID& cell) const {

      // untill we have the alignment map object, we return the nominal position

      return positionNominal( cell ) ;
    }

    Position CellIDPositionConverter::positionNominal(const CellID& cell) const {

      double l[3], e[3], g[3];

      const Geometry::VolumeManagerContext* context = findContext( cell ) ;

      if( context == NULL)
	return Position() ;

      DetElement det = context->element ;
      

#if 0  // this uses the deprecated VolumeManagerContext::placement
      
      PlacedVolume pv = context->placement ;

      if( ! pv.volume().isSensitive() )
	return Position() ;
	
      Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
      Readout r = sd.readout() ;

#else

      // use a recursive search for the Readout
      
      Readout r = findReadout( det ) ;

#endif



      Segmentation seg = r.segmentation() ;
      Position local = seg.position(cell);
      
      local.GetCoordinates(l);

      const TGeoMatrix& volToElement = context->toElement ;
      volToElement.LocalToMaster(l, e);

      const TGeoMatrix& elementToGlobal = det.nominal().worldTransformation();
      elementToGlobal.LocalToMaster(e, g);


      return Position(g[0], g[1], g[2]);
    }



    CellID CellIDPositionConverter::cellID(const Position& global) const {

      CellID result(0) ;
      
      DetElement motherDet = _lcdd->world()  ; // could also start from an arbitrary DetElement here !?

      DetElement det = findDetElement( global , motherDet ) ;

      if( ! det.isValid() )
	return result ;

      double g[3], e[3] , l[3] ;
      global.GetCoordinates( g ) ;
      det.nominal().worldTransformation().MasterToLocal( g, e );
     
      PlacedVolume pv = findPlacement( Position( e[0], e[1] , e[2] ) , det.placement() , l ) ;
      
      if(  pv.isValid() && pv.volume().isSensitive() ) {

	Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
	Readout r = sd.readout() ;
	VolumeID volID = det.volumeID() ;

	result = r.segmentation().cellID( Position( l[0], l[1], l[2] ) , global, volID );

      } else {
	
	std::cout << " *** ERROR : found non-sensitive Placement " << pv.name()
		  << "  for point " << global << std::endl ;
      }

      
      return result ;
    }


    namespace {
      
      bool containsPoint( const DetElement& det, const Geometry::Position& global ) {
	
	if( det.volume().isValid() and det.volume().solid().isValid() ) {
	  
	  double g[3], l[3] ;
	  global.GetCoordinates( g ) ;
	  
	  det.nominal().worldTransformation().MasterToLocal( g, l );
	  
	  return det.volume().solid()->Contains( l ) ;
	}
	
	return false ;
      }
      
    }

    DetElement CellIDPositionConverter::findDetElement(const Geometry::Position& global,
						       const DetElement& d) const {

      DetElement det = ( d.isValid() ? d : _lcdd->world() ) ;
      
      //      std::cout << " --- " << global << det.name() << std::endl ;

      if( containsPoint( det, global ) ) {
	
	if( det.children().empty() )  // no children -> we are done 
	  return det ;
	
	// see if we have a child DetElement that contains the point ...

	DetElement result ;
	for( auto it : det.children() ){
	  // std::cout << " -   " << global << it.second.name()  << " " << containsPoint( it.second , global )
	  // 	    << " nChild: " << it.second.children().size() << " isValid: " <<  it.second.isValid()
	  // 	    << std::endl ;

	  if( containsPoint( it.second , global ) ){
	    result = it.second ;
	    break ;
	  }
	}  
	if( result.isValid() ){  // ... yes, we have

	  if( result.children().empty() )  // no more children -> done
	    return result ;
	  else
	    return findDetElement( global, result ) ; // keep searching in children
	}
	
      }
      
      // point not contained 
      return DetElement() ;
    } 

    Geometry::PlacedVolume CellIDPositionConverter::findPlacement(const Geometry::Position& pos, const  Geometry::PlacedVolume& pv , double locPos[3]) const {

      
      double l[3] ;
      pos.GetCoordinates( l ) ;

      std::cout << " --- " << pos << " " << pv.name() << " loc: (" << locPos[0] << "," << locPos[1] << "," << locPos[2] << ")" << std::endl ;

      if( pv.volume().solid()->Contains( l ) ) {
	
	int ndau = pv->GetNdaughters() ;

	if( ndau == 0 ) // no daughter volumes -> we are done
	  return pv ;
	
	// see if we have a daughter volume that contains the point ...

	PlacedVolume result ;
	for (int i = 0 ; i < ndau; ++i) {
	  
	  PlacedVolume pvDau = pv->GetDaughter( i );
	  pvDau->MasterToLocal( l , locPos ) ;  // transform point to daughter's local frame

	  std::cout << "   - " << pos << " " << pvDau.name()
		    << " loc: (" << locPos[0] << "," << locPos[1] << "," << locPos[2] << ")"
		    <<  pvDau.volume().solid()->Contains( locPos )
		    << " ndau: " << pvDau->GetNdaughters()
		    << std::endl ;


	  if( pvDau.volume().solid()->Contains( locPos ) ) { // point is contained in daughter node
	    result = pvDau ;
	    break ;
	  }
	}

	if( result.isValid() ){  // ... yes, we have

	  if( result->GetNdaughters() == 0 ){  // no more children -> done
	    return result ;
	  } else
	    return findPlacement( Position( locPos[0], locPos[1] , locPos[2]  ), result , locPos ) ; // keep searching in daughter volumes
	}
	
      }
	  
      return  PlacedVolume() ;
    } 



    Readout CellIDPositionConverter::findReadout(const Geometry::DetElement& det) const {

      // first check if top level is a sensitive detector
      if (det.volume().isValid() and det.volume().isSensitive()) {
	Geometry::SensitiveDetector sd = det.volume().sensitiveDetector();
	if (sd.isValid() and sd.readout().isValid()) {
	  return sd.readout();
	}
      }
      // if not, return the first sensitive daughter volume's readout

      Readout r = findReadout( det.placement() ) ;
      if( r.isValid() )
	return r ;

      // nothing found !?
      return Readout();
    }

    Readout CellIDPositionConverter::findReadout(const Geometry::PlacedVolume& pv) const {
      
      // first check if we are in a sensitive volume
      if( pv.volume().isSensitive() ){
	Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
      	if (sd.isValid() and sd.readout().isValid()) {
	  return sd.readout();
	}
      }

      for (Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau) {
	  
	PlacedVolume dpv = pv->GetDaughter(idau);
	Readout r = findReadout( dpv ) ;
	if( r.isValid() )
	  return r ;
      }

      return Readout() ;
    }




  } /* namespace DDRec */
} /* namespace DD4hep */
