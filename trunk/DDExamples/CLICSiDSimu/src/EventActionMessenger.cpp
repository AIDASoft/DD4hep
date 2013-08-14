#include "EventActionMessenger.h"
#include "EventAction.h"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"

EventActionMessenger::EventActionMessenger(EventAction* EvAct)
: eventAction(EvAct)
{
  eventDir = new G4UIdirectory("/Dir/event/");
  eventDir->SetGuidance("event control");
   
  PrintCmd = new G4UIcmdWithAnInteger("/Dir/event/printModulo",this);
  PrintCmd->SetGuidance("Print events modulo n");
  PrintCmd->SetParameterName("EventNb",false);
  PrintCmd->SetRange("EventNb>0");
}

EventActionMessenger::~EventActionMessenger()  {
  delete PrintCmd;
  delete eventDir;   
}

void EventActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)  { 
  if(command == PrintCmd)
    {eventAction->SetPrintModulo(PrintCmd->GetNewIntValue(newValue));}
}
