#include "DD4hep/Detector.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"

#include "DDRec/SurfaceManager.h"

namespace dd4hep{
  namespace rec{
    
    using namespace detail ;


    /**
    \addtogroup SurfacePlugin
    @{
    \package SurfaceManager

    *  \brief Plugin that creates a SurfaceManager object and attaches it to description as a user extension object.
    *
    @}
    *
    *  @author  F.Gaede, CERN/DESY
    *  @date May, 11 2015
    *  @version $Id: $
    */


    static long createSurfaceManager(Detector& description, int /*argc*/, char** /*argv*/) {

      printout(INFO,"InstallSurfaceManager","**** running plugin InstallSurfaceManager ! " );

      description.addExtension<SurfaceManager>(  new SurfaceManager(description) ) ;

      printout(INFO,"InstallSurfaceManager","%s" , description.extension<SurfaceManager>()->toString().c_str() );

      return 1;
    }
  }
}

DECLARE_APPLY( InstallSurfaceManager, dd4hep::rec::createSurfaceManager )


