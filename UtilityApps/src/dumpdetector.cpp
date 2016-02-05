// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//
//  Simple program to dump the complete DetElement hierarchy
// 
//  Author     : F.Gaede, CERN/DESY
//  Date       : 07 Nov 2014
//==========================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/DetType.h"
#include "DD4hep/DD4hepUnits.h"

#include "DDRec/Surface.h"
#include "DDRec/DetectorSurfaces.h"
#include "DDRec/DetectorData.h"
#include "DDRec/SurfaceHelper.h"

// C/C++ include files
#include <list>


using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec;
using namespace DDSurfaces ;
using namespace dd4hep ;

//=============================================================================
void printDetectorData( DetElement det ){

  try{ 
    FixedPadSizeTPCData* d = det.extension<FixedPadSizeTPCData>() ; 
    std::cout << *d ;
  } catch(...){}
  try{ 
    ZPlanarData* d = det.extension<ZPlanarData>() ; 
    std::cout << *d ;
  } catch(...){}
  try{ 
    ZDiskPetalsData* d = det.extension<ZDiskPetalsData>() ; 
    std::cout << *d ;
  } catch(...){}
  try{ 
    ConicalSupportData* d = det.extension<ConicalSupportData>() ; 
    std::cout << *d ;
  } catch(...){}
  try{ 
    LayeredCalorimeterData* d = det.extension<LayeredCalorimeterData>() ; 
    std::cout << *d ;
  } catch(...){}

}

void printDetectorSets( std::string name, unsigned int typeFlag ){

  LCDD& lcdd = LCDD::getInstance();
  const std::vector<DetElement>& dets = lcdd.detectors( typeFlag ) ;
  std::cout << " " << name  ;
  for(int i=0,N=dets.size();i<N;++i)  
    std::cout << dets[i].name() << ", " ;
  std::cout << endl ;
} 


//=============================================================================

int main(int argc, char** argv ){
    
  if( argc < 2 ) {
    std::cout << " usage: dumpdetector compact.xml [-s]" 
	      << "  -d :  only print DetectorData objects " 
	      << "  -s :  also print surfaces " 
	      << std::endl ;

    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;


  bool printDetData = ( argc>2 && !strcmp( argv[2] , "-d" ) );

  bool printSurfaces = ( argc>2 && !strcmp( argv[2] , "-s" ) );


  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );

  DetElement world = lcdd.world() ;


  std::cout << "############################################################################### "  << std::endl  ;
  
  Header h = lcdd.header() ;

  std::cout << " detector model : " <<  h.name()  << std::endl 
	    << "    title : "  << h.title() << std::endl 
	    << "    author : " << h.author() << std::endl 
	    << "    status : " << h.status() << std::endl ;


  printDetectorSets( " barrel trackers : " , ( DetType::TRACKER | DetType::BARREL ) ) ; 
  printDetectorSets( " endcap trackers : " , ( DetType::TRACKER | DetType::ENDCAP ) ) ; 

  printDetectorSets( " barrel calorimeters : " , ( DetType::CALORIMETER | DetType::BARREL ) ) ; 
  printDetectorSets( " endcap calorimeters : " , ( DetType::CALORIMETER | DetType::ENDCAP ) ) ; 


  if( printDetData ){

    DD4hep::Geometry::LCDD::HandleMap dets = lcdd.detectors() ;

    for( DD4hep::Geometry::LCDD::HandleMap::const_iterator it = dets.begin() ; it != dets.end() ; ++it ){
      
      DetElement det = it->second ;

      std::cout << " ---------------------------- " << det.name() << " ----------------------------- " << std::endl ;

      DetType type( det.typeFlag() ) ;
      
      std::cout << " ------     " << type << std:: endl ;

      printDetectorData( det ) ;

    }
    
    std::cout << "############################################################################### "  << std::endl  ;

    return 0;
  }




  DD4hep::Geometry::LCDD::HandleMap sensDet = lcdd.sensitiveDetectors() ;


  std::cout << "############################################################################### "  << std::endl  
            << "     sensitive detectors:     " << std::endl ;

  for( DD4hep::Geometry::LCDD::HandleMap::const_iterator it = sensDet.begin() ; it != sensDet.end() ; ++it ){

    SensitiveDetector sDet = it->second ;
    std::cout << "     " << it->first << " : type = " << sDet.type() << std::endl ;
  }


  std::cout << "############################################################################### "  << std::endl  << std::endl  ;
	

  //------------------ breadth first tree traversal ---------
  std::list< DetElement > dets ;
  std::list< DetElement > daugs ; 
  std::list< DetElement > gdaugs ; 
  daugs.push_back( world ) ;
  while( ! daugs.empty() ) {
    for( std::list< DetElement >::iterator li=daugs.begin() ; li != daugs.end() ; ++li ){
      DetElement dau = *li ;
      DetElement::Children chMap = dau.children() ;
      for ( DetElement::Children::const_iterator it=chMap.begin() ; it != chMap.end() ; ++it ){
        DetElement de = (*it).second ;
        gdaugs.push_back( de ) ;
      }  
    }
    dets.splice( dets.end() , daugs ) ;
    daugs.splice( daugs.end() , gdaugs ) ;
  }
  //------------------ end tree traversal ---------
  

  for (  std::list< DetElement >::const_iterator it=dets.begin() ; it != dets.end() ; ++it ){

    DetElement de = (*it) ;
    
    DetElement mother = de.parent() ;
    unsigned parentCount = 0 ;
    while( mother.isValid() ) {
      mother = mother.parent() ;
      ++parentCount ;
    } 

    SurfaceHelper surfMan( de ) ;

    const SurfaceList& sL = surfMan.surfaceList() ;

    std::cout << "DetElement: " ;
    
    for(unsigned i=0 ; i < parentCount ; ++i ) std::cout << "\t" ;

    std::cout << de.name() << "[ path: "<< de.placementPath ()  <<  "] (id: " << de.id() << ") - sens type : " << lcdd.sensitiveDetector( de.name() ).type() << "\t surfaces : " <<  ( sL.empty() ? 0 : sL.size()  ) << std::endl ;


    //    printDetectorData( de ) ;

    if( printSurfaces ){
      for( SurfaceList::const_iterator sit = sL.begin() ; sit != sL.end() ; ++sit ){
	const ISurface* surf =  *sit ;
	std::cout << " ------------------------- " 
		  << " surface: "  << *surf         << std::endl
		  << " ------------------------- "  << std::endl ;
      }
    }
  }

  std::cout << "############################################################################### "  << std::endl  << std::endl  ;

  //	FixedPadSizeTPCData* tpc = tpcDE.extension<FixedPadSizeTPCData>() ;

  return 0;
}





//=============================================================================
