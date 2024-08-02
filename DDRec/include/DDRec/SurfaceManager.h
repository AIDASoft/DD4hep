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
#include <set>

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

      typedef std::map< std::string,  std::pair<bool, SurfaceMap>> SurfaceMapsMap;

    public:
      /// The constructor
      SurfaceManager(const Detector& theDetector);

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
      const SurfaceMap* map( const std::string name );


      ///create a string with all available maps and their size (number of surfaces)
      std::string toString() const ;

    protected :


      /// initialize all known surface maps
      void initialize(const Detector& theDetector) ;

      /// name has to be the name of a single detector! not e.g. tracker or world.
      const SurfaceMap& getOrConstruct(const std::string name);

      SurfaceMapsMap _map ;
      const Detector& m_detector;
      std::set<std::string> m_detectorNames;
      std::set<std::string> m_detectorTypes;
      std::map<std::string, std::set<std::string>> m_missingDetectors;
    };

  } /* namespace rec */
} /* namespace dd4hep */



#endif // DDREC_SURFACEMANAGER_H
