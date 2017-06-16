#ifndef DDRec_DetectorSurfaces_H_
#define DDRec_DetectorSurfaces_H_

#include "DDRec/Surface.h"

namespace dd4hep {
  namespace rec {

    /** View on DetElements for accessing the surfaces assigned to this DetElement
     *  in detector construction.
     *
     * @author F.Gaede, DESY
     * @date Apr, 10 2014
     * @version $Id$
     */
    class DetectorSurfaces: virtual public DetElement {

    public:
      typedef DetElement DetElement;

      DetectorSurfaces(const DetElement& e);

      virtual ~DetectorSurfaces();

      /// get the list of surfaces added to this DetElement
      const SurfaceList& surfaceList() { return *_sL ; }

    protected :
      SurfaceList* _sL ;

      /// initializes surfaces from VolSurfaces assigned to this DetElement in detector construction
      void initialize() ;

    };

  } /* namespace rec */
} /* namespace dd4hep */

namespace DD4hep { namespace DDRec { using namespace dd4hep::rec  ; } }  // bwd compatibility for old namsepaces

#endif // DDRec_DetectorSurfaces_H_
