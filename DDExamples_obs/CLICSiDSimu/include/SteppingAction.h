#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class DetectorConstruction;
class EventAction;

class SteppingAction : public G4UserSteppingAction   {
public:
  SteppingAction(EventAction*);
  virtual ~SteppingAction();
  void UserSteppingAction(const G4Step*);
    
private:
  DetectorConstruction* detector;
  EventAction*          eventaction;  
  G4Material*           SiMaterial;
  G4Material*           TPCGasMaterial;
};
#endif
