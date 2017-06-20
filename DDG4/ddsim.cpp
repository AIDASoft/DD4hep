//====================================================================
//  DDSim - LC simulation based on dd4hep
//--------------------------------------------------------------------
//  F.Gaede, DESY
//====================================================================

#include "DDG4/Geant4Config.h"
#include <cerrno>
#include <iostream>
#include <stdexcept>

using namespace dd4hep::sim::Setup;

/** Simple main program to run a simulation with DDG4
 *  Loops over all xml files given on command line:
 *   - first file defines geometry
 *   - subsequent files configure the application
 */

int main_wrapper(int argc, char** argv)  {
  if( argc < 2 ){
    std::cout << " --- Usage example: \n "
              << " dd_sim  ../ILD/compact/ILD_o1_v05.xml [sensitive_detectors.xml] sequences.xml physics.xml "
              << std::endl ;
    exit( 0 ) ;
  }

  dd4hep::Detector& description = dd4hep::Detector::getInstance();
  Kernel& kernel = Kernel::instance(description);

  // first argument: geometry file

  std::string geoFile = "file:" ;
  geoFile += argv[1] ;

  kernel.loadGeometry( geoFile ) ;
  for( int i=2 ; i < argc  ; ++i ) {
    std::cout << "  will open xml file " << argv[i] <<  " and load to kernel ..." << std::endl ;
    kernel.loadXML( argv[i] ) ;
  }

  kernel.configure();
  kernel.initialize();
  kernel.run();

  std::cout << "Successfully executed application .... " << std::endl;
  kernel.terminate();
  return 0;
}

//______________________________________________________________________________
int main(int argc, char** argv)  {
  try {
    return main_wrapper(argc,argv);
  }
  catch(const std::exception& e)  {
    std::cout << "Got uncaught exception: " << e.what() << std::endl;
  }
  catch (...)  {
    std::cout << "Got UNKNOWN uncaught exception." << std::endl;
  }
  return EINVAL;    
}
