#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h" 

#include "DDRec/DetectorData.h"
#include "DDRec/DDGear.h"

#include "gearimpl/TPCParametersImpl.h"
#include "gearimpl/FixedPadSizeDiskLayout.h"
#include "gearimpl/ZPlanarParametersImpl.h"

#include <iostream>

namespace DD4hep{
  namespace DDRec{
    
    using namespace Geometry ;
    //    using namespace gear ;

    /** Plugin that creates Gear objects for DetElements and attaches them 
     *  as extensions. Called from DDGear::createGearMgr().
     *  NB: this code is for backward compatibility to run 
     *  the old reconstruction that expects Gear information
     *  and should eventually be phased out.
     * 
     *  @author  F.Gaede, CERN/DESY
     *  @date Oct 2014
     *  @version $Id: $
     */
    
    static long createGearForILD(LCDD& lcdd, int argc, char** argv) {
      
      std::cout << " **** running plugin createGearForILD ! " <<  std::endl ;
      

      //========= TPC ==============================================================================
      DetElement tpcDE = lcdd.detector("TPC") ;

      FixedPadSizeTPCData* tpc = tpcDE.extension<FixedPadSizeTPCData>() ;

      gear::TPCParametersImpl* gearTPC = new gear::TPCParametersImpl( tpc->driftLength /dd4hep::mm , gear::PadRowLayout2D::POLAR ) ;
  
      gearTPC->setPadLayout( new gear::FixedPadSizeDiskLayout( tpc->rMinReadout/dd4hep::mm , tpc->rMaxReadout/dd4hep::mm, tpc->padHeight/dd4hep::mm,
							       tpc->padWidth/dd4hep::mm , tpc->maxRow, tpc->padGap /dd4hep::mm  ) ) ;
      
      gearTPC->setDoubleVal("tpcInnerRadius", tpc->rMin/dd4hep::mm  )  ; // inner r of support tube
      gearTPC->setDoubleVal("tpcOuterRadius", tpc->rMax/dd4hep::mm  )  ; // outer radius of TPC
      gearTPC->setDoubleVal("tpcInnerWallThickness", tpc->innerWallThickness/dd4hep::mm  )  ;   // thickness of inner shell
      gearTPC->setDoubleVal("tpcOuterWallThickness", tpc->outerWallThickness/dd4hep::mm  )  ;   // thickness of outer shell
      
      tpcDE.addExtension< GearHandle >( new GearHandle( gearTPC, "TPCParameters" ) ) ;
      
      //========= VXD ==============================================================================
      
      DetElement vxdDE = lcdd.detector("VTX") ;
      
      ZPlanarData* vxd = vxdDE.extension<ZPlanarData>() ;
      
      //      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
      int type =  gear::ZPlanarParameters::CMOS ;
      gear::ZPlanarParametersImpl* gearVXD = new gear::ZPlanarParametersImpl( type, vxd->rInnerShell/dd4hep::mm,  vxd->rOuterShell/dd4hep::mm,
									      vxd->zHalfShell/dd4hep::mm , vxd->gapShell/dd4hep::mm , 0.  ) ;
      
      for(unsigned i=0,n=vxd->layers.size() ; i<n; ++i){
	
	const DDRec::ZPlanarData::LayerLayout& l = vxd->layers[i] ;
	
	// FIXME set rad lengths to 0 -> need to get from DD4hep ....
	gearVXD->addLayer( l.ladderNumber, l.phi0, 
			   l.distanceSupport/dd4hep::mm,   l.offsetSupport/dd4hep::mm,   l. thicknessSupport/dd4hep::mm,   l.zHalfSupport/dd4hep::mm,   l.widthSupport/dd4hep::mm,   0. , 
			   l.distanceSensitive/dd4hep::mm, l.offsetSensitive/dd4hep::mm, l. thicknessSensitive/dd4hep::mm, l.zHalfSensitive/dd4hep::mm, l.widthSensitive/dd4hep::mm, 0. )  ;
	
      }
      
     vxdDE.addExtension< GearHandle >( new GearHandle( gearVXD, "ZPlanarParameters" ) ) ;

     //============================================================================================

      // --- LCDD::apply() expects return code 1 if all went well ! ----
      return 1;
    }
  }
}
DECLARE_APPLY( GearForILD, DD4hep::DDRec::createGearForILD )


