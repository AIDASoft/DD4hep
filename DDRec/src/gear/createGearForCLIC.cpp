#include "DD4hep/Detector.h"
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

namespace dd4hep{
  namespace rec{
    
    using namespace detail ;
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
     *  @version $Id$
     */
    
    static long createGearForCLIC(Detector& description, int /*argc*/, char** /*argv*/) {
      
      std::cout << " **** running plugin createGearForCLIC ! " <<  std::endl ;
      
   
      //========= VXD ==============================================================================
      
      try{
	DetElement vxdDE = description.detector("VertexBarrel") ;
	
	ZPlanarData* vxd = vxdDE.extension<ZPlanarData>() ;
	
	//      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
	int vxdType =  gear::ZPlanarParameters::CMOS ;
	gear::ZPlanarParametersImpl* gearVXD = new gear::ZPlanarParametersImpl( vxdType, vxd->rInnerShell/dd4hep::mm,  vxd->rOuterShell/dd4hep::mm,
										vxd->zHalfShell/dd4hep::mm , vxd->gapShell/dd4hep::mm , 0.  ) ;
	
//   std::cout<<"Got "<<vxd->layers.size()<<" layers."<<std::endl;
	for(unsigned i=0,n=vxd->layers.size() ; i<n; ++i){
//     std::cout<<"Working on layer "<<i<<" for a total of "<<n<<" layers."<<std::endl;
    
	  const rec::ZPlanarData::LayerLayout& l = vxd->layers[i] ;
	  
	  // FIXME set rad lengths to 0 -> need to get from dd4hep ....
	  gearVXD->addLayer( l.ladderNumber, l.phi0, 
			     l.distanceSupport/dd4hep::mm,   l.offsetSupport/dd4hep::mm,   l.thicknessSupport/dd4hep::mm,   l.zHalfSupport/dd4hep::mm,   l.widthSupport/dd4hep::mm,   0. , 
			     l.distanceSensitive/dd4hep::mm, l.offsetSensitive/dd4hep::mm, l.thicknessSensitive/dd4hep::mm, l.zHalfSensitive/dd4hep::mm, l.widthSensitive/dd4hep::mm, 0. )  ;
	
	}
	
	std::cout<<"Added layers"<<std::endl;
  
	GearHandle* handle = new GearHandle( gearVXD, "VXDParameters" )  ;
	
	// quick hack for now: add the one material that is needed by KalDet :  
	     handle->addMaterial( "VXDSupportMaterial", 2.075865162e+01, 1.039383117e+01, 2.765900000e+02, 1.014262421e+03, 3.341388059e+03)  ; 
	
	// -------- better: get right averaged material from first ladder:  ------------------
// 	MaterialManager matMgr ;
//       
// 	const rec::ZPlanarData::LayerLayout& l = vxd->layers[0] ;
// 	
// 	Vector3D a( l.distanceSupport                      , l.phi0 , 0. ,  Vector3D::cylindrical ) ;
// 	Vector3D b( l.distanceSupport + l.thicknessSupport , l.phi0 , 0. ,  Vector3D::cylindrical ) ;
// 	
// 	const MaterialVec& materials = matMgr.materialsBetween( a , b  ) ;
// 	
//   std::cout<<"Calculating material average."<<std::endl;
// 	MaterialData mat = ( materials.size() > 1  ? matMgr.createAveragedMaterial( materials ) : materials[0].first  ) ;
// 	
// 	std::cout << " ####### found materials between points : " << a << " and " << b << " : " ;
// 	for( unsigned i=0,n=materials.size();i<n;++i){
// 		std::cout <<  materials[i].first.name() << "[" <<   materials[i].second << "], " ;
// 	}
// 	std::cout << std::endl ;
// 	std::cout << "   averaged material : " << mat << std::endl ;
// 	
// 	handle->addMaterial( "VXDSupportMaterial", mat.A(), mat.Z() , mat.density()/(dd4hep::kg/(dd4hep::g*dd4hep::m3)) , mat.radiationLength()/dd4hep::mm , mat.interactionLength()/dd4hep::mm )  ; 
	
	
	vxdDE.addExtension< GearHandle >( handle ) ;

      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //========= SIT ==============================================================================
     
      try{ 

        DetElement sitDE = description.detector("InnerTrackerBarrel") ;
	
	ZPlanarData* sit = sitDE.extension<ZPlanarData>() ;
	
	//      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
	int sitType =  gear::ZPlanarParameters::CCD ;
  
	gear::ZPlanarParametersImpl* gearSIT = new gear::ZPlanarParametersImpl( sitType, sit->rInnerShell/dd4hep::mm,  sit->rOuterShell/dd4hep::mm,
										sit->zHalfShell/dd4hep::mm , sit->gapShell/dd4hep::mm , 0.  ) ;
                    
	std::vector<int> n_sensors_per_ladder ;
	
	for(unsigned i=0,n=sit->layers.size() ; i<n; ++i){
	  
	  const rec::ZPlanarData::LayerLayout& l = sit->layers[i] ;
	  
	  // FIXME set rad lengths to 0 -> need to get from dd4hep ....
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

	DetElement setDE = description.detector("OuterTrackerBarrel") ;
	
	ZPlanarData* set = setDE.extension<ZPlanarData>() ;
	
	//      ZPlanarParametersImpl (int type, double shellInnerRadius, double shellOuterRadius, double shellHalfLength, double shellGap, double shellRadLength)
	int setType =  gear::ZPlanarParameters::CCD ;
	gear::ZPlanarParametersImpl* gearSET = new gear::ZPlanarParametersImpl( setType, set->rInnerShell/dd4hep::mm,  set->rOuterShell/dd4hep::mm,
										set->zHalfShell/dd4hep::mm , set->gapShell/dd4hep::mm , 0.  ) ;
	std::vector<int> n_sensors_per_ladder ;
	//n_sensors_per_ladder.clear() ;
	
	for(unsigned i=0,n=set->layers.size() ; i<n; ++i){
	  
	  const rec::ZPlanarData::LayerLayout& l = set->layers[i] ;
	  
	  // FIXME set rad lengths to 0 -> need to get from dd4hep ....
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

	DetElement iteDE = description.detector("InnerTrackerEndcap") ;
	
	ZDiskPetalsData* iteData = iteDE.extension<ZDiskPetalsData>() ;
	
  gear::GearParametersImpl* gearFTD = new gear::GearParametersImpl();
  
  std::vector<double> thicknesses;
  std::vector<double> innerRadii;
  std::vector<double> outerRadii;
  std::vector<double> zPositions;
  
	
	for(unsigned i=0,n=iteData->layers.size() ; i<n; ++i){
    
    const rec::ZDiskPetalsData::LayerLayout& l = iteData->layers[i] ;
    
    thicknesses.push_back(l.thicknessSupport/ dd4hep::mm);
    innerRadii.push_back( l.distanceSensitive/ dd4hep::mm);
    outerRadii.push_back( (l.distanceSensitive + l.lengthSensitive)/ dd4hep::mm);
    zPositions.push_back( l.zPosition/ dd4hep::mm);
    


	}
	
	
	//Append OuterTracker
	DetElement oteDE = description.detector("OuterTrackerEndcap") ;
  ZDiskPetalsData* oteData = oteDE.extension<ZDiskPetalsData>() ;
   
  
  for(unsigned i=0,n=oteData->layers.size() ; i<n; ++i){
    
    const rec::ZDiskPetalsData::LayerLayout& l = oteData->layers[i] ;
    
    thicknesses.push_back(l.thicknessSupport/ dd4hep::mm);
    innerRadii.push_back( l.distanceSensitive/ dd4hep::mm);
    outerRadii.push_back( (l.distanceSensitive + l.lengthSensitive)/ dd4hep::mm);
    zPositions.push_back( l.zPosition/ dd4hep::mm);
    
    
    
  }
	

	gearFTD->setDoubleVals("FTDDiskSupportThickness"  ,  thicknesses) ;
  gearFTD->setDoubleVals("FTDInnerRadius" , innerRadii) ;
  gearFTD->setDoubleVals("FTDOuterRadius"  , outerRadii ) ;
  gearFTD->setDoubleVals("FTDZCoordinate" ,zPositions) ;
  
  //attach gear handle to inner tracker
  iteDE.addExtension< GearHandle >( new GearHandle( gearFTD, "FTD" ) ) ;
  
  
      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //============================================================================================
      
      try {
	
	DetElement coilDE = description.detector("Solenoid") ;
	
	gear::GearParametersImpl* gearCOIL = new gear::GearParametersImpl();
	
	Tube coilTube = Tube( coilDE.volume().solid() )  ;
	
	gearCOIL->setDoubleVal("Coil_cryostat_inner_radius" , coilTube->GetRmin()/ dd4hep::mm ) ;
	gearCOIL->setDoubleVal("Coil_cryostat_outer_radius" , coilTube->GetRmax()/ dd4hep::mm ) ;
	gearCOIL->setDoubleVal("Coil_cryostat_half_z"       , coilTube->GetDZ()/ dd4hep::mm ) ;
	
	coilDE.addExtension< GearHandle >( new GearHandle( gearCOIL, "CoilParameters" ) ) ;
	
      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      } 

      //============================================================================================
      try {

	DetElement tubeDE = description.detector("Tube") ;
	
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
	  
	  // FIXME set rad lengths to 0 -> need to get from dd4hep ....
	}
	
	gearTUBE->setDoubleVals("RInner" , rInner ) ;
	gearTUBE->setDoubleVals("ROuter" , rOuter ) ;
	gearTUBE->setDoubleVals("Z"      , zStart ) ;
	
	
	tubeDE.addExtension< GearHandle >( new GearHandle( gearTUBE, "Beampipe" ) ) ;
      
      } catch( std::runtime_error& e ){  
	std::cerr << " >>>> " << e.what() << std::endl ;
      }

      //========= CALO ==============================================================================

      //**********************************************************
      //*  gear interface w/ LayeredCalorimeterData extension
      //**********************************************************

      std::map< std::string, std::string > caloMap ;
      caloMap["HCalBarrel"] = "HcalBarrelParameters"  ; 
      caloMap["ECalBarrel"] = "EcalBarrelParameters" ;
      caloMap["ECalEndcap"] = "EcalEndcapParameters" ;
      caloMap["ECalPlug"]   = "EcalPlugParameters" ;
      caloMap["YokeBarrel"] = "YokeBarrelParameters" ;
      caloMap["YokeEndcap"] = "YokeEndcapParameters" ;
      caloMap["YokePlug"]   = "YokePlugParameters" ;
      caloMap["HCalBarrel"] = "HcalBarrelParameters" ;
      caloMap["HCalEndcap"] = "HcalEndcapParameters" ;
      caloMap["HCalRing"]   = "HcalRingParameters" ;
      caloMap["LumiCal"]	    = "LcalParameters" ;
//       caloMap["LHCal"]	    = "LHcalParameters" ;
      caloMap["BeamCal"]    = "BeamCalParameters" ;
      
      for(  std::map< std::string, std::string >::const_iterator it = caloMap.begin() ; it != caloMap.end() ; ++it ){

      

	try {

	  DetElement caloDE = description.detector( it->first ) ;
	
	  LayeredCalorimeterData* calo = caloDE.extension<LayeredCalorimeterData>() ;
	  
	  gear::CalorimeterParametersImpl* gearCalo = 
	    ( calo->layoutType == LayeredCalorimeterData::BarrelLayout  ?
	      new gear::CalorimeterParametersImpl(  calo->extent[0]/dd4hep::mm, calo->extent[3]/dd4hep::mm, calo->inner_symmetry, calo->inner_phi0 )  :
	      //CalorimeterParametersImpl (double rMin, double zMax, int symOrder=8, double phi0=0.0) - C'tor for a cylindrical (octagonal) BARREL calorimeter.
	      new gear::CalorimeterParametersImpl(  calo->extent[0]/dd4hep::mm,  calo->extent[1]/dd4hep::mm,  calo->extent[2]/dd4hep::mm, calo->outer_symmetry, calo->outer_phi0 )   ) ;
	  //CalorimeterParametersImpl (double rMin, double rMax, double zMin, int symOrder=2, double phi0=0.0) - C'tor for a cylindrical (octagonal) ENDCAP calorimeter. 
	  
	  for( unsigned i=0, nL = calo->layers.size() ; i <nL ; ++i ){
	    
	    LayeredCalorimeterData::Layer& l = calo->layers[i] ;
	    
            
            //Do some arithmetic to get thicknesses and (approximate) absorber thickneses from "new" rec structures
            //The positioning should come out right, but the absorber thickness should be overestimated due to the presence of 
            //other less dense material
	    if( i == 0 ) {
            //First layer is positioned with only its inner thickness taken into account
	      gearCalo->layerLayout().positionLayer( l.distance/dd4hep::mm, (l.inner_thickness+l.sensitive_thickness/2.)/dd4hep::mm , 
						     l.cellSize0/dd4hep::mm, l.cellSize1/dd4hep::mm, (l.inner_thickness-l.sensitive_thickness/2.)/dd4hep::mm ) ;
	    }else if ( i == nL -1 ) {
                    //Need to handle outermost layer differently; add its outer thickness
                    //This gives the right extent, but the "wrong" distance for the last layer
            	      gearCalo->layerLayout().addLayer((l.inner_thickness+l.outer_thickness+calo->layers[i-1].outer_thickness-calo->layers[i-1].sensitive_thickness/2.)/dd4hep::mm , 
									    l.cellSize0/dd4hep::mm, l.cellSize1/dd4hep::mm, (l.inner_thickness+l.outer_thickness-l.sensitive_thickness+calo->layers[i-1].outer_thickness-calo->layers[i-1].sensitive_thickness/2.)/dd4hep::mm) ;
        }else{
            //All other layers need to be added taking into account the previous layer outer thicknesses
	      gearCalo->layerLayout().addLayer((l.inner_thickness+l.sensitive_thickness/2.+calo->layers[i-1].outer_thickness-calo->layers[i-1].sensitive_thickness/2.)/dd4hep::mm , 
									    l.cellSize0/dd4hep::mm, l.cellSize1/dd4hep::mm, (l.inner_thickness-l.sensitive_thickness/2.+calo->layers[i-1].outer_thickness-calo->layers[i-1].sensitive_thickness/2.)/dd4hep::mm) ;
	    }
	    
	  }
	
	  if( it->first == "HCalBarrel" ){
	    // additional parameters needed by MarlinPandora
	    gearCalo->setIntVal("Hcal_outer_polygon_order"   , calo->outer_symmetry  ) ;
	    gearCalo->setDoubleVal("Hcal_outer_polygon_phi0" ,  calo->outer_phi0 ) ;
	  }
		  


	  caloDE.addExtension< GearHandle >( new GearHandle( gearCalo, it->second ) ) ;

	} catch( std::runtime_error& e ){  
	  std::cerr << " >>>> " << e.what() << std::endl ;
    
	} 

      } // calo loop 



      //**********************************************************
      //*  test gear interface w/ LayeredExtensionImpl extension
      //**********************************************************
      
      // DetElement calo2DE = description.detector("EcalBarrel") ;
      
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



      // --- Detector::apply() expects return code 1 if all went well ! ----
      return 1;
    }
  }
}
DECLARE_APPLY( GearForCLIC, dd4hep::rec::createGearForCLIC )


