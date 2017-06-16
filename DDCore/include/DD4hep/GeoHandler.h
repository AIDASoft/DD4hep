//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_GEOHANDLER_H
#define DD4HEP_GEOHANDLER_H

#include "DD4hep/Detector.h"
#include <set>
#include <map>
#include <vector>

// Forward declarations
class TGeoMatrix;
class TGeoVolume;
class TGeoMedium;
class TGeoShape;
class TGeoNode;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class  Detector;
  class  NamedObject;
  class  DetElement;
  class  SensitiveDetector;
  class  VisAttrObject;
  class  Volume;
  class  PlacedVolume;

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// Defintion of the object types used by generic geometry handlers
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class GeoHandlerTypes {
    public:
#if 0
      typedef std::set<const TGeoVolume*> ConstVolumeSet;
      typedef std::map<SensitiveDetector, ConstVolumeSet> SensitiveVolumes;
      typedef std::map<Region,   ConstVolumeSet>          RegionVolumes;
      typedef std::map<LimitSet, ConstVolumeSet>          LimitVolumes;
      typedef std::map<int, std::set<const TGeoNode*> >   Data;
      typedef std::set<SensitiveDetector>                 SensitiveDetectorSet;
      typedef std::set<Region>                            RegionSet;
      typedef std::set<LimitSet>                          LimitSetSet;
      typedef std::set<TNamed*>                           ObjectSet;
#endif
      /// Data container
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      class GeometryInfo {
      public:
        std::set<TGeoShape*>  solids;
        std::set<Volume>      volumeSet;
        std::vector<Volume>   volumes;
        std::vector<std::pair<std::string, TGeoMatrix*> > trafos;
        std::set<VisAttr>      vis;
        std::set<Ref_t>        fields;
        std::set<Material>     materials;
        std::set<TGeoMedium*>  media;
        std::set<TGeoElement*> elements;
      };
    };

    /// The base class for all dd4hep geometry crawlers
    /**
     *  Geometry crawlers are used for multiple purposes, whenever entire
     *  geometries must be traversed like e.g. to create a new geometry
     *  for simulation etc.
     *  While analysing the geometry, information is collected, which
     *  may be later processed.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class GeoHandler: public GeoHandlerTypes {

    protected:
      bool  m_propagateRegions;
      std::map<int, std::set<const TGeoNode*> >* m_data;

      /// detaill helper to collect geometry information from traversal
      GeoHandler& i_collect(const TGeoNode* node, int level, Region rg, LimitSet ls);

    private:
      /// Never call Copy constructor
      GeoHandler(const GeoHandler&) {
      }
      /// Never call assignment operator
      GeoHandler& operator=(const GeoHandler&) {
        return *this;
      }

    public:
      /// Default constructor
      GeoHandler();
      /// Initializing constructor
      GeoHandler(std::map<int, std::set<const TGeoNode*> >* ptr);
      /// Default destructor
      virtual ~GeoHandler();
      /// Propagate regions. Returns the previous value
      bool setPropagateRegions(bool value);
      /// Collect geometry information from traversal
      GeoHandler& collect(DetElement top);
      /// Collect geometry information from traversal with aggregated information
      GeoHandler& collect(DetElement top, GeometryInfo& info);
      /// Access to collected node list
      std::map<int, std::set<const TGeoNode*> >* release();
    };

    /// Geometry scanner (handle object)
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class GeoScan {
    protected:
      /// Data holder
      std::map<int, std::set<const TGeoNode*> >* m_data;
    public:
      /// Initializing constructor
      GeoScan(DetElement e);
      /// Initializing constructor
      GeoScan(DetElement e, bool propagateRegions);
      /// Default destructor
      virtual ~GeoScan();
      /// Work callback
      virtual GeoScan& operator()();
    };
  }    // End namespace detail
}      // End namespace dd4hep

#endif // DD4HEP_GEOHANDLER_H
