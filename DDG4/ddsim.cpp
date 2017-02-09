//====================================================================
//  DDSim - LC simulation based on DD4hep
//--------------------------------------------------------------------
//  F.Gaede, DESY
//====================================================================

#include "DDG4/Geant4Config.h"
#include <iostream>


using namespace DD4hep::Simulation::Setup;

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

  DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();
  Kernel& kernel = Kernel::instance(lcdd);

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
}

//______________________________________________________________________________
int main(int argc, char** argv)  {
  try {
    return main_wrapper(argc,argv);
  }
  catch(const exception& e)  {
    cout << "Got uncaught exception: " << e.what() << endl;
  }
  catch (...)  {
    cout << "Got UNKNOWN uncaught exception." << endl;
  }
  return EINVAL;    
}
