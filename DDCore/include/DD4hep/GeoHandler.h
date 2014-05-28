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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    struct LCDD;
    struct Volume;
    struct PlacedVolume;
    struct DetElement;
    struct SensitiveDetector;

    /** @class GeoHandlerTypes  GeoHandler.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class GeoHandlerTypes {
    public:
      typedef std::set<TGeoVolume*> VolumeSet;
      typedef std::vector<TGeoVolume*> VolumeVector;
      typedef std::set<const TGeoVolume*> ConstVolumeSet;
      typedef std::vector<std::pair<std::string, TGeoMatrix*> > TransformSet;
      typedef std::set<TGeoShape*> SolidSet;
      typedef std::set<TGeoMedium*> MaterialSet;
      typedef std::map<const TNamed*, ConstVolumeSet> SensitiveVolumes;
      typedef std::map<const TNamed*, ConstVolumeSet> RegionVolumes;
      typedef std::map<const TNamed*, ConstVolumeSet> LimitVolumes;
      typedef std::map<int, std::set<const TGeoNode*> > Data;
      typedef std::set<const TNamed*> VisRefs;
      typedef std::set<TNamed*> Fields;
      typedef std::set<TNamed*> ObjectSet;
      typedef LCDD::HandleMap DefinitionSet;
      struct GeometryInfo {
        SolidSet solids;
        VolumeSet volumeSet;
        VolumeVector volumes;
        TransformSet trafos;
        VisRefs vis;
        Fields fields;
        MaterialSet materials;
        // SensitiveVolumes   sensitives;
        // RegionVolumes      regions;
        // LimitVolumes       limits;
        std::set<TGeoMedium*> media;
        std::set<TGeoElement*> elements;
      };
    };

    /** @class GeoHandler  GeoHandler.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class GeoHandler: public GeoHandlerTypes {

    protected:
      Data* m_data;

      /// Internal helper to collect geometry information from traversal
      GeoHandler& i_collect(const TGeoNode* node, int level);

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
      /// Collect geometry information from traversal
      GeoHandler& collect(DetElement top);
      /// Collect geometry information from traversal with aggregated information
      GeoHandler& collect(DetElement top, GeometryInfo& info);
      /// Access to collected node list
      Data* release();
    };

    struct GeoScan {
    protected:
      /// Data holder
      GeoHandler::Data* m_data;
    public:
      /// Initializing constructor
      GeoScan(DetElement e);
      /// Default destructor
      virtual ~GeoScan();
      /// Work callback
      virtual GeoScan& operator()();
    };
  }    // End namespace Geometry
}      // End namespace DD4hep

#endif // DD4HEP_GEOHANDLER_H
