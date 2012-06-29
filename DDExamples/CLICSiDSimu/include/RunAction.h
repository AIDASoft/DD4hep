#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class RunAction : public G4UserRunAction  {
public:
  RunAction();
  virtual ~RunAction();

  void BeginOfRunAction(const G4Run*);
  void   EndOfRunAction(const G4Run*);
    
  void fillPerEvent(G4double ESupport, G4double ESensitive,
		    G4double LSupport, G4double LSensitive,
		    G4double AngleSupport, G4double AngleSensitive);

private:

  G4double sumESupport, sum2ESupport;
  G4double sumESensitive, sum2ESensitive;
    
  G4double sumLSupport, sum2LSupport;
  G4double sumLSensitive, sum2LSensitive;    

  G4double sumAngleSupport, sum2AngleSupport;
  G4double sumAngleSensitive, sum2AngleSensitive;

};
#endif

