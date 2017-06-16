// $Id$
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

using namespace std;
using namespace dd4hep::sim::Setup;

int setupG4_XML(bool interactive)  {
  string install_dir = getenv("DD4hepINSTALL");
  string prefix = "file:"+install_dir+"/examples/";
  Kernel& kernel = Kernel::instance(dd4hep::Detector::getInstance());
  kernel.loadGeometry((prefix+"CLICSiD/compact/compact.xml").c_str());
  kernel.loadXML((prefix+"CLICSiD/sim/field.xml").c_str());
  kernel.loadXML((prefix+"CLICSiD/sim/sequences.xml").c_str());
  kernel.loadXML((prefix+"CLICSiD/sim/physics.xml").c_str());
  if ( interactive )  {
    kernel.property("UI") = "UI";
  }
  kernel.configure();
  kernel.initialize();
  kernel.run();
  cout << "Successfully executed application .... " << endl;
  kernel.terminate();
  cout << "TEST_PASSED" << endl;
  return 0;
}

#if defined(G__DICTIONARY) || defined(__CLING__) || defined(__CINT__) || defined(__MAKECINT__) // CINT script
int CLICSiDXML()
#else
int main(int, char**)                              // Main program if linked standalone
#endif
{
  return setupG4_XML(false);
}
