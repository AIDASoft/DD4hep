// $Id: Geant4Data.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Run Geant4 using DDG4 from root prompt with test objects for
//  Run/Event/Sensitive actions.
//
//  The Geant4 setup is programmed in the xml files to be parsed. Only
//  the commands to parse these xml files is executed by root.
//
//  Usage:
//  $> root.exe
//  root[0] .x <installation-directory>/examples/DDG4/examples/initAClick.C
//  root[1] .L <installation-directory>/examples/DDG4/examples/CLICSidXML.C+
//  root[2] CLICSidAClick()
//
//
//  Author     : M.Frank
//
//====================================================================
#include "DDG4/Geant4Config.h"
#include <iostream>

using namespace DD4hep::Simulation::Setup;

void setupG4_XML()  {
  std::string prefix = "file:../DD4hep.trunk";
  Kernel& kernel = Kernel::instance(Kernel::LCDD::getInstance());
  kernel.loadGeometry((prefix+"/DDExamples/CLICSiD/compact/compact.xml").c_str());
  kernel.loadXML((prefix+"/DDG4/examples/DDG4_field.xml").c_str());
  kernel.loadXML((prefix+"/DDG4/examples/sequences.xml").c_str());
  kernel.loadXML((prefix+"/DDG4/examples/physics.xml").c_str());
  kernel.property("UI") = "UI";
  kernel.configure();
  kernel.initialize();
  kernel.run();
  std::cout << "Successfully executed application .... " << std::endl;
  kernel.terminate();
}

#if defined(G__DICTIONARY) || defined(__CINT__) || defined(__MAKECINT__) // Cint script
int CLICSidXML()
#else
int main(int, char**)                              // Main program if linked standalone
#endif
{
  setupG4_XML();
  return 1;
}
