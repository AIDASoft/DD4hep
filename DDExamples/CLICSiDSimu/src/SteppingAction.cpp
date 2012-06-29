#include "SteppingAction.h"
#include "EventAction.h"
#include "G4Step.hh"

SteppingAction::SteppingAction(EventAction* evt) : eventaction(evt)					 
{ }

SteppingAction::~SteppingAction()
{ }

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{  
  static G4Material* SiMaterial = G4Material::GetMaterial("Silicon");
  static G4Material* TPCGasMaterial = G4Material::GetMaterial("Argon");  

    // get volume of the current step
  G4VPhysicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4Material* material = volume->GetLogicalVolume()->GetMaterial();
  
  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit();
  
  G4double stepl = 0.;
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) stepl = aStep->GetStepLength();
      
  if (material == SiMaterial || material == TPCGasMaterial) { 
    eventaction->SumSensitive(edep, stepl, 0.0);
  }
  else {
    eventaction->SumSupport(edep, stepl, 0.0);
  }
    
  //example of saving random number seed of this event, under condition
  //// if (condition) G4RunManager::GetRunManager()->rndmSaveThisEvent(); 
}
