#include "DDG4/Geant4Config.h"
#include "DDG4/Geant4TestActions.h"
#include "DDG4/Geant4TrackHandler.h"
#include <iostream>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
using namespace DD4hep::Simulation::Test;
using namespace DD4hep::Simulation::Setup;

#if defined(__MAKECINT__)
#pragma link C++ class Geant4RunActionSequence;
#pragma link C++ class Geant4EventActionSequence;
#pragma link C++ class Geant4SteppingActionSequence;
#pragma link C++ class Geant4StackingActionSequence;
#pragma link C++ class Geant4GeneratorActionSequence;
#pragma link C++ class Geant4Action;
#pragma link C++ class Geant4Kernel;
#endif

SensitiveSeq::handled_type* setupDetector(Kernel& kernel, const std::string& name)   {
  SensitiveSeq sd = SensitiveSeq(kernel,name);
  Sensitive  sens = Sensitive(kernel,"Geant4TestSensitive/"+name+"Handler",name);
  sd->adopt(sens);
  sens = Sensitive(kernel,"Geant4TestSensitive/"+name+"Monitor",name);
  sd->adopt(sens);
  return sd;
}

void setupG4_CINT()  {
  Kernel kernel();

  Phase p;
  kernel.loadGeometry("file:../DD4hep.trunk/DDExamples/CLICSiD/compact/compact.xml");
  kernel.loadXML("sensitive_detectors.xml");

  GenAction gun(kernel,"Geant4ParticleGun/Gun");
  gun["energy"] = 0.5*GeV;
  gun["particle"] = "e-";
  gun["multiplicity"] = 1;
  kernel.generatorAction().adopt(gun);

  Action run_init(kernel,"Geant4TestRunAction/RunInit");
  run_init["Property_int"] = 12345;
  kernel.runAction().callAtBegin  (run_init.get(),&Geant4TestRunAction::begin);
  kernel.eventAction().callAtBegin(run_init.get(),&Geant4TestRunAction::beginEvent);
  kernel.eventAction().callAtEnd  (run_init.get(),&Geant4TestRunAction::endEvent);

  Action evt_1(kernel,"Geant4TestEventAction/UserEvent_1");
  evt_1["Property_int"] = 12345;
  evt_1["Property_string"] = "Events";

  p = kernel.addPhase<const G4Run*>("BeginRun");
  p->add(evt_1.get(),&Geant4TestEventAction::beginRun);
  kernel.runAction().callAtBegin(p.get(),&Geant4ActionPhase::call<const G4Run*>);

  p = kernel.addPhase<const G4Run*>("EndRun");
  p->add(evt_1.get(),&Geant4TestEventAction::endRun);
  kernel.runAction().callAtEnd(p.get(),&Geant4ActionPhase::call<const G4Run*>);

  EventAction evt_2(kernel,"Geant4TestEventAction/UserEvent_2");
  kernel.eventAction().adopt(evt_2);
 
  setupDetector(kernel,"SiVertexBarrel");
  setupDetector(kernel,"SiVertexEndcap");
  setupDetector(kernel,"SiTrackerBarrel");
  setupDetector(kernel,"SiTrackerEndcap");
  setupDetector(kernel,"SiTrackerForward");
  setupDetector(kernel,"EcalBarrel");
  setupDetector(kernel,"EcalEndcap");
  setupDetector(kernel,"HcalBarrel");
  setupDetector(kernel,"HcalEndcap");
  setupDetector(kernel,"HcalPlug");
  setupDetector(kernel,"MuonBarrel");
  setupDetector(kernel,"MuonEndcap");
  setupDetector(kernel,"LumiCal");
  setupDetector(kernel,"BeamCal");

  kernel.configure();
  kernel.initialize();
  kernel.run();
  std::cout << "Successfully executed application .... " << std::endl;
  kernel.terminate();
}

void exampleAClick()  {
  setupG4_CINT();
}

