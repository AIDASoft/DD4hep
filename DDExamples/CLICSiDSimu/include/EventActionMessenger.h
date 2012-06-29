#ifndef EventActionMessenger_h
#define EventActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class EventAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;

class EventActionMessenger: public G4UImessenger   {
public:
  EventActionMessenger(EventAction*);
  virtual ~EventActionMessenger();    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  EventAction*     eventAction;
  G4UIdirectory*        eventDir;   
  G4UIcmdWithAnInteger* PrintCmd;    
};
#endif
