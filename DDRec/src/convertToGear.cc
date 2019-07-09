//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#include "DD4hep/Detector.h"
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
using namespace dd4hep ;
using namespace dd4hep::detail;
using namespace dd4hep::rec ;

//=============================================================================

int main(int argc, char** argv ){
    
  if( argc < 3 ) {
    std::cout << " usage: convertToGear plugin compact.xml [gear_file.xml]" << std::endl 
	      << "  plugin: name of a plugin with a signature \"long int (Detector*, int, char**)\" \n " 
	      << "  e.g. GearForILD (same as 'default'), GearForCLIC, ..." << std::endl ; 
    
    exit(1) ;
  }

  std::string pluginName =  argv[1] ;

  std::string inFile =  argv[2] ;

  std::string outFile = ( argc>3  ?  argv[3]  : ""  ) ;

  Detector& description = Detector::getInstance();

  description.fromCompact( inFile );

  gear::GearMgr* gearMgr = ( pluginName == "default" ) ?  createGearMgr( description )  :   createGearMgr( description , pluginName )  ;
 
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
