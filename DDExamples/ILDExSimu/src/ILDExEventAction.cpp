
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ILDExEventAction.h"
#include "ILDExRunAction.h"
#include "ILDExEventActionMessenger.h"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include "DDG4/Geant4Hits.h" 
#include "DD4hep/VolumeManager.h"
#include "DD4hep/Volumes.h"

//--- lcio
#include "lcio.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "UTIL/Operators.h"
#include "UTIL/ILDConf.h"


#define DEBUG 0


//------ helper functions ------------------

lcio::SimTrackerHitImpl* createSimTrackerHit( DD4hep::Simulation::Geant4TrackerHit* gh ){
  
  lcio::SimTrackerHitImpl* lh = new lcio::SimTrackerHitImpl ;
  
  lh->setCellID0( ( gh->cellID >>    0          ) & 0xFFFFFFFF ) ; 
  lh->setCellID1( ( gh->cellID >> sizeof( int ) ) & 0xFFFFFFFF ) ;

  const double pos[3] = { gh->position.x() , gh->position.y() , gh->position.z()  } ;
  lh->setPosition( pos ) ;

  lh->setEDep( gh->energyDeposit ) ;

  lh->setTime( gh->truth.time ) ;

  lh->setMomentum( gh->momentum.x(), gh->momentum.y() , gh->momentum.z() ) ;

  lh->setPathLength( gh->length ) ;

  return lh ;
} 

//--------------
lcio::SimCalorimeterHitImpl* createSimCalorimeterHit( DD4hep::Simulation::Geant4CalorimeterHit* gh ){
  
  lcio::SimCalorimeterHitImpl* lh = new lcio::SimCalorimeterHitImpl ;
  
  lh->setCellID0( ( gh->cellID >>    0          ) & 0xFFFFFFFF ) ; 
  lh->setCellID1( ( gh->cellID >> sizeof( int ) ) & 0xFFFFFFFF ) ;

  const float pos[3] = { gh->position.x() , gh->position.y() , gh->position.z()  } ;
  lh->setPosition( pos ) ;

  lh->setEnergy( gh->energyDeposit ) ;

  return lh ;
} 

//------------------------------------------


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExEventAction::ILDExEventAction(ILDExRunAction* run, DD4hep::Geometry::LCDD& lcdd )
  :runAct(run),printModulo(1),eventMessenger(0), _lcdd( lcdd ) 
{
  eventMessenger = new ILDExEventActionMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExEventAction::~ILDExEventAction()
{
  delete eventMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExEventAction::BeginOfEventAction(const G4Event* evt)
{  
  G4int evtNb = evt->GetEventID();
  if (evtNb%printModulo == 0) { 
    G4cout << "\n---> Begin of event: " << evtNb << G4endl;
    CLHEP::HepRandom::showEngineStatus();
}
 
 // initialisation per event
 EnergySupport = EnergySensitive = 0.;
 TrackLSupport = TrackLSensitive = 0.;
 AngleSupport  = AngleSensitive = 0.;


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExEventAction::EndOfEventAction(const G4Event* evt)
{


  //accumulates statistic
  //
  runAct->fillPerEvent(EnergySupport, EnergySensitive, TrackLSupport, TrackLSensitive, AngleSupport, AngleSensitive);
  
  //print per event (modulo n)
  //
  G4int evtNb = evt->GetEventID();

  
  if (evtNb%printModulo == 0) {

#if DEBUG

    G4cout << "---> End of event: " << evtNb << G4endl;	

    G4cout
       << "   Support: total energy: " << std::setw(7)
                                        << G4BestUnit(EnergySupport,"Energy")
       << "       total track length: " << std::setw(7)
                                        << G4BestUnit(TrackLSupport,"Length")
       << G4endl
       << "   Sensitive: total energy: " << std::setw(7)
                                        << G4BestUnit(EnergySensitive,"Energy")
       << "       total track length: " << std::setw(7)
                                        << G4BestUnit(TrackLSensitive,"Length")
       << G4endl;
	  
#endif
  }

  G4HCofThisEvent* hce = evt->GetHCofThisEvent() ;

  int nCol = hce->GetNumberOfCollections() ;

  lcio::LCEventImpl* lcEvt = new lcio::LCEventImpl ;

  lcEvt->setEventNumber( evt->GetEventID() ) ;
  //  lcEvt->setRunNumber( evt->GetRunID() ) ;
  
#if DEBUG
  G4cout << " ILDExEventAction::EndOfEventAction:  HCE has " << nCol << " collections : " << G4endl ;
#endif

  for(int i=0 ; i<nCol ; ++i ){

    G4VHitsCollection* hCol =  hce->GetHC( i) ;

#if DEBUG
    G4cout << "    --- " << i << ":  " << hCol->GetName() << "  from SD: " << hCol->GetSDname() << " size: " << hCol->GetSize() ;
#endif


    bool isTracker     = ( hCol->GetSize() ?  dynamic_cast<DD4hep::Simulation::Geant4TrackerHit*    >( hCol->GetHit(0) ) : 0 ) ;
    bool isCalorimeter = ( hCol->GetSize() ?  dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>( hCol->GetHit(0) ) : 0 ) ;
    
    if(  hCol->GetSize()  > 0 ){
      
#if DEBUG
      if     ( isTracker )     G4cout << "  - type Geant4TrackerHit "  << G4endl; 
      else if( isCalorimeter ) G4cout << "  - type Geant4CalorimeterHit " << G4endl ; 
      else                     G4cout << "  - type UNKNOWN "  << G4endl;
#endif
      



 
      // ------ get the readout/cellId description string from first element:

          
      std::string cellIDDesc("") ;
      
      DD4hep::Geometry::VolumeManager vm = _lcdd.volumeManager();

      DD4hep::Geometry::VolumeManager::VolumeID volume_id = dynamic_cast<DD4hep::Simulation::Geant4Hit*>( hCol->GetHit(0) )->cellID ;   
      
      //      std::cout << " looking up placed volume for id " <<  std::hex << volume_id << std::dec  << std::endl ; 

      const DD4hep::Geometry::PlacedVolume & pv =  vm.lookupPlacement ( volume_id ) ;


#if DEBUG
      const  DD4hep::Geometry::DetElement & detElem =  vm.lookupDetElement( volume_id) ;
      if ( detElem.isValid() ) 
	std::cout << "  ILDExEventAction::EndOfEventAction  --- for detector element : "  << detElem.name()  << std::endl ;
      else
	std::cout << "  ILDExEventAction::EndOfEventAction  --- detector element not found "   << std::endl ;
#endif
      
      
      if( pv.isValid() && pv.volume().isSensitive() ) {

	DD4hep::Geometry::Volume            vol    = pv.volume();
	DD4hep::Geometry::SensitiveDetector sd     = vol.sensitiveDetector();
	DD4hep::Geometry::Readout           ro     = sd.readout();
	DD4hep::Geometry::IDDescriptor      iddesc = ro.idSpec();
	

	cellIDDesc = iddesc.toString() ;

      } else {

	std::cout << " **** WARNING: could not get sensitive placedVolume for cellID : " << std::hex << volume_id << std::dec << std::endl ;
      }


      if( isTracker ) { //-----------------------------------------------------------------

	lcio::LCCollectionVec* col = new lcio::LCCollectionVec( lcio::LCIO::SIMTRACKERHIT ) ;

	// the encoder sets the correct cellid encoding string
	// ILDCellIDEncoder<SimTrackerHit> idDec( col ) ;	

	UTIL::CellIDEncoder<SimTrackerHit> idDec( cellIDDesc, col ) ;  

	for(int j=0,N= hCol->GetSize() ; j<N ; ++j) {
	  
	  lcio::SimTrackerHit* h =  createSimTrackerHit(   dynamic_cast<DD4hep::Simulation::Geant4TrackerHit*>( hCol->GetHit(j) )  ) ;

	  col->addElement( h ) ;
	  
#if DEBUG
	  using namespace UTIL ;
	  std::cout << *h << G4endl ;
#endif	
	}
	
	lcEvt->addCollection( col , hCol->GetName() ) ; 
      } //-----------------------------------------------------------------


      if( isCalorimeter ) {

	lcio::LCCollectionVec* col = new lcio::LCCollectionVec( lcio::LCIO::SIMCALORIMETERHIT ) ;
	
	UTIL::CellIDEncoder<SimCalorimeterHit> idDec( cellIDDesc, col ) ;  

	col->setFlag( UTIL::make_bitset32(  LCIO::CHBIT_LONG, LCIO::CHBIT_STEP ) ); 

#if DEBUG
       	std::cout << " setting collection flag:  0x" << std::hex << col->getFlag()  << std::dec << std::endl ;
#endif	

	for(int j=0,N= hCol->GetSize() ; j<N ; ++j) {
	  
	  lcio::SimCalorimeterHit* h =  createSimCalorimeterHit(   dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>( hCol->GetHit(j) )  ) ;

	  col->addElement( h ) ;
	  
#if DEBUG
	  using namespace UTIL ;
	  std::cout << *h << G4endl ;
#endif	
	}
	
	lcEvt->addCollection( col , hCol->GetName() ) ; 
      } //-----------------------------------------------------------------
      
    }
  }

  // --- write the event
  lcEvt->setRunNumber( runAct->g4run->GetRunID()  ) ;
  lcEvt->setDetectorName( runAct->runData.detectorName ) ;
  runAct->runData.lcioWriter->writeEvent( lcEvt ) ;

  
}  


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
