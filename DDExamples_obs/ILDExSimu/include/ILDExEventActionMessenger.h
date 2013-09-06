
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExEventActionMessenger_h
#define ILDExEventActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ILDExEventAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExEventActionMessenger: public G4UImessenger
{
public:
  ILDExEventActionMessenger(ILDExEventAction*);
  virtual ~ILDExEventActionMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  ILDExEventAction*     eventAction;
  G4UIdirectory*        eventDir;   
  G4UIcmdWithAnInteger* PrintCmd;    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
