#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DD4hepUnits.h" 

namespace DD4hep{
  namespace DDRec{
    
    using namespace Geometry ;

    /** Plugin that creates a SurfaceManager object and attaches
     *  to lcdd as user extension.
     * 
     *  @author  F.Gaede, CERN/DESY
     *  @date May, 11 2015
     *  @version $Id: $
     */
    
    static long createSurfaceManager(LCDD& lcdd, int /*argc*/, char** /*argv*/) {
      

      std::cout << " ************* " <<  std::endl
		<< " **** running plugin SurfaceManagerPlugin ! " <<  std::endl
		<< " ************* " <<  std::endl
		<<  std::endl ;
      

      return 1;
    }


  }
}

DECLARE_APPLY( SurfaceManagerPlugin, DD4hep::DDRec::createSurfaceManager )


