#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"

#include "DDRec/SurfaceManager.h"

namespace dd4hep{
  namespace rec{
    
    using namespace Geometry ;


    /**
    \addtogroup SurfacePlugin
    @{
    \package SurfaceManager

    *  \brief Plugin that creates a SurfaceManager object and attaches it to lcdd as a user extension object.
    *
    @}
    *
    *  @author  F.Gaede, CERN/DESY
    *  @date May, 11 2015
    *  @version $Id: $
    */


    static long createSurfaceManager(LCDD& theDetector, int /*argc*/, char** /*argv*/) {

      printout(INFO,"InstallSurfaceManager","**** running plugin InstallSurfaceManager ! " );

      theDetector.addExtension<SurfaceManager>(  new SurfaceManager(theDetector) ) ;

      printout(INFO,"InstallSurfaceManager","%s" , theDetector.extension<SurfaceManager>()->toString().c_str() );

      return 1;
    }
  }
}

DECLARE_APPLY( InstallSurfaceManager, DD4hep::DDRec::createSurfaceManager )


