#include "DDG4/Geant4Config.h"
#include <iostream>
using namespace DD4hep::Simulation::Setup;

void setupG4_XML()  {
  DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();
  Kernel& kernel = Kernel::instance(lcdd);
  kernel.loadGeometry("file:../DD4hep.trunk/DDExamples/CLICSiD/compact/compact.xml");
  kernel.loadXML("DDG4_field.xml");
  kernel.loadXML("sequences.xml");
  kernel.loadXML("physics.xml");
  kernel.configure();
  kernel.initialize();
  kernel.run();
  std::cout << "Successfully executed application .... " << std::endl;
  kernel.terminate();
}


int main(int, char**)   {
  setupG4_XML();
  return 1;
}
