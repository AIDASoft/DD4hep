#ifndef DDRec_SurfaceHelper_H_
#define DDRec_SurfaceHelper_H_

#include "DDRec/Surface.h"

namespace DD4hep {
  namespace DDRec {

    /** Surface helper class that allows to access all surfaces
     *  assigned to a DetElement and all its daughters.
     *  (originally this was called SurfaceManager).
     *
     * @author F.Gaede, DESY
     * @date Apr, 11 2014
     * @version $Id: $
     */
    class SurfaceHelper {

    public:

      SurfaceHelper(const Geometry::DetElement& e);
      
      ~SurfaceHelper();
      
      /** Get the list of all surfaces added to this DetElement and all its daughters -
       *  instantiate SurfaceHelper with lcdd.world() to get all surfaces.
       */
      const SurfaceList& surfaceList() { return _sL ; }

    protected :
      SurfaceList  _sL ;
      const Geometry::DetElement& _det ;

      /// initializes surfaces from VolSurfaces assigned to this DetElement in detector construction
      void initialize() ;

    };

  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_SurfaceHelper_H_
