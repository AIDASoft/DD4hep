#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h" 

#include "DDRec/DetectorData.h"
#include "DDRec/DDGear.h"
#include "DDRec/MaterialManager.h"
#include "DDRec/API/Calorimeter.h"

#include "DDSurfaces/Vector3D.h"

#include "gearimpl/TPCParametersImpl.h"
#include "gearimpl/FixedPadSizeDiskLayout.h"
#include "gearimpl/ZPlanarParametersImpl.h"
#include "gearimpl/FTDParametersImpl.h"
#include "gearimpl/CalorimeterParametersImpl.h"

#include <iostream>

namespace DD4hep{
  namespace DDRec{
    
    using namespace Geometry ;
    //    using namespace gear ;
    using DDSurfaces::Vector3D ;

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
    
    static long createGearForILD(LCDD& lcdd, int /*argc*/, char** /*argv*/) {
      
      std::cout << " **** running plugin createGearForILD ! " <<  std::endl ;
      

      //========= TPC ==============================================================================
      try{

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

      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //========= VXD ==============================================================================
      
      try{
	DetElement vxdDE = lcdd.detector("VXD") ;
	
	ZPlanarData* vxd = vxdDE.extension<ZPlanarData>() ;
	
	//      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
	int vxdType =  gear::ZPlanarParameters::CMOS ;
	gear::ZPlanarParametersImpl* gearVXD = new gear::ZPlanarParametersImpl( vxdType, vxd->rInnerShell/dd4hep::mm,  vxd->rOuterShell/dd4hep::mm,
										vxd->zHalfShell/dd4hep::mm , vxd->gapShell/dd4hep::mm , 0.  ) ;
	
	for(unsigned i=0,n=vxd->layers.size() ; i<n; ++i){
	  
	  const DDRec::ZPlanarData::LayerLayout& l = vxd->layers[i] ;
	  
	  // FIXME set rad lengths to 0 -> need to get from DD4hep ....
	  gearVXD->addLayer( l.ladderNumber, l.phi0, 
			     l.distanceSupport/dd4hep::mm,   l.offsetSupport/dd4hep::mm,   l.thicknessSupport/dd4hep::mm,   l.zHalfSupport/dd4hep::mm,   l.widthSupport/dd4hep::mm,   0. , 
			     l.distanceSensitive/dd4hep::mm, l.offsetSensitive/dd4hep::mm, l.thicknessSensitive/dd4hep::mm, l.zHalfSensitive/dd4hep::mm, l.widthSensitive/dd4hep::mm, 0. )  ;
	
	}
	
	GearHandle* handle = new GearHandle( gearVXD, "VXDParameters" )  ;
	
	// quick hack for now: add the one material that is needed by KalDet :  
	//      handle->addMaterial( "VXDSupportMaterial", 2.075865162e+01, 1.039383117e+01, 2.765900000e+02, 1.014262421e+03, 3.341388059e+03)  ; 
	
	// -------- better: get right averaged material from first ladder:  ------------------
	MaterialManager matMgr ;
      
	const DDRec::ZPlanarData::LayerLayout& l = vxd->layers[0] ;
	
	Vector3D a( l.distanceSupport                      , l.phi0 , 0. ,  Vector3D::cylindrical ) ;
	Vector3D b( l.distanceSupport + l.thicknessSupport , l.phi0 , 0. ,  Vector3D::cylindrical ) ;
	
	const MaterialVec& materials = matMgr.materialsBetween( a , b  ) ;
	
	MaterialData mat = ( materials.size() > 1  ? matMgr.createAveragedMaterial( materials ) : materials[0].first  ) ;
	
	// std::cout << " ####### found materials between points : " << a << " and " << b << " : " ;
	// for( unsigned i=0,n=materials.size();i<n;++i){
	// 	std::cout <<  materials[i].first.name() << "[" <<   materials[i].second << "], " ;
	// }
	// std::cout << std::endl ;
	// std::cout << "   averaged material : " << mat << std::endl ;
	
	handle->addMaterial( "VXDSupportMaterial", mat.A(), mat.Z() , mat.density()/(dd4hep::kg/(dd4hep::g*dd4hep::m3)) , mat.radiationLength()/dd4hep::mm , mat.interactionLength()/dd4hep::mm )  ; 
	
	
	vxdDE.addExtension< GearHandle >( handle ) ;

      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //========= SIT ==============================================================================
      
      try{ 

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

      } catch( std::runtime_error& e ){ 
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //============================================================================================

      try {

	DetElement setDE = lcdd.detector("SET") ;
	
	ZPlanarData* set = setDE.extension<ZPlanarData>() ;
	
	//      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
	int setType =  gear::ZPlanarParameters::CCD ;
	gear::ZPlanarParametersImpl* gearSET = new gear::ZPlanarParametersImpl( setType, set->rInnerShell/dd4hep::mm,  set->rOuterShell/dd4hep::mm,
										set->zHalfShell/dd4hep::mm , set->gapShell/dd4hep::mm , 0.  ) ;
	std::vector<int> n_sensors_per_ladder ;
	//n_sensors_per_ladder.clear() ;
	
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

      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 
      
      //============================================================================================

      try {

	DetElement ftdDE = lcdd.detector("FTD") ;
	
	ZDiskPetalsData* ftd = ftdDE.extension<ZDiskPetalsData>() ;
	
	gear::FTDParametersImpl* gearFTD = new gear::FTDParametersImpl();
	
	for(unsigned i=0,n=ftd->layers.size() ; i<n; ++i){
	  
	  const DDRec::ZDiskPetalsData::LayerLayout& l = ftd->layers[i] ;
	  
	  
	  bool isDoubleSided  = l.typeFlags[ DDRec::ZDiskPetalsStruct::SensorType::DoubleSided ] ;
	  
	  // avoid 'undefined reference' at link time ( if built w/o optimization ):
	  static const int PIXEL = gear::FTDParameters::PIXEL ;
	  static const int STRIP = gear::FTDParameters::STRIP ;
	  int  sensorType   = ( l.typeFlags[ DDRec::ZDiskPetalsStruct::SensorType::Pixel ] ? PIXEL : STRIP ) ;
	  //			      gear::FTDParameters::PIXEL :  gear::FTDParameters::STRIP ) ;
	  
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

      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //============================================================================================

      try {

	DetElement tubeDE = lcdd.detector("Tube") ;
	
	ConicalSupportData* tube = tubeDE.extension<ConicalSupportData>() ;
	
	gear::GearParametersImpl* gearTUBE = new gear::GearParametersImpl();
	
	tube->isSymmetricInZ = true ;
	
	unsigned n = tube->sections.size() ;
	
	std::vector<double> rInner(n) ;
	std::vector<double> rOuter(n) ;
	std::vector<double> zStart(n) ;
	
	for(unsigned i=0 ; i<n ; ++i){
	  
	  const ConicalSupportData::Section& s = tube->sections[i] ;
	  
	  rInner[i] = s.rInner/ dd4hep::mm  ; 
	  rOuter[i] = s.rOuter/ dd4hep::mm  ; 
	  zStart[i] = s.zPos  / dd4hep::mm  ; 
	  
	  // FIXME set rad lengths to 0 -> need to get from DD4hep ....
	}
	
	gearTUBE->setDoubleVals("RInner" , rInner ) ;
	gearTUBE->setDoubleVals("ROuter" , rOuter ) ;
	gearTUBE->setDoubleVals("Z"      , zStart ) ;
	
	
	tubeDE.addExtension< GearHandle >( new GearHandle( gearTUBE, "BeamPipe" ) ) ;
      
      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //========= CALO ==============================================================================

      //**********************************************************
      //*  test gear interface w/ LayeredCalorimeterData extension
      //**********************************************************

      try {

	DetElement caloDE = lcdd.detector("HcalBarrel") ;
	
	LayeredCalorimeterData* calo = caloDE.extension<LayeredCalorimeterData>() ;
      
	gear::CalorimeterParametersImpl* gearCalo = 
	  ( calo->layoutType == LayeredCalorimeterData::BarrelLayout  ?
	    new gear::CalorimeterParametersImpl(  calo->extent[0]/dd4hep::mm, calo->extent[3]/dd4hep::mm, calo->symmetry, calo->phi0 )  :
	    //CalorimeterParametersImpl (double rMin, double zMax, int symOrder=8, double phi0=0.0) - C'tor for a cylindrical (octagonal) BARREL calorimeter.
	    new gear::CalorimeterParametersImpl(  calo->extent[0]/dd4hep::mm,  calo->extent[1]/dd4hep::mm,  calo->extent[2]/dd4hep::mm, calo->symmetry, calo->phi0 )   ) ;
	//CalorimeterParametersImpl (double rMin, double rMax, double zMin, int symOrder=2, double phi0=0.0) - C'tor for a cylindrical (octagonal) ENDCAP calorimeter. 
	
	for( unsigned i=0, nL = calo->layers.size() ; i <nL ; ++i ){
	  
	  LayeredCalorimeterData::Layer& l = calo->layers[i] ;
	  
	  if( i == 0 ) {
	    gearCalo->layerLayout().positionLayer( l.distance/dd4hep::mm, l.thickness/dd4hep::mm , l.cellSize0/dd4hep::mm, l.cellSize1/dd4hep::mm, l.absorberThickness/dd4hep::mm ) ;
	  }else{
	    gearCalo->layerLayout().addLayer(                             l.thickness/dd4hep::mm , l.cellSize0/dd4hep::mm, l.cellSize1/dd4hep::mm, l.absorberThickness/dd4hep::mm ) ;
	  }
	  
	  
	}
	
	caloDE.addExtension< GearHandle >( new GearHandle( gearCalo, "HcalBarrelParameters" ) ) ;
      

      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 


      //**********************************************************
      //*  test gear interface w/ LayeredExtensionImpl extension
      //**********************************************************
      
      // DetElement calo2DE = lcdd.detector("EcalBarrel") ;
      
      // Calorimeter calo2( calo2DE ) ;
      
      // gear::CalorimeterParametersImpl* gearCalo2 = 
      // 	( calo2.isBarrel()  ?
      // 	  new gear::CalorimeterParametersImpl(  calo2.getRMin()/dd4hep::mm,                             calo2.getZMax()/dd4hep::mm, calo2.getNSides(),  0. )  :    // fixme: phi 0  is not defined ??
      // 	  new gear::CalorimeterParametersImpl(  calo2.getRMin()/dd4hep::mm, calo2.getRMax()/dd4hep::mm, calo2.getZMin()/dd4hep::mm, calo2.getNSides(),  0. ) 
      // 	  ) ;

      // for( unsigned i=0, nL = calo2.numberOfLayers() ; i <nL ; ++i ){
	    
      // 	if( i == 0 ) {
      // 	  gearCalo2->layerLayout().positionLayer( calo2.getRMin()/dd4hep::mm, calo2.thickness(i)/dd4hep::mm ,  0. /dd4hep::mm,   0. /dd4hep::mm, calo2.absorberThickness(i)/dd4hep::mm ) ;

      // 	}else{                                                                                        //     fixme:   cell sizes  not in API !? 

      // 	  gearCalo2->layerLayout().addLayer(                                  calo2.thickness(i)/dd4hep::mm ,  0. /dd4hep::mm,   0. /dd4hep::mm, calo2.absorberThickness(i)/dd4hep::mm ) ;
      // 	}


      // }

      // calo2DE.addExtension< GearHandle >( new GearHandle( gearCalo2, "EcalBarrelParameters" ) ) ;


      //============================================================================================



      // --- LCDD::apply() expects return code 1 if all went well ! ----
      return 1;
    }
  }
}
DECLARE_APPLY( GearForILD, DD4hep::DDRec::createGearForILD )


