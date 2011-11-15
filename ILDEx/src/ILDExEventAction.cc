
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ILDExEventAction.hh"

#include "ILDExRunAction.hh"
#include "ILDExEventActionMessenger.hh"

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExEventAction::ILDExEventAction(ILDExRunAction* run)
:runAct(run),printModulo(1),eventMessenger(0)
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
	  
  }
 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
