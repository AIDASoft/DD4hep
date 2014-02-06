#include "DD4hep/LCDD.h"
#include "DD4hep/TGeoUnits.h"
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

#include "DDGear.h"

using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace tgeo ;

//=============================================================================

int main(int argc, char** argv ){
    
  if( argc < 2 ) {
    std::cout << " usage: convertToGear compact.xml [gear_file.xml]" << std::endl ;
    exit(1) ;
  }

  std::string inFile =  argv[1] ;

  std::string outFile = ( argc>2  ?  argv[2]  : ""  ) ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );

  gear::GearMgr* gearMgr = createGearMgr( lcdd ) ;

  //----------------------------------------------------------------------------------

  std::cout << "  ***************************** GEAR parameters **************************************** " << std::endl ;
  std::cout <<         *gearMgr ;
  std::cout << "  ************************* End of GEAR parameters ************************************* " << std::endl ;

  //----------------------------------------------------------------------------------
  

  if( outFile.empty() ){

    outFile = "gear_"  + gearMgr->getDetectorName() + ".xml" ;
  }
    

  gear::GearXML::createXMLFile ( gearMgr, outFile ) ;

  return 0;
}

//=============================================================================
