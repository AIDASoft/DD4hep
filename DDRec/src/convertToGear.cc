#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Fields.h"

#include "TGeoManager.h"

//---- GEAR ----
#include "gear/GEAR.h"
#include "gearxml/GearXML.h"

#include "gearimpl/Util.h"
#include "gearimpl/ConstantBField.h"
#include "gearimpl/GearMgrImpl.h"

#include "geartgeo/TGeoGearPointProperties.h"
#include "geartgeo/TGeoGearDistanceProperties.h"

#include "DDRec/DDGear.h"

using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace dd4hep ;

//=============================================================================

int main(int argc, char** argv ){
    
  if( argc < 3 ) {
    std::cout << " usage: convertToGear plugin compact.xml [gear_file.xml]" << std::endl 
	      << "  plugin: name of a plugin with a signature \"long int (LCDD*, int, char**)\" \n " 
	      << "  or \"default\" " << std::endl ; 
    
    exit(1) ;
  }

  std::string pluginName =  argv[1] ;

  std::string inFile =  argv[2] ;

  std::string outFile = ( argc>3  ?  argv[3]  : ""  ) ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );

  gear::GearMgr* gearMgr = ( pluginName == "default" ) ?  createGearMgr( lcdd )  :   createGearMgr( lcdd , pluginName )  ;
 
  //----------------------------------------------------------------------------------

  // std::cout << "  ***************************** GEAR parameters **************************************** " << std::endl ;
  // std::cout <<         *gearMgr ;
  // std::cout << "  ************************* End of GEAR parameters ************************************* " << std::endl ;

  //----------------------------------------------------------------------------------
  

  if( outFile.empty() ){

    outFile = "gear_"  + gearMgr->getDetectorName() + ".xml" ;
  }
    
  gear::GearXML::createXMLFile ( gearMgr, outFile ) ;
 

  std::cout << "  ************************************************************** " << std::endl ;
  std::cout << "   created gear file : " << outFile << std::endl ;
  std::cout << "  ************************************************************** " << std::endl ;

  return 0;
}

//=============================================================================
