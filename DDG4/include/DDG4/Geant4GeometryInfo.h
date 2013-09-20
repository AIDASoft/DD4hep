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
  namespace Simulation   {

    // Forward declarations
    class Geant4Mapping;
    
    struct Geant4GeometryInfo : public TNamed, public Geometry::GeoHandlerTypes::GeometryInfo   {
    public:
      typedef std::vector<const G4VPhysicalVolume*>                      PlacementPath;
      typedef std::vector<std::pair<size_t,const TGeoNode*> >            AssemblyChildren;
      typedef std::map<const TGeoElement*,     G4Element*>               ElementMap;
      typedef std::map<const TGeoMedium*,      G4Material*>              MaterialMap;
      typedef std::map<const TNamed*,          G4UserLimits*>            LimitMap;
      typedef std::map<const TGeoNode*,        G4VPhysicalVolume*>       PlacementMap;
      typedef std::map<const G4AssemblyVolume*,AssemblyChildren>         AssemblyChildMap;
      typedef std::map<const TNamed*,          G4Region*>                RegionMap;
      typedef std::map<const TNamed*,          G4VSensitiveDetector*>    SensDetMap;
      typedef std::map<const TGeoVolume*,      G4LogicalVolume*>         VolumeMap;
      typedef std::map<const TGeoShape*,       G4VSolid*>                SolidMap;
      typedef std::map<const TNamed*,          G4VisAttributes*>         VisMap;
      typedef std::map<PlacementPath,          VolumeID>                 PathMap;

      typedef Geometry::GeoHandlerTypes::SensitiveVolumes                SensitiveVolumes;
      typedef Geometry::GeoHandlerTypes::RegionVolumes                   RegionVolumes;
      typedef Geometry::GeoHandlerTypes::LimitVolumes                    LimitVolumes;

      ElementMap              g4Elements;
      MaterialMap             g4Materials;
      SolidMap                g4Solids;
      VolumeMap               g4Volumes;
      PlacementMap            g4Placements;
      AssemblyChildMap        g4AssemblyChildren;
      RegionMap               g4Regions;
      VisMap                  g4Vis;
      LimitMap                g4Limits;
      SensDetMap              g4SensDets;
      PathMap                 g4Paths;

      SensitiveVolumes        sensitives;
      RegionVolumes           regions;
      LimitVolumes            limits;
      bool                    valid;
    private:
      friend class Geant4Mapping;
      /// Default constructor
      Geant4GeometryInfo();
      /// Default destructor
      virtual ~Geant4GeometryInfo();
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4GEOMETRYINFO_H
