#include "DDG4/Geant4Config.h"
#include "DDG4/Geant4TestActions.h"

#include <iostream>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
using namespace DD4hep::Simulation::Test;
using namespace DD4hep::Simulation::Setup;

SensitiveSeq::handled_type* setupDetector(Geant4Kernel& kernel, const std::string& name)   {
  SensitiveSeq sd = SensitiveSeq(kernel,name);
  Sensitive  sens = Sensitive(kernel,"Geant4TestSensitive/"+name+"Handler",name);
  sens["OutputLevel"] = 4;
  sd->adopt(sens);
  sens = Sensitive(kernel,"Geant4TestSensitive/"+name+"Monitor",name);
  sd->adopt(sens);
  return sd;
}

void setupG4_CINT()  {
  //Geant4Kernel& kernel = Geant4Kernel::instance(Geometry::LCDD::getInstance());
  Geant4Kernel kernel(Geometry::LCDD::getInstance());
  string install_dir = getenv("DD4hepINSTALL");
  string ddg4_examples = install_dir+"/examples/DDG4/examples";
  Phase p;

  kernel.loadGeometry(("file:"+install_dir+"/examples/CLICSiD/compact/compact.xml").c_str());
  kernel.loadXML(("file:"+ddg4_examples+"/DDG4_field.xml").c_str());

  kernel.property("UI") = "UI";
  setPrintLevel(DEBUG);

  Action ui(kernel,"Geant4UIManager/UI");
  ui["HaveVIS"] = true;
  ui["HaveUI"]  = true;
  ui["SessionType"] = "csh";
  kernel.registerGlobalAction(ui);

  GenAction gun(kernel,"Geant4ParticleGun/Gun");
  gun["energy"] = 25*GeV;
  gun["particle"] = "e-";
  gun["multiplicity"] = 1;
  gun["OutputLevel"] = 5;
  kernel.generatorAction().adopt(gun);

  RunAction run_init(kernel,"Geant4TestRunAction/RunInit");
  run_init["Property_int"] = 12345;
  kernel.runAction().adopt(run_init);
  kernel.eventAction().callAtBegin(run_init.get(),&Geant4TestRunAction::beginEvent);
  kernel.eventAction().callAtEnd  (run_init.get(),&Geant4TestRunAction::endEvent);

  EventAction evt_1(kernel,"Geant4TestEventAction/UserEvent_1");
  evt_1["Property_int"] = 12345;
  evt_1["Property_string"] = "Events";
  evt_1["OutputLevel"] = 5;
  kernel.eventAction().adopt(evt_1);

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

void CLICSidAClick()  {
  setupG4_CINT();
}
