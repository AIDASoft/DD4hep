
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class RunAction;
class EventActionMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
public:
  EventAction(RunAction*);
  virtual ~EventAction();

  void  BeginOfEventAction(const G4Event*);
  void    EndOfEventAction(const G4Event*);
    
  void SumSupport(G4double de, G4double dl, G4double da) {EnergySupport += de; TrackLSupport += dl; AngleSupport += da; };
  void SumSensitive(G4double de, G4double dl, G4double da) {EnergySensitive += de; TrackLSensitive += dl; AngleSensitive += da; };
                     
  void SetPrintModulo(G4int    val)  {printModulo = val;};
    
private:
   RunAction*  runAct;
   
   G4double  EnergySupport, EnergySensitive;
   G4double  TrackLSupport, TrackLSensitive;
   G4double  AngleSupport, AngleSensitive;                     

   G4int     printModulo;
                             
   EventActionMessenger*  eventMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
