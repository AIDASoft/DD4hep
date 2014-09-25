#include "DDG4/Geant4Config.h"
#if 0
#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4ActionPhase.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4ParticleHandler.h"
#include "DDG4/Geant4UserParticleHandler.h"
#include "DDG4/ComponentUtils.h"
#include "DD4hep/LCDD.h"
#endif

#include <iostream>


using namespace DD4hep;
using namespace DD4hep::Simulation;
using namespace DD4hep::Simulation::Setup;

void setupG4_XML()  {
  std::string prefix = "file:../DD4hep.trunk";
  Geant4Kernel& kernel = Kernel::instance(Geometry::LCDD::getInstance());
  kernel.loadGeometry((prefix+"/DDExamples/CLICSiD/compact/compact.xml").c_str());
  kernel.loadXML((prefix+"/DDG4/examples/DDG4_field.xml").c_str());
  kernel.loadXML((prefix+"/DDG4/examples/sequences.xml").c_str());
  kernel.loadXML((prefix+"/DDG4/examples/physics.xml").c_str());
  kernel.configure();
  kernel.initialize();
  kernel.run();
  std::cout << "Successfully executed application .... " << std::endl;
  kernel.terminate();
#if 0
#endif
}


void xmlAClick()  {
  setupG4_XML();
}

