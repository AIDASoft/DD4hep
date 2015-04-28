#ifndef DDRec_SurfaceManager_H_
#define DDRec_SurfaceManager_H_

#include "DDRec/Surface.h"

namespace DD4hep {
  namespace DDRec {

    /** Surface manager class that allows to access all surfaces
     *  assigned to a DetElement and all its daughters.
     *
     * @author F.Gaede, DESY
     * @date Apr, 11 2014
     * @version $Id$
     */
    class SurfaceManager {

    public:

      SurfaceManager();
      
      ~SurfaceManager();
      
      /** Get the list of all surfaces added to this DetElement and all its daughters -
       *  instantiate SurfaceManager with lcdd.world() to get all surfaces.
       */
      //      const SurfaceList& surfaceList() { return _sL ; }

    protected :
      // SurfaceList  _sL ;
      // const Geometry::DetElement& _det ;

      // /// initializes surfaces from VolSurfaces assigned to this DetElement in detector construction
      // void initialize() ;

    };

  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_SurfaceManager_H_
