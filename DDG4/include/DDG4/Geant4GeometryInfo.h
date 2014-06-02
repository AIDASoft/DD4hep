// $Id: Geant4Geometryinfo.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4GEOMETRYINFO_H
#define DD4HEP_DDG4_GEANT4GEOMETRYINFO_H

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/GeoHandler.h"
#include "DDG4/Geant4Primitives.h"

// C/C++ include files
#include <map>
#include <vector>

// Forward declarations (TGeo)
class TGeoElement;
class TGeoMedium;
class TGeoVolume;
class TGeoShape;
class TGeoNode;
// Forward declarations (Geant4)
class G4Element;
class G4Material;
class G4VSolid;
class G4LogicalVolume;
class G4Region;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;
class G4AssemblyVolume;
class G4VSensitiveDetector;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4Mapping;
    class Geant4AssemblyVolume;

    namespace Geant4GeometryMaps  {
      typedef Geometry::Atom Atom;
      typedef Geometry::Material Material;
      typedef Geometry::VisAttr VisAttr;
      typedef Geometry::Volume Volume;
      typedef Geometry::PlacedVolume PlacedVolume;
      typedef Geometry::Region Region;
      typedef Geometry::LimitSet LimitSet;
      typedef Geometry::SensitiveDetector SensitiveDetector;

      typedef std::vector<const G4VPhysicalVolume*> Geant4PlacementPath;
      typedef std::map<Atom, G4Element*> ElementMap;
      typedef std::map<Material, G4Material*> MaterialMap;
      typedef std::map<LimitSet, G4UserLimits*> LimitMap;
      typedef std::map<PlacedVolume, G4VPhysicalVolume*> PlacementMap;
      typedef std::map<Region, G4Region*> RegionMap;
      typedef std::map<SensitiveDetector, G4VSensitiveDetector*> SensDetMap;
      typedef std::map<Volume, G4LogicalVolume*> VolumeMap;
      typedef std::map<PlacedVolume, Geant4AssemblyVolume*>  AssemblyMap;

      typedef std::vector<const TGeoNode*> VolumeChain;
      typedef std::pair<VolumeChain,const G4VPhysicalVolume*> ImprintEntry;
      typedef std::vector<ImprintEntry> Imprints;
      typedef std::map<Volume,Imprints>   VolumeImprintMap;
      typedef std::map<const TGeoShape*, G4VSolid*> SolidMap;
      typedef std::map<VisAttr, G4VisAttributes*> VisMap;
      typedef std::map<Geant4PlacementPath, VolumeID> Geant4PathMap;

      typedef Geometry::GeoHandlerTypes::SensitiveVolumes SensitiveVolumes;
      typedef Geometry::GeoHandlerTypes::RegionVolumes RegionVolumes;
      typedef Geometry::GeoHandlerTypes::LimitVolumes LimitVolumes;
      /// Assemble Geant4 volume path
      std::string placementPath(const Geant4PlacementPath& path, bool reverse=true);
    }

    struct Geant4GeometryInfo : public TNamed, public Geometry::GeoHandlerTypes::GeometryInfo {
    public:
      Geant4GeometryMaps::ElementMap g4Elements;
      Geant4GeometryMaps::MaterialMap g4Materials;
      Geant4GeometryMaps::SolidMap g4Solids;
      Geant4GeometryMaps::VolumeMap g4Volumes;
      Geant4GeometryMaps::PlacementMap g4Placements;
      Geant4GeometryMaps::AssemblyMap  g4AssemblyVolumes;
      Geant4GeometryMaps::VolumeImprintMap g4VolumeImprints;
      Geant4GeometryMaps::RegionMap g4Regions;
      Geant4GeometryMaps::VisMap g4Vis;
      Geant4GeometryMaps::LimitMap g4Limits;
      Geant4GeometryMaps::SensDetMap g4SensDets;

      Geant4GeometryMaps::Geant4PathMap g4Paths;

      Geant4GeometryMaps::SensitiveVolumes sensitives;
      Geant4GeometryMaps::RegionVolumes regions;
      Geant4GeometryMaps::LimitVolumes limits;
      G4VPhysicalVolume* m_world;
      bool valid;
    private:
      friend class Geant4Mapping;
      /// Default constructor
      Geant4GeometryInfo();
      /// Default destructor
      virtual ~Geant4GeometryInfo();
    public:
      /// The world placement
      G4VPhysicalVolume* world() const;
      /// Set the world volume
      void setWorld(const TGeoNode* node);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4GEOMETRYINFO_H
