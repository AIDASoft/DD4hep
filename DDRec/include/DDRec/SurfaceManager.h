//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede
//
//==========================================================================
#ifndef DDREC_SURFACEMANAGER_H
#define DDREC_SURFACEMANAGER_H

#include "DDRec/ISurface.h"
#include "DD4hep/Detector.h"
#include <string>
#include <map>

namespace dd4hep {
  namespace rec {

    /// typedef for surface maps, keyed by the cellID 
    typedef std::multimap< unsigned long, ISurface*> SurfaceMap ;

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
      SurfaceManager(Detector& theDetector);

      /// No default constructor
#if defined(G__ROOT)
      SurfaceManager() = default ;
#else
      SurfaceManager() = delete ;
#endif
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
      void initialize(Detector& theDetector) ;

      SurfaceMapsMap _map ;
    };

  } /* namespace rec */
} /* namespace dd4hep */



#endif // DDREC_SURFACEMANAGER_H
