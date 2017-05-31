
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
      

#if 0  // this uses the deprected VolumeManagerContext::placement
      
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

      //FIXME: how to best implement this ?

      return 0 ;
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

      // traverse the daughter nodes:
      const TGeoNode* node = pv.ptr();
      
      for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
	  
	TGeoNode* daughter = node->GetDaughter(idau);
	PlacedVolume dpv( daughter );
	Readout r = findReadout( dpv ) ;
	if( r.isValid() )
	  return r ;
      }

      return Readout() ;
    }




  } /* namespace DDRec */
} /* namespace DD4hep */
