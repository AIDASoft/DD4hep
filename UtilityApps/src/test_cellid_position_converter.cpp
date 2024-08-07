//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// @author F.Gaede, DESY
// @date May, 2017
//==========================================================================

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/DDTest.h"

#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/BitFieldCoder.h"
#include "DDRec/CellIDPositionConverter.h"

#include "lcio.h"
#include "IO/LCReader.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"

#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;
using namespace dd4hep::rec;
using namespace lcio;


static DDTest test( "cellid_position_converter" ) ; 

//=============================================================================

const double epsilon = dd4hep::micrometer ;
const int maxHit = 100 ;


double dist( const Position& p0, const Position& p1 ){
  Position p2 = p1 - p0 ;
  return p2.r() ;
} 


struct TestCounter{
  unsigned passed{} ;
  unsigned failed{} ;
} ;

struct TestCounters{
  TestCounter position{} ;
  TestCounter cellid{} ;
};

typedef std::map<std::string, TestCounters > TestMap ;



int main_wrapper(int argc, char** argv ){

  if( argc < 3 ) {
    std::cout << " usage: test_cellid_position_converter compact.xml lcio_file.slcio" << std::endl ;
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;

  Detector& description = Detector::getInstance();

  description.fromCompact( inFile );

  CellIDPositionConverter idposConv( description )  ;

  
  //---------------------------------------------------------------------
  //    open lcio file with SimCalorimeterHits
  //---------------------------------------------------------------------

  std::string lcioFileName = argv[2] ;

  LCReader* rdr = LCFactory::getInstance()->createLCReader() ;
  rdr->open( lcioFileName ) ;

  LCEvent* evt = 0 ;


  // use only hits from these collections
  std::set< std::string > subset = {} ;
  //{"BeamCalCollection" } ; //{"EcalBarrelCollection" } ; //{"HcalBarrelRegCollection"} ; 


  // ignore all hits from these collections
  std::set< std::string > subsetIgnore = {"HCalBarrelRPCHits","HCalECRingRPCHits","HCalEndcapRPCHits" } ;
  
  TestMap tMap ;

  while( ( evt = rdr->readNextEvent() ) != 0 ){

    const std::vector< std::string >& colNames = *evt->getCollectionNames() ;

    for(unsigned icol=0, ncol = colNames.size() ; icol < ncol ; ++icol ){

      LCCollection* col =  evt->getCollection( colNames[ icol ] ) ;

      std::string typeName = col->getTypeName() ;

      if( typeName != lcio::LCIO::SIMCALORIMETERHIT )
        continue ;

      if( !subset.empty() && subset.find( colNames[icol] ) ==  subset.end()  ) 
	continue ;

      if( !subsetIgnore.empty() && subsetIgnore.find( colNames[icol] ) !=  subsetIgnore.end()  ) 
       	continue ;

      std::cout << "  -- testing collection : " <<  colNames[ icol ] << std::endl ;

      std::string cellIDEcoding = col->getParameters().getStringVal("CellIDEncoding") ;
      
      dd4hep::BitFieldCoder idDecoder0( cellIDEcoding ) ;
      dd4hep::BitFieldCoder idDecoder1( cellIDEcoding ) ;

      int nHit = std::min( col->getNumberOfElements(), maxHit )  ;
     
      
      for(int i=0 ; i< nHit ; ++i){
	
        SimCalorimeterHit* sHit = (SimCalorimeterHit*) col->getElementAt(i) ;
	
        dd4hep::CellID id0 = sHit->getCellID0() ;
        dd4hep::CellID id1 = sHit->getCellID1() ;

	dd4hep::CellID id =  idDecoder0.toLong( id0 , id1 ) ;                                                                                                                  

	Position point( sHit->getPosition()[0]* dd4hep::mm , sHit->getPosition()[1]* dd4hep::mm ,  sHit->getPosition()[2]* dd4hep::mm ) ;
	

	// ====== test cellID to position and position to cellID conversion  ================================
	DetElement det = idposConv.findDetElement( point ) ;
	
	CellID idFromDecoder = idposConv.cellID( point ) ;

	std::stringstream sst ;
	sst << " compare ids: " << det.name() << " " <<  idDecoder0.valueString(id) << "  -  " << idDecoder1.valueString(idFromDecoder) ;

	test( id, idFromDecoder,  sst.str() ) ;
	
	if( ! strcmp( test.last_test_status() , "PASSED" ) )
	  tMap[ colNames[icol] ].cellid.passed++ ;
	else
	  tMap[ colNames[icol] ].cellid.failed++ ;
	  
	Position pointFromDecoder = idposConv.position( id ) ;

	double d = dist(pointFromDecoder, point)  ;
	std::stringstream sst1 ;
	sst1 << " dist " << d << " ( " <<  point << " ) - ( " << pointFromDecoder << " )  - detElement: "
	    << det.name() ;

	test( d < epsilon , true  , sst1.str()  ) ;
	
	if( ! strcmp( test.last_test_status() , "PASSED" ) )
	  tMap[ colNames[icol] ].position.passed++ ;
	else
	  tMap[ colNames[icol] ].position.failed++ ;

      }
    }
    
  }

  // print summary

  std::cout << "\n ----------------------- summary  ----------------------   " << std::endl ;

  
  for( const auto& res : tMap )  {
    const std::string& name = res.first;
    unsigned total      = res.second.position.passed+res.second.position.failed ;
    unsigned pos_failed = res.second.position.failed ;
    unsigned id_failed  = res.second.cellid.failed ;

    
    printf(" %-30s \t  failed position: %5d  failed cellID:  %5d    of total: %5d   \n",
           name.c_str(), pos_failed , id_failed, total ) ;

  }
  std::cout << "\n -------------------------------------------------------- " << std::endl ;

  
  return 0;
}

//=============================================================================
#include "main.h"
