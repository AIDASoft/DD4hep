#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h" 

#include "DDRec/DetectorData.h"
#include "DDRec/DDGear.h"

#include "gearimpl/TPCParametersImpl.h"
#include "gearimpl/FixedPadSizeDiskLayout.h"
#include "gearimpl/ZPlanarParametersImpl.h"
#include "gearimpl/FTDParametersImpl.h"

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
      int vxdType =  gear::ZPlanarParameters::CMOS ;
      gear::ZPlanarParametersImpl* gearVXD = new gear::ZPlanarParametersImpl( vxdType, vxd->rInnerShell/dd4hep::mm,  vxd->rOuterShell/dd4hep::mm,
									      vxd->zHalfShell/dd4hep::mm , vxd->gapShell/dd4hep::mm , 0.  ) ;
      
      for(unsigned i=0,n=vxd->layers.size() ; i<n; ++i){
	
	const DDRec::ZPlanarData::LayerLayout& l = vxd->layers[i] ;
	
	// FIXME set rad lengths to 0 -> need to get from DD4hep ....
	gearVXD->addLayer( l.ladderNumber, l.phi0, 
			   l.distanceSupport/dd4hep::mm,   l.offsetSupport/dd4hep::mm,   l. thicknessSupport/dd4hep::mm,   l.zHalfSupport/dd4hep::mm,   l.widthSupport/dd4hep::mm,   0. , 
			   l.distanceSensitive/dd4hep::mm, l.offsetSensitive/dd4hep::mm, l. thicknessSensitive/dd4hep::mm, l.zHalfSensitive/dd4hep::mm, l.widthSensitive/dd4hep::mm, 0. )  ;
	
      }
      
     vxdDE.addExtension< GearHandle >( new GearHandle( gearVXD, "VXDParameters" ) ) ;

      //========= SIT ==============================================================================
      
      DetElement sitDE = lcdd.detector("SIT") ;
      
      ZPlanarData* sit = sitDE.extension<ZPlanarData>() ;
      
      //      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
      int sitType =  gear::ZPlanarParameters::CCD ;
      gear::ZPlanarParametersImpl* gearSIT = new gear::ZPlanarParametersImpl( sitType, sit->rInnerShell/dd4hep::mm,  sit->rOuterShell/dd4hep::mm,
									      sit->zHalfShell/dd4hep::mm , sit->gapShell/dd4hep::mm , 0.  ) ;
      std::vector<int> n_sensors_per_ladder ;

     for(unsigned i=0,n=sit->layers.size() ; i<n; ++i){
	
	const DDRec::ZPlanarData::LayerLayout& l = sit->layers[i] ;
	
	// FIXME set rad lengths to 0 -> need to get from DD4hep ....
	gearSIT->addLayer( l.ladderNumber, l.phi0, 
			   l.distanceSupport/dd4hep::mm,   l.offsetSupport/dd4hep::mm,   l. thicknessSupport/dd4hep::mm,   l.zHalfSupport/dd4hep::mm,   l.widthSupport/dd4hep::mm,   0. , 
			   l.distanceSensitive/dd4hep::mm, l.offsetSensitive/dd4hep::mm, l. thicknessSensitive/dd4hep::mm, l.zHalfSensitive/dd4hep::mm, l.widthSensitive/dd4hep::mm, 0. )  ;
	

	n_sensors_per_ladder.push_back( l.sensorsPerLadder);
     }
     
      gearSIT->setDoubleVal("strip_width_mm"  , sit->widthStrip / dd4hep::mm ) ;
      gearSIT->setDoubleVal("strip_length_mm" , sit->lengthStrip/ dd4hep::mm ) ;
      gearSIT->setDoubleVal("strip_pitch_mm"  , sit->pitchStrip / dd4hep::mm ) ;
      gearSIT->setDoubleVal("strip_angle_deg" , sit->angleStrip / dd4hep::deg ) ;

      
      gearSIT->setIntVals("n_sensors_per_ladder",n_sensors_per_ladder);

      sitDE.addExtension< GearHandle >( new GearHandle( gearSIT, "SITParameters" ) ) ;
     //============================================================================================

      DetElement setDE = lcdd.detector("SET") ;
      
      ZPlanarData* set = setDE.extension<ZPlanarData>() ;
      
      //      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
      int setType =  gear::ZPlanarParameters::CCD ;
      gear::ZPlanarParametersImpl* gearSET = new gear::ZPlanarParametersImpl( setType, set->rInnerShell/dd4hep::mm,  set->rOuterShell/dd4hep::mm,
									      set->zHalfShell/dd4hep::mm , set->gapShell/dd4hep::mm , 0.  ) ;
      //      std::vector<int> n_sensors_per_ladder ;
      n_sensors_per_ladder.clear() ;

     for(unsigned i=0,n=set->layers.size() ; i<n; ++i){
	
	const DDRec::ZPlanarData::LayerLayout& l = set->layers[i] ;
	
	// FIXME set rad lengths to 0 -> need to get from DD4hep ....
	gearSET->addLayer( l.ladderNumber, l.phi0, 
			   l.distanceSupport/dd4hep::mm,   l.offsetSupport/dd4hep::mm,   l. thicknessSupport/dd4hep::mm,   l.zHalfSupport/dd4hep::mm,   l.widthSupport/dd4hep::mm,   0. , 
			   l.distanceSensitive/dd4hep::mm, l.offsetSensitive/dd4hep::mm, l. thicknessSensitive/dd4hep::mm, l.zHalfSensitive/dd4hep::mm, l.widthSensitive/dd4hep::mm, 0. )  ;
	

	n_sensors_per_ladder.push_back( l.sensorsPerLadder);
     }
     
      gearSET->setDoubleVal("strip_width_mm"  , set->widthStrip / dd4hep::mm ) ;
      gearSET->setDoubleVal("strip_length_mm" , set->lengthStrip/ dd4hep::mm ) ;
      gearSET->setDoubleVal("strip_pitch_mm"  , set->pitchStrip / dd4hep::mm ) ;
      gearSET->setDoubleVal("strip_angle_deg" , set->angleStrip / dd4hep::deg ) ;

      
      gearSET->setIntVals("n_sensors_per_ladder",n_sensors_per_ladder);

      setDE.addExtension< GearHandle >( new GearHandle( gearSET, "SETParameters" ) ) ;
     //============================================================================================

      DetElement ftdDE = lcdd.detector("FTD") ;
      
      ZDiskPetalsData* ftd = ftdDE.extension<ZDiskPetalsData>() ;
      
      gear::FTDParametersImpl* gearFTD = new gear::FTDParametersImpl();
      
      for(unsigned i=0,n=ftd->layers.size() ; i<n; ++i){
	
	const DDRec::ZDiskPetalsData::LayerLayout& l = ftd->layers[i] ;
	

	bool isDoubleSided  = l.typeFlags[ DDRec::ZDiskPetalsStruct::SensorType::DoubleSided ] ;
	int  sensorType   = ( l.typeFlags[ DDRec::ZDiskPetalsStruct::SensorType::Pixel ] ?
			      gear::FTDParameters::PIXEL :  gear::FTDParameters::STRIP ) ;

	double zoffset = fabs( l.zOffsetSupport ) ;
	double signoffset =  l.zOffsetSupport > 0  ?  1. : -1 ;
	
	gearFTD->addLayer( l.petalNumber, l.sensorsPerPetal, 
			   isDoubleSided, sensorType, 
			   l.petalHalfAngle, l.phi0, l.alphaPetal, 
			   l.zPosition/dd4hep::mm, zoffset/dd4hep::mm, signoffset,
			   l.distanceSupport/dd4hep::mm, l.thicknessSupport/dd4hep::mm,
			   l.widthInnerSupport/dd4hep::mm, l.widthOuterSupport/dd4hep::mm,
			   l.lengthSupport/dd4hep::mm, 
			   0.,
			   l.distanceSensitive/dd4hep::mm, l.thicknessSensitive/dd4hep::mm,
			   l.widthInnerSensitive/dd4hep::mm, l.widthOuterSensitive/dd4hep::mm,
			   l.lengthSensitive/dd4hep::mm, 
			   0. ) ;
	

	// FIXME set rad lengths to 0 -> need to get from DD4hep ....
      }
     
      gearFTD->setDoubleVal("strip_width_mm"  , ftd->widthStrip / dd4hep::mm ) ;
      gearFTD->setDoubleVal("strip_length_mm" , ftd->lengthStrip/ dd4hep::mm ) ;
      gearFTD->setDoubleVal("strip_pitch_mm"  , ftd->pitchStrip / dd4hep::mm ) ;
      gearFTD->setDoubleVal("strip_angle_deg" , ftd->angleStrip / dd4hep::deg ) ;

     
      ftdDE.addExtension< GearHandle >( new GearHandle( gearFTD, "FTDParameters" ) ) ;
     //============================================================================================



      // --- LCDD::apply() expects return code 1 if all went well ! ----
      return 1;
    }
  }
}
DECLARE_APPLY( GearForILD, DD4hep::DDRec::createGearForILD )


