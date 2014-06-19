#include "TEveManager.h"
#include "TEveStraightLineSet.h"
#include "TEvePointSet.h"
#include <iostream>

#include "lcio.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/TrackerHit.h"
#include "UTIL/Operators.h"

#include "DD4hep/DD4hepUnits.h"

#include "MultiView.h"

using namespace lcio;

//=====================================================================================

template <class T>
TEveElement* createPointSet( lcio::LCCollection* col, const std::string& name, unsigned color=kMagenta, unsigned size=1, unsigned style=4 ) ;

//=====================================================================================

/**Helper class for printing LCIO objects from TEve
 */
template <class T>
class LCIOTObject : public TObject{
  const T* _obj ;
  LCIOTObject() {}
public:
  LCIOTObject( const T* o) : _obj(o) {}
  void Print() {
    std::cout <<  *_obj  ;
  }
} ;


void next_event(){
  
  static int count = 1 ;

  static LCReader* rdr = 0 ;

  std::string lcioFileName = "teve_infile.slcio" ; 

  std::cout <<  " next_event_lcio called ..." << std::endl ;

  if( rdr == 0 && count==1 ){

    rdr = LCFactory::getInstance()->createLCReader() ;

    try{

      rdr->open( lcioFileName ) ;

    }catch(lcio::IOException& e) {

      std::cout << " ------------------------------------------------------------------------------------------------ "     << std::endl
		<<  "*** file " << lcioFileName << " does not exist - can't read LCIO events  !                       "     << std::endl
		<<  "    will display detector geometry only. Link LCIO file to " << lcioFileName << " to display events ! "<< std::endl
		<< " -------------------------------------------------------------------------------------------------"  	
		<< std::endl ;
      ++count ;

      return ;
    }

  } else {

    // nothing to do as inoutfile does not exist:
    return ;
  }

  
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
	  
	MultiView::instance()->ImportEvent(  createPointSet<EVENT::SimTrackerHit>( col , colNames[ icol ]  , kMagenta+2 , 1 , 4  ) ) ; 
      }
      else if( col->getTypeName() == LCIO::SIMCALORIMETERHIT ){   
	  
	MultiView::instance()->ImportEvent(  createPointSet<EVENT::SimCalorimeterHit>( col , colNames[ icol ]  , kMagenta+4 , 1 , 4  ) ) ; 
      }
      else if( col->getTypeName() == LCIO::TRACKERHIT ){   
	  
	MultiView::instance()->ImportEvent(  createPointSet<EVENT::TrackerHit>( col , colNames[ icol ]  , kBlue+2 , 1 , 4  ) ) ; 
      }
      else if( col->getTypeName() == LCIO::CALORIMETERHIT ){   
	  
	MultiView::instance()->ImportEvent(  createPointSet<EVENT::CalorimeterHit>( col , colNames[ icol ]  , kBlue+4 , 1 , 4  ) ) ; 
      }
	
    }
    
  } else{
    
    std::cout << "WARNING: can't read LCEvent from input file ! " << std::endl ;
  }
  
  
  gEve->Redraw3D();
  
  count += 3 ;
}


//=====================================================================================
template <class T>
TEveElement* createPointSet( lcio::LCCollection* col, const std::string& name, unsigned color, unsigned size, unsigned style ) {
   
  TEvePointSet* ps = new TEvePointSet( name.c_str()  );
  ps->SetOwnIds(kTRUE);
   
  int nHit = col->getNumberOfElements() ;
   
  for( int i=0 ; i< nHit ; ++i ){
     
    T* hit = (T*) col->getElementAt( i ) ;
     
    double pos[3] = {  hit->getPosition()[0], hit->getPosition()[1] , hit->getPosition()[2]  } ;
    // pos[0] = hit->getPosition()[0] ;
    // pos[1] = hit->getPosition()[1] ;
    // pos[2] = hit->getPosition()[2] ;
     
    ps->SetNextPoint( pos[0]*dd4hep::mm , pos[1]*dd4hep::mm, pos[2]*dd4hep::mm );
     
    int id[2] ;
    id[0] = hit->getCellID0()  ;
    id[1] = hit->getCellID1()  ;
     
    ps->SetPointIntIds( id ) ;
    // does work for the point itself ...
    //    ps->SetSourceObject( new LCIOTObject<T>( hit ) ) ; 
  }
   
  ps->SetMarkerColor( color ) ;
  ps->SetMarkerSize( size );
  ps->SetMarkerStyle( style );
   
  return ps;
   
   
}
//=====================================================================================

