#include "DD4hep/Printout.h"
#include "DDG4/Geant4Config.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::sim::Setup;

int setupG4_CINT()  {
  typedef dd4hep::sim::Geant4Kernel Kernel;
  Kernel& kernel = Kernel::instance(dd4hep::Detector::getInstance());
  string install_dir = getenv("DD4hepINSTALL");
  string ddg4_examples = install_dir+"/examples/DDG4/examples";
  char text[256];

  //kernel.loadGeometry(("file:"+install_dir+"/examples/ClientTests/compact/FCC_HcalBarrel.xml").c_str());
  kernel.loadGeometry(("file:"+install_dir+"/examples/ClientTests/compact/IronCylinder.xml").c_str());
  kernel.loadXML(("file:"+ddg4_examples+"/DDG4_field.xml").c_str());
  kernel.setOutputLevel("Geant4Converter",DEBUG);
  kernel.setOutputLevel("RootOutput",INFO);
  kernel.setOutputLevel("ShellHandler",DEBUG);
  kernel.setOutputLevel("Gun",INFO);
  kernel.property("UI") = "UI";  // Bind UI instance to kernel
  kernel.printProperties();

  // Configure UI
  Action ui(kernel,"Geant4UIManager/UI");
  ui["HaveVIS"]     = true;
  ui["HaveUI"]      = true;
  ui["SessionType"] = "csh";
  kernel.registerGlobalAction(ui);

  // Configure I/O
  time_t tim;
  ::time(&tim);
  struct tm *tm = ::localtime(&tim);
  ::strftime(text,sizeof(text),"FCC_HCAL_%Y-%m-%d_%H-%M.root",tm);
  EventAction evt_root(kernel,"Geant4Output2ROOT/RootOutput");
  evt_root["Control"] = true;
  evt_root["Output"]  = (const char*)text;
  evt_root["HandleMCTruth"] = false;
  evt_root->enableUI();
  kernel.eventAction().adopt(evt_root);

  GenAction gun(kernel,"Geant4ParticleGun/Gun");
  gun["energy"] = 10.0*GeV;
  gun["particle"] = "pi-";
  gun["multiplicity"] = 1;
  gun["isotrop"] = true;
  gun->enableUI();
  kernel.generatorAction().adopt(gun);

  // Setup global filters fur use in sensntive detectors
  Filter f1(kernel,"GeantinoRejectFilter/GeantinoRejector");
  kernel.registerGlobalFilter(f1);

  // Now the calorimeters
  SensitiveSeq seq_hc(kernel,"Geant4SensDetActionSequence/HcalBarrel");
  Sensitive    act_hc(kernel,"Geant4SimpleCalorimeterAction/HcalBarrelHandler","HcalBarrel");
  seq_hc->adopt(f1);  // ignore geantinos
  seq_hc->adopt(act_hc);

  SensitiveSeq seq_cs(kernel,"Geant4SensDetActionSequence/ContainmentShell");
  Sensitive    act_cs(kernel,"Geant4EscapeCounter/ShellHandler","ContainmentShell");
  seq_cs->adopt(act_cs);

  // Now build the physics list:
  printout(INFO,"FCC_Hcal","PhysicsList:%p",&kernel.physicsList());
  PhysicsActionSeq phys(&kernel.physicsList());
  phys["extends"] = "QGSP_BERT";
  phys->enableUI();
  phys->dump();

  kernel.configure();
  kernel.initialize();
  kernel.run();
  printout(INFO,"FCC_Hcal","Successfully executed application .... ");
  kernel.terminate();
  return 0;
}

#if defined(G__DICTIONARY) || defined(__CINT__) || defined(__MAKECINT__) // Cint script
int FCC_Hcal()
#else
int main(int, char**)                              // Main program if linked standalone
#endif
{
  return setupG4_CINT();
}
