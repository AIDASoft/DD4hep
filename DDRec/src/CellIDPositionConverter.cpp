
#include "DDRec/CellIDPositionConverter.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/detail/VolumeManagerInterna.h"

#include "TGeoManager.h"

namespace dd4hep {
  namespace rec {

    const VolumeManagerContext*
    CellIDPositionConverter::findContext(const CellID& cellID) const {
      return _volumeManager.lookupContext( cellID ) ;
    }
    

    Position CellIDPositionConverter::position(const CellID& cell) const {

      // untill we have the alignment map object, we return the nominal position

      return positionNominal( cell ) ;
    }

    Position CellIDPositionConverter::positionNominal(const CellID& cell) const {

      double l[3], e[3], g[3];

      const VolumeManagerContext* context = findContext( cell ) ;

      if( context == NULL)
	return Position() ;

      DetElement det = context->element ;
      

#if 0  // this uses the deprecated VolumeManagerContext::placement
      
      PlacedVolume pv = context->placement ;

      if( ! pv.volume().isSensitive() )
	return Position() ;
	
      SensitiveDetector sd = pv.volume().sensitiveDetector();
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
      
      TGeoManager *geoManager = _lcdd->world().volume()->GetGeoManager() ;
      
      PlacedVolume pv = geoManager->FindNode( global.x() , global.y() , global.z() ) ;
      
      if(  pv.isValid() && pv.volume().isSensitive() ) {

	TGeoHMatrix*  m = geoManager->GetCurrentMatrix() ;
      
	double g[3], l[3] ;
	global.GetCoordinates( g ) ;
	m->MasterToLocal( g, l );

	SensitiveDetector sd = pv.volume().sensitiveDetector();
	Readout r = sd.readout() ;
	
	// collect all volIDs for the current path
	PlacedVolume::VolIDs volIDs ;
	volIDs.insert( std::end(volIDs), std::begin(pv.volIDs()), std::end(pv.volIDs())) ;

	TGeoPhysicalNode pN( geoManager->GetPath() ) ; 
	
	unsigned motherCount = 0 ;

	while( pN.GetMother( motherCount ) != NULL   ){

	    PlacedVolume mPv = pN.GetMother( motherCount++ ) ;
	    
	    if( mPv.isValid() &&  pN.GetMother( motherCount ) != NULL )  // world has no volIDs
	      volIDs.insert( std::end(volIDs), std::begin(mPv.volIDs()), std::end(mPv.volIDs())) ;
	}
	
	VolumeID volIDPVs = r.idSpec().encode( volIDs ) ;
	
	result = r.segmentation().cellID( Position( l[0], l[1], l[2] ) , global, volIDPVs  );
      }
	
      return result ;
    }

    // CellID CellIDPositionConverter::cellID(const Position& global) const {
      
    //   CellID result(0) ;
      
    //   DetElement motherDet = _lcdd->world()  ; // could also start from an arbitrary DetElement here !?
      
    //   DetElement det = findDetElement( global , motherDet ) ;
      
    //   if( ! det.isValid() )
    // 	return result ;
      
    //   double g[3], e[3] , l[3] ;
    //   global.GetCoordinates( g ) ;
    //   det.nominal().worldTransformation().MasterToLocal( g, e );
      
    //   PlacedVolume::VolIDs volIDs ;
      
    //   PlacedVolume pv = findPlacement( Position( e[0], e[1] , e[2] ) , det.placement() , l , volIDs ) ;
      
    //   TGeoManager *geoManager = det.volume()->GetGeoManager() ;
    //   // TGeoManager *geoManager = _lcdd->world().volume()->GetGeoManager() ;
      
    //   PlacedVolume pv1 = geoManager->FindNode( global.x() , global.y() , global.z() ) ;

    //   // std::cout << " -> TGM : " << pv1.name() << "  valid: " << pv1.isValid() << " sensitive: "
    //   // 		<<  (pv1.isValid() ? pv1.volume().isSensitive() : false ) << std::endl ;
    //   // std::cout << " -> FG :  " << pv.name() << "  valid: " << pv.isValid() << " sensitive: "
    //   // 		<<  (pv.isValid() ? pv.volume().isSensitive() : false ) << std::endl ;



    // 	if(  pv.isValid() && pv.volume().isSensitive() ) {
	
    // 	Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
    // 	Readout r = sd.readout() ;
	
    // 	VolumeID volIDElement = det.volumeID() ;
    // 	// add the placed volumes volIDs:
    // 	VolumeID volIDPVs = r.idSpec().encode( volIDs ) ;
	
    // 	result = r.segmentation().cellID( Position( l[0], l[1], l[2] ) , global, ( volIDElement | volIDPVs  ) );
	
    //   // } else {
    //   // 	std::cout << " *** ERROR : found no sensitive Placement " << pv.name()
    //   // 		  << "  for point " << global << " try with TGeoManager ... " << std::endl ;
	
    //   // 	TGeoManager *geoManager = det.volume()->GetGeoManager() ;
    //   // 	// TGeoManager *geoManager = _lcdd->world().volume()->GetGeoManager() ;

    //   // 	PlacedVolume p = geoManager->FindNode( global.x() , global.y() , global.z() ) ;

    //   // 	std::cout << " -> found: " << p.name() << "  valid: " << p.isValid() << " sensitive: "
    //   // 		  <<  (p.isValid() ? p.volume().isSensitive() : false ) << std::endl ;
	
    //   }
	
    //   return result ;
    // }


    namespace {
      
      bool containsPoint( const DetElement& det, const Position& global ) {
	
	if( det.volume().isValid() and det.volume().solid().isValid() ) {
	  
	  double g[3], l[3] ;
	  global.GetCoordinates( g ) ;
	  
	  det.nominal().worldTransformation().MasterToLocal( g, l );
	  
	  return det.volume().solid()->Contains( l ) ;
	}
	
	return false ;
      }
      
    }

    DetElement CellIDPositionConverter::findDetElement(const Position& global,
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

    PlacedVolume CellIDPositionConverter::findPlacement(const Position& pos, const  PlacedVolume& pv , double locPos[3], PlacedVolume::VolIDs& volIDs) const {

      
      double l[3] ;
      pos.GetCoordinates( l ) ;

      //      std::cout << " --- " << pos << " " << pv.name() << " loc: (" << locPos[0] << "," << locPos[1] << "," << locPos[2] << ")" << std::endl ;

      if( pv.volume().solid()->Contains( l ) ) {
	
	// copy the volIDs
	volIDs.insert( std::end(volIDs), std::begin(pv.volIDs()), std::end(pv.volIDs()));
	
	int ndau = pv->GetNdaughters() ;

	if( ndau == 0 ) // no daughter volumes -> we are done
	  return pv ;
	
	// see if we have a daughter volume that contains the point ...

	PlacedVolume result ;
	for (int i = 0 ; i < ndau; ++i) {
	  
	  PlacedVolume pvDau = pv->GetDaughter( i );
	  pvDau->MasterToLocal( l , locPos ) ;  // transform point to daughter's local frame

	  // std::cout << "   - " << pos << " " << pvDau.name()
	  // 	    << " loc: (" << locPos[0] << "," << locPos[1] << "," << locPos[2] << ")"
	  // 	    <<  pvDau.volume().solid()->Contains( locPos )
	  // 	    << " ndau: " << pvDau->GetNdaughters()
	  // 	    << std::endl ;


	  if( pvDau.volume().solid()->Contains( locPos ) ) { // point is contained in daughter node
	    result = pvDau ;
	    volIDs.insert( std::end(volIDs), std::begin(pvDau.volIDs()), std::end(pvDau.volIDs()) );
	    break ;
	  }
	}

	if( result.isValid() ){  // ... yes, we have

	  if( result->GetNdaughters() == 0 ){  // no more children -> done
	    return result ;
	  } else
	    return findPlacement( Position( locPos[0], locPos[1] , locPos[2]  ), result , locPos , volIDs) ; // keep searching in daughter volumes
	}
	
      }
	  
      return  PlacedVolume() ;
    } 

    
    Readout CellIDPositionConverter::findReadout(const DetElement& det) const {

      // first check if top level is a sensitive detector
      if (det.volume().isValid() and det.volume().isSensitive()) {
	SensitiveDetector sd = det.volume().sensitiveDetector();
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

    Readout CellIDPositionConverter::findReadout(const PlacedVolume& pv) const {
      
      // first check if we are in a sensitive volume
      if( pv.volume().isSensitive() ){
	SensitiveDetector sd = pv.volume().sensitiveDetector();
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




  } /* namespace rec */
} /* namespace dd4hep */
