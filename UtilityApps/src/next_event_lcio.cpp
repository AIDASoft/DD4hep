#include "TEveManager.h"
#include "TEveStraightLineSet.h"
#include <iostream>

#include "lcio.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"

#include "DD4hep/TGeoUnits.h"

using namespace lcio;

//=====================================================================================
TEveElement* drawSimTrackerHits( lcio::LCCollection* col, const std::string& name ) ; 

//=====================================================================================


void next_event(){
  
  static int count = 1 ;

  static LCReader* rdr = 0 ;

  std::string lcioFileName = "teve_infile.slcio" ; 


  if( rdr == 0 ){
    rdr = LCFactory::getInstance()->createLCReader() ;
    rdr->open( lcioFileName ) ;
  }

  std::cout <<  " next_event_lcio called ..." << std::endl ;
  
  TEveElementList* tevent = (TEveElementList* ) gEve->GetCurrentEvent() ;
  
  if( tevent ) 
    tevent->DestroyElements() ;
  
  LCEvent* evt =  rdr->readNextEvent() ;

  if( evt != 0 ){

    const std::vector< std::string >& colNames = * evt->getCollectionNames() ;

    for(unsigned icol=0, ncol = colNames.size() ; icol < ncol ; ++icol ){

      LCCollection* col = evt->getCollection( colNames[ icol ] ) ;

      std::cout << " **** reading collection " << colNames[ icol ] << std::endl ;


      if( col->getTypeName() == LCIO::SIMTRACKERHIT ){   

	gEve->AddElement( drawSimTrackerHits( col , colNames[ icol ] ) ) ; 

      }

    }
    
  } else{
    
    std::cout << "WARNING: can't read LCEvent from input file ! " << std::endl ;
  }
  
  
  gEve->Redraw3D();
  
  count += 3 ;
}


//=====================================================================================
TEveElement* drawSimTrackerHits( lcio::LCCollection* col, const std::string& name ) {

  TEveStraightLineSet* ls = new TEveStraightLineSet( name.c_str() );

  int nHit = col->getNumberOfElements() ;

    //    ls->AddLine( o.x(), o.y(), o.z(), ou.x() , ou.y() , ou.z()  ) ;

  for( int i=0 ; i< nHit ; ++i ){

    SimTrackerHit* hit = (SimTrackerHit*) col->getElementAt( i ) ;

    const double* pos = hit->getPosition() ;

    ls->AddMarker( pos[0]*tgeo::mm , pos[1]*tgeo::mm, pos[2]*tgeo::mm );
  }

  ls->SetMarkerColor( kGreen ) ;
  ls->SetMarkerSize(1);
  ls->SetMarkerStyle(4);
  
  //  gEve->AddElement(ls);

  return ls;


}
//=====================================================================================
