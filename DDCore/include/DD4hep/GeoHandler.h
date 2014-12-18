// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOHANDLER_H
#define DD4HEP_GEOHANDLER_H

#include "DD4hep/LCDD.h"
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
namespace DD4hep {

  // Forward declarations
  class  NamedObject;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    class  LCDD;
    class  Volume;
    class  PlacedVolume;
    class  DetElement;
    class  SensitiveDetector;
    class  VisAttrObject;

    /// Defintion of the object types used by generic geometry handlers
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class GeoHandlerTypes {
    public:
      typedef std::set<Volume> VolumeSet;
      typedef std::vector<Volume> VolumeVector;
      typedef std::set<const TGeoVolume*> ConstVolumeSet;
      typedef std::vector<std::pair<std::string, TGeoMatrix*> > TransformSet;
      typedef std::set<TGeoShape*> SolidSet;
      typedef std::set<Material> MaterialSet;
      typedef std::map<SensitiveDetector, ConstVolumeSet> SensitiveVolumes;
      typedef std::map<Region,   ConstVolumeSet> RegionVolumes;
      typedef std::map<LimitSet, ConstVolumeSet> LimitVolumes;
      typedef std::map<int, std::set<const TGeoNode*> > Data;
      typedef std::set<VisAttr> VisRefs;
      typedef std::set<SensitiveDetector> SensitiveDetectorSet;
      typedef std::set<Region>            RegionSet;
      typedef std::set<LimitSet>          LimitSetSet;
      typedef std::set<Ref_t>             Fields;
      typedef std::set<TNamed*> ObjectSet;
      typedef LCDD::HandleMap             DefinitionSet;

      /// Data container
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_GEOMETRY
       */
      class GeometryInfo {
      public:
        SolidSet solids;
        VolumeSet volumeSet;
        VolumeVector volumes;
        TransformSet trafos;
        VisRefs vis;
        Fields fields;
        MaterialSet materials;
        std::set<TGeoMedium*>  media;
        std::set<TGeoElement*> elements;
      };
    };

    /// The base class for all DD4hep geometry crawlers
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
      Data* m_data;

      /// Internal helper to collect geometry information from traversal
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
      GeoHandler(Data* ptr);
      /// Default destructor
      virtual ~GeoHandler();
      /// Propagate regions. Returns the previous value
      bool setPropagateRegions(bool value);
      /// Collect geometry information from traversal
      GeoHandler& collect(DetElement top);
      /// Collect geometry information from traversal with aggregated information
      GeoHandler& collect(DetElement top, GeometryInfo& info);
      /// Access to collected node list
      Data* release();
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
      GeoHandler::Data* m_data;
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
  }    // End namespace Geometry
}      // End namespace DD4hep

#endif // DD4HEP_GEOHANDLER_H
