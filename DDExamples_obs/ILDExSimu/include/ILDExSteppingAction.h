
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExSteppingAction_h
#define ILDExSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class ILDExDetectorConstruction;
class ILDExEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExSteppingAction : public G4UserSteppingAction
{
public:
  ILDExSteppingAction(ILDExEventAction*);
  virtual ~ILDExSteppingAction();

  void UserSteppingAction(const G4Step*);
    
private:
  ILDExDetectorConstruction* detector;
  ILDExEventAction*          eventaction;  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
