#ifndef DDRec_SurfaceManager_H_
#define DDRec_SurfaceManager_H_

#include "DDRec/Surface.h"

namespace DD4hep {
  namespace DDRec {

    /// typedef for surface maps, keyed by the cellID 
    typedef std::map< unsigned long, DD4hep::DDRec::Surface*> SurfaceMap ;

    /** Surface manager class that holds maps of surfaces for all known 
     *  sensitive detector types and  individual sub detectors. 
     *  Maps can be retrieved via detector name.
     * 
     * @author F.Gaede, DESY
     * @date May, 11 2015
     * @version $Id$
     */
    class SurfaceManager {

      typedef std::map< std::string,  SurfaceMap > SurfaceMapsMap ;

    public:

      SurfaceManager();
      
      ~SurfaceManager();
      
      /** Get the maps of all surfaces associated to the given detector or
       *  type of detectors, e.g. map("tracker") returns a map with all surfaces
       *  assigned to tracking detectors. Returns 0 if no map exists.
       */
      const SurfaceMap* map( const std::string name ) const ;

    protected :


      /// initialize all known surface maps
      void initialize() ;

      SurfaceMapsMap _map ;
    };

  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_SurfaceManager_H_
