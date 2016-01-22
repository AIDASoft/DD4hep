#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"

#include "DDRec/SurfaceManager.h"

namespace DD4hep{
  namespace DDRec{
    
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


    static long createSurfaceManager(LCDD& lcdd, int /*argc*/, char** /*argv*/) {

      printout(INFO,"InstallSurfaceManager","**** running plugin InstallSurfaceManager ! " );

      lcdd.addExtension<SurfaceManager>(  new SurfaceManager() ) ;

      printout(INFO,"InstallSurfaceManager","%s" , lcdd.extension<SurfaceManager>()->toString().c_str() );

      return 1;
    }
  }
}

DECLARE_APPLY( InstallSurfaceManager, DD4hep::DDRec::createSurfaceManager )


