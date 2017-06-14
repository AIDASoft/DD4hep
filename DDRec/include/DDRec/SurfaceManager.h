#ifndef DDRec_SurfaceManager_H_
#define DDRec_SurfaceManager_H_

#include "DDSurfaces/ISurface.h"
#include <string>
#include <map>

namespace DD4hep {

  namespace Geometry{
    class LCDD ;
  }

  namespace DDRec {

    /// typedef for surface maps, keyed by the cellID 
    typedef std::multimap< unsigned long, DDSurfaces::ISurface*> SurfaceMap ;

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
      /// The constructor
      SurfaceManager(DD4hep::Geometry::LCDD& theDetector);

      /// No default constructor
      SurfaceManager() = delete ;

      /// No copy constructor
      SurfaceManager(const SurfaceManager& copy) = delete;
      
      /// Default destructor
      ~SurfaceManager();

      /// No assignment operator
      SurfaceManager& operator=(const SurfaceManager& copy) = delete;
      
      /** Get the maps of all surfaces associated to the given detector or
       *  type of detectors, e.g. map("tracker") returns a map with all surfaces
       *  assigned to tracking detectors. Returns 0 if no map exists.
       */
      const SurfaceMap* map( const std::string name ) const ;

      
      ///create a string with all available maps and their size (number of surfaces)
      std::string toString() const ;
      
    protected :


      /// initialize all known surface maps
      void initialize(DD4hep::Geometry::LCDD& theDetector) ;

      SurfaceMapsMap _map ;
    };

  } /* namespace DDRec */
} /* namespace DD4hep */

#endif // DDRec_SurfaceManager_H_
