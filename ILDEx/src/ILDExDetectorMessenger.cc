
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ILDExDetectorMessenger.hh"

#include "ILDExDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExDetectorMessenger::ILDExDetectorMessenger(
                                           ILDExDetectorConstruction* ILDExDet)
:ILDExDetector(ILDExDet)
{ 
  ILDExDir = new G4UIdirectory("/ILDExDir/");
  ILDExDir->SetGuidance("UI commands of this example");
  
  detDir = new G4UIdirectory("/ILDExDir/det/");
  detDir->SetGuidance("detector control");

  UpdateCmd = new G4UIcmdWithoutParameter("/ILDExDir/det/update",this);
  UpdateCmd->SetGuidance("Update detector geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  UpdateCmd->AvailableForStates(G4State_Idle);
      
  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/ILDExDir/det/setField",this);  
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Z direction.");
  MagFieldCmd->SetParameterName("Bz",false);
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExDetectorMessenger::~ILDExDetectorMessenger()
{
  delete MagFieldCmd;
  delete detDir;
  delete ILDExDir;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  
  if( command == UpdateCmd )
   { ILDExDetector->UpdateGeometry(); }

  if( command == MagFieldCmd )
   { ILDExDetector->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
