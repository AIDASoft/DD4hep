
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExEventAction_h
#define ILDExEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class ILDExRunAction;
class ILDExEventActionMessenger;

namespace DD4hep{namespace Geometry{    class LCDD ; }}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ILDExEventAction : public G4UserEventAction
{
public:
  ILDExEventAction(ILDExRunAction*, DD4hep::Geometry::LCDD& );

  virtual ~ILDExEventAction();

  void  BeginOfEventAction(const G4Event*);
  void    EndOfEventAction(const G4Event*);
    
  void SumSupport(G4double de, G4double dl, G4double da) {EnergySupport += de; TrackLSupport += dl; AngleSupport += da; };
  void SumSensitive(G4double de, G4double dl, G4double da) {EnergySensitive += de; TrackLSensitive += dl; AngleSensitive += da; };
                     
  void SetPrintModulo(G4int    val)  {printModulo = val;};
    
private:
   ILDExRunAction*  runAct;
   
   G4double  EnergySupport, EnergySensitive;
   G4double  TrackLSupport, TrackLSensitive;
   G4double  AngleSupport, AngleSensitive;                     

   G4int     printModulo;

  ILDExEventActionMessenger*  eventMessenger;

  DD4hep::Geometry::LCDD& _lcdd ;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
