
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ILDExRunAction.h"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

//--- lcio --
#include "lcio.h"
#include "IMPL/LCRunHeaderImpl.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExRunAction::ILDExRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExRunAction::~ILDExRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExRunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  // keep the run around:
  g4run = aRun ;

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);

    
  //initialize cumulative quantities
  //

  sumESupport = sum2ESupport = sumESensitive = sum2ESensitive = 0.;
  sumLSupport = sum2LSupport = sumLSensitive = sum2LSensitive = 0.; 
  sumAngleSupport = sum2AngleSupport = sumAngleSensitive = sum2AngleSensitive = 0.;


  // --- write an lcio::RunHeader ---------
  lcio::LCRunHeaderImpl* rh =  new lcio::LCRunHeaderImpl ;
  rh->setRunNumber( aRun->GetRunID()  ) ;
  rh->setDetectorName( runData.detectorName ) ;
  runData.lcioWriter->writeRunHeader( rh ) ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExRunAction::fillPerEvent(G4double ESupport, G4double ESensitive,
                                  G4double LSupport, G4double LSensitive,
				  G4double AngleSupport, G4double AngleSensitive)
{
  //accumulate statistic
  //
  sumESupport += ESupport;  sum2ESupport += ESupport*ESupport;
  sumESensitive += ESensitive;  sum2ESensitive += ESensitive*ESensitive;
  
  sumLSupport += LSupport;  sum2LSupport += LSupport*LSupport;
  sumLSensitive += LSensitive;  sum2LSensitive += LSensitive*LSensitive;  

  sumAngleSupport += AngleSupport;  sum2AngleSupport += AngleSupport*AngleSupport;
  sumAngleSensitive += AngleSensitive;  sum2AngleSensitive += AngleSensitive*AngleSensitive;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExRunAction::EndOfRunAction(const G4Run* aRun)
{
  G4int NbOfEvents = aRun->GetNumberOfEvent();
  if (NbOfEvents == 0) return;
  
  //compute statistics: mean and rms
  //
  sumESupport /= NbOfEvents; sum2ESupport /= NbOfEvents;
  G4double rmsESupport = sum2ESupport - sumESupport*sumESupport;
  if (rmsESupport >0.) rmsESupport = std::sqrt(rmsESupport); else rmsESupport = 0.;
  
  sumESensitive /= NbOfEvents; sum2ESensitive /= NbOfEvents;
  G4double rmsESensitive = sum2ESensitive - sumESensitive*sumESensitive;
  if (rmsESensitive >0.) rmsESensitive = std::sqrt(rmsESensitive); else rmsESensitive = 0.;
  
  sumLSupport /= NbOfEvents; sum2LSupport /= NbOfEvents;
  G4double rmsLSupport = sum2LSupport - sumLSupport*sumLSupport;
  if (rmsLSupport >0.) rmsLSupport = std::sqrt(rmsLSupport); else rmsLSupport = 0.;
  
  sumLSensitive /= NbOfEvents; sum2LSensitive /= NbOfEvents;
  G4double rmsLSensitive = sum2LSensitive - sumLSensitive*sumLSensitive;
  if (rmsLSensitive >0.) rmsLSensitive = std::sqrt(rmsLSensitive); else rmsLSensitive = 0.;

  sumAngleSupport /= NbOfEvents; sum2AngleSupport /= NbOfEvents;
  G4double rmsAngleSupport = sum2AngleSupport - sumAngleSupport*sumAngleSupport;
  if (rmsAngleSupport >0.) rmsAngleSupport = std::sqrt(rmsAngleSupport); else rmsAngleSupport = 0.;
  
  sumAngleSensitive /= NbOfEvents; sum2AngleSensitive /= NbOfEvents;
  G4double rmsAngleSensitive = sum2AngleSensitive - sumAngleSensitive*sumAngleSensitive;
  if (rmsAngleSensitive >0.) rmsAngleSensitive = std::sqrt(rmsAngleSensitive); else rmsAngleSensitive = 0.;

  
  //print
  //
  G4cout
     << "\n--------------------End of Run------------------------------\n"
     << "\n mean Energy in Support  : " << G4BestUnit(sumESupport,"Energy")
     << " +- "                          << G4BestUnit(rmsESupport,"Energy")  
     << "\n mean Energy in Sensitive: " << G4BestUnit(sumESensitive,"Energy")
     << " +- "                          << G4BestUnit(rmsESensitive,"Energy")
     << G4endl;
     
  G4cout
     << "\n mean trackLength in Support   : " << G4BestUnit(sumLSupport,"Length")
     << " +- "                               << G4BestUnit(rmsLSupport,"Length")  
     << "\n mean trackLength in Sensitive : " << G4BestUnit(sumLSensitive,"Length")
     << " +- "                               << G4BestUnit(rmsLSensitive,"Length")
     << G4endl;

  G4cout
     << "\n mean Angle in Support  : " << G4BestUnit(sumAngleSupport,"Angle")
     << " +- "                          << G4BestUnit(rmsAngleSupport,"Angle")  
     << "\n mean Angle in Sensitive: " << G4BestUnit(sumAngleSensitive,"Angle")
     << " +- "                          << G4BestUnit(rmsAngleSensitive,"Angle")
     << "\n------------------------------------------------------------\n"
     << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
