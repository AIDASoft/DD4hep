
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExDetectorMessenger_h
#define ILDExDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ILDExDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExDetectorMessenger: public G4UImessenger
{
  public:
    ILDExDetectorMessenger(ILDExDetectorConstruction* );
   ~ILDExDetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    ILDExDetectorConstruction* ILDExDetector;
    
    G4UIdirectory*             ILDExDir;
    G4UIdirectory*             detDir;
    G4UIcmdWithADoubleAndUnit* MagFieldCmd;
    G4UIcmdWithoutParameter*   UpdateCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

