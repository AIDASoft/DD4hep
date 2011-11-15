
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExPrimaryGeneratorMessenger_h
#define ILDExPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class ILDExPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExPrimaryGeneratorMessenger: public G4UImessenger
{
public:
  ILDExPrimaryGeneratorMessenger(ILDExPrimaryGeneratorAction*);
  virtual ~ILDExPrimaryGeneratorMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  ILDExPrimaryGeneratorAction* ILDExAction;
  G4UIdirectory*               gunDir; 
  G4UIcmdWithAString*          RndmCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

