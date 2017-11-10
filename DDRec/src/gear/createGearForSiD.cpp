#include "DD4hep/Detector.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h" 

#include "DDRec/DetectorData.h"
#include "DDRec/DDGear.h"
#include "DDRec/MaterialManager.h"

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
     *  @author F.Gaede, CERN/DESY
     *  @date Oct 2014
     *  @version $Id$
     *
     *  Minimal plugin for SiD solely to be used with LCCalibration
     *  adapted from the ILD and CLIC plugins
     *  @author D.Protopopescu, Glasgow
     *  @date Nov 2017
     *
     */
    
    static long createGearForSiD(Detector& description, int /*argc*/, char** /*argv*/) {
      
      std::cout << " **** running plugin createGearForSiD ! " <<  std::endl ;
      
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
	    
      //========= CALO ONLY ==============================================================

      //**********************************************************
      //*  gear interface w/ LayeredCalorimeterData extension
      //**********************************************************

      std::map< std::string, std::string > caloMap ;
      caloMap["HCalBarrel"] = "HcalBarrelParameters" ; 
      caloMap["HCalEndcap"] = "HcalEndcapParameters" ;
      caloMap["ECalBarrel"] = "EcalBarrelParameters" ;
      caloMap["ECalEndcap"] = "EcalEndcapParameters" ;
      caloMap["MuonBarrel"] = "YokeBarrelParameters" ;
      caloMap["MuonEndcap"] = "YokeEndcapParameters" ;
      caloMap["LumiCal"]    = "LcalParameters" ;
      caloMap["BeamCal"]    = "BeamCalParameters" ;
      
      for( std::map< std::string, std::string >::const_iterator it = caloMap.begin() ; it != caloMap.end() ; ++it ){

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
	    } else if ( i == nL -1 ) {
	      //Need to handle outermost layer differently; add its outer thickness
	      //This gives the right extent, but the "wrong" distance for the last layer
	      gearCalo->layerLayout().addLayer((l.inner_thickness+l.outer_thickness+calo->layers[i-1].outer_thickness-calo->layers[i-1].sensitive_thickness/2.)/dd4hep::mm , 
					       l.cellSize0/dd4hep::mm, l.cellSize1/dd4hep::mm, (l.inner_thickness+l.outer_thickness-l.sensitive_thickness+calo->layers[i-1].outer_thickness-calo->layers[i-1].sensitive_thickness/2.)/dd4hep::mm) ;
	    } else {
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

      // --- Detector::apply() expects return code 1 if all went well ! ----
      return 1;
    }
  }
}
DECLARE_APPLY( GearForSiD, dd4hep::rec::createGearForSiD )


