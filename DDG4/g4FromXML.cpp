//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDG4/Geant4Config.h"

// C/C++ include files
#include <stdexcept>
#include <iostream>
#include <cerrno>

using namespace dd4hep::sim::Setup;

int setupG4_XML()  {
  dd4hep::Detector& description = dd4hep::Detector::getInstance();
  Kernel& kernel = Kernel::instance(description);
  kernel.loadGeometry("file:../DD4hep.trunk/DDExamples/CLICSiD/compact/compact.xml");
  kernel.loadXML("DDG4_field.xml");
  kernel.loadXML("sequences.xml");
  kernel.loadXML("physics.xml");
  kernel.configure();
  kernel.initialize();
  kernel.run();
  std::cout << "Successfully executed application .... " << std::endl;
  kernel.terminate();
  return 1;
}

/// Main entry point as a program
int main(int, char**)   {
  try  {
    return setupG4_XML();
  }
  catch(const std::exception& e)  {
    std::cout << "Got uncaught exception: " << e.what() << std::endl;
  }
  catch (...)  {
    std::cout << "Got UNKNOWN uncaught exception." << std::endl;
  }
  return EINVAL;    
}
