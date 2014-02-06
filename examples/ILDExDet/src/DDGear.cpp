#include "DDGear.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/TGeoUnits.h"
#include "DD4hep/Fields.h"

// ROOT
//#include "TGeoManager.h"

//---- GEAR ----
#include "gear/GEAR.h"
#include "gearxml/GearXML.h"

#include "gearimpl/Util.h"
#include "gearimpl/ConstantBField.h"
#include "gearimpl/GearMgrImpl.h"

#include "geartgeo/TGeoGearPointProperties.h"
#include "geartgeo/TGeoGearDistanceProperties.h"


namespace DD4hep{

  using namespace Geometry ;


  gear::GearMgr* createGearMgr( const Geometry::LCDD& lcdd ){


    DetElement world = lcdd.world() ;


    gear::GearMgrImpl* gearMgr = new gear::GearMgrImpl() ;

    gearMgr->setDetectorName(  lcdd.header().name() ) ; 

    std::cout << " **** will convert detector "  <<  lcdd.header().name() << " to Gear \n"
	      << "      Iterating over all subdetectors: " << std::endl ;


    //------------------ SubdetectorParamters ----------------------------------
  
    typedef std::map<std::string, DetElement> DEMap ;
    DEMap chMap = world.children() ;
  
    for ( DEMap::const_iterator it=chMap.begin() ; it != chMap.end() ; ++it ){
    
      DetElement subDet = (*it).second ;
    
      std::cout << "  *** subdetector : " << subDet.name() << std::endl ;
    
      DD4hep::GearHandle* gearH = 0 ;
      try{
      
	gearH = subDet.extension<DD4hep::GearHandle>() ;

      } catch( std::exception& e) {
      
	std::cout << "  ***  " << e.what() << std::endl ;

	continue ; // with next DetElement
      }
    
      // --- check for canonical names of GearHandle objects :
      //   (fixme: will have to iterate over daughters as well ... )

      if     ( gearH->name() == "TPCParameters" )        { gearMgr->setTPCParameters       ( dynamic_cast<gear::TPCParameters*     >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "EcalBarrelParameters" ) { gearMgr->setEcalBarrelParameters( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "EcalEndcapParameters" ) { gearMgr->setEcalEndcapParameters( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "EcalPlugParameters" )   { gearMgr->setEcalPlugParameters  ( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "YokeBarrelParameters" ) { gearMgr->setYokeBarrelParameters( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "YokeEndcapParameters" ) { gearMgr->setYokeEndcapParameters( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "YokePlugParameters" )   { gearMgr->setYokePlugParameters  ( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "HcalBarrelParameters" ) { gearMgr->setHcalBarrelParameters( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "HcalEndcapParameters" ) { gearMgr->setHcalEndcapParameters( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "HcalRingParameters" )   { gearMgr->setHcalRingParameters  ( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "LcalParameters" )       { gearMgr->setLcalParameters      ( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "LHcalParameters" )      { gearMgr->setLHcalParameters     ( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "BeamCalParameters" )    { gearMgr->setBeamCalParameters   ( dynamic_cast<gear::CalorimeterParameters* >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "VXDParameters" )        { gearMgr->setVXDParameters       ( dynamic_cast<gear::ZPlanarParameters*     >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "FTDParameters" )        { gearMgr->setFTDParameters       ( dynamic_cast<gear::FTDParameters*         >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "SITParameters" )        { gearMgr->setSITParameters       ( dynamic_cast<gear::ZPlanarParameters*     >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "SETParameters" )        { gearMgr->setSETParameters       ( dynamic_cast<gear::ZPlanarParameters*     >( gearH->takeGearObject() ) ) ; }
      else if( gearH->name() == "SiPlanesParameters" )   { gearMgr->setSiPlanesParameters  ( dynamic_cast<gear::SiPlanesParameters*    >( gearH->takeGearObject() ) ) ; }  
      else  { 

	gearMgr->setGearParameters( gearH->name() , gearH->takeGearObject() ) ; 
      }  

    }
    std::cout << std::endl ;

    //------------------ Bfield ----------------------------------

    // fixme: for now we just assume a constant field - should be a real field map ...
    double origin[3] = { 0., 0., 0. } ;
    double bfield[3] ;
    OverlayedField ovField = lcdd.field() ;
    ovField.magneticField( origin , bfield  ) ;
  
    gearMgr->setBField( new gear::ConstantBField( gear::Vector3D( bfield[0]/ tgeo::tesla , bfield[1]/ tgeo::tesla , bfield[2] / tgeo::tesla ) ) ) ;


    //----------------- PointProperties and DistanceProperties -------------------------
  
    TGeoManager *geoManager = world.volume()->GetGeoManager();
  
    gearMgr->setPointProperties( new gear::TGeoGearPointProperties( geoManager ) ) ;
  
    gearMgr->setDistanceProperties( new gear::TGeoGearDistanceProperties( geoManager ) ) ;
  
    //----------------------------------------------------------------------------------

    return gearMgr ;

  }
}
 
