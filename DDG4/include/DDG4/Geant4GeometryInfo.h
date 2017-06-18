// $Id: $
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
class G4MaterialPropertiesTable;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4Mapping;
    class Geant4AssemblyVolume;

    /// Helper namespace defining data types for the relation information between geant4 objects and dd4hep objects.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    namespace Geant4GeometryMaps  {
      //typedef std::vector<const G4VPhysicalVolume*>           Geant4PlacementPath;
      typedef std::map<Atom, G4Element*>                      ElementMap;
      typedef std::map<Material, G4Material*>                 MaterialMap;
      //typedef std::map<LimitSet, G4UserLimits*>               LimitMap;
      typedef std::map<PlacedVolume, G4VPhysicalVolume*>      PlacementMap;
      //typedef std::map<Region, G4Region*>                     RegionMap;
      typedef std::map<Volume, G4LogicalVolume*>              VolumeMap;
      typedef std::map<PlacedVolume, Geant4AssemblyVolume*>   AssemblyMap;
      typedef std::map<MaterialPropertiesTable, G4MaterialPropertiesTable*> MaterialPropertiesTableMap;

      typedef std::vector<const TGeoNode*>                    VolumeChain;
      typedef std::pair<VolumeChain,const G4VPhysicalVolume*> ImprintEntry;
      typedef std::vector<ImprintEntry>                       Imprints;
      typedef std::map<Volume,Imprints>                       VolumeImprintMap;
      typedef std::map<const TGeoShape*, G4VSolid*>           SolidMap;
      //typedef std::map<VisAttr, G4VisAttributes*>             VisMap;
      //typedef std::map<Geant4PlacementPath, VolumeID>         Geant4PathMap;
    }

    /// Concreate class holding the relation information between geant4 objects and dd4hep objects.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GeometryInfo : public TNamed, public detail::GeoHandlerTypes::GeometryInfo {
    public:
      typedef std::vector<const G4VPhysicalVolume*>           Geant4PlacementPath;
      Geant4GeometryMaps::ElementMap       g4Elements;
      Geant4GeometryMaps::MaterialMap      g4Materials;
      Geant4GeometryMaps::SolidMap         g4Solids;
      Geant4GeometryMaps::VolumeMap        g4Volumes;
      Geant4GeometryMaps::PlacementMap     g4Placements;
      Geant4GeometryMaps::AssemblyMap      g4AssemblyVolumes;
      Geant4GeometryMaps::MaterialPropertiesTableMap g4MPTMap;
      Geant4GeometryMaps::VolumeImprintMap g4VolumeImprints;
      std::map<MaterialPropertiesTable, G4MaterialPropertiesTable*> g4MaterialPropertiesTable;
      std::map<Region, G4Region*>                              g4Regions;
      std::map<VisAttr, G4VisAttributes*>                      g4Vis;
      std::map<LimitSet, G4UserLimits*>                        g4Limits;
      std::map<Geant4PlacementPath, VolumeID>                  g4Paths;
      std::map<SensitiveDetector,std::set<const TGeoVolume*> > sensitives;
      std::map<Region,           std::set<const TGeoVolume*> > regions;
      std::map<LimitSet,         std::set<const TGeoVolume*> > limits;
      G4VPhysicalVolume*                                       m_world;
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
      /// Assemble Geant4 volume path
      static std::string placementPath(const Geant4PlacementPath& path, bool reverse=true);
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DD4HEP_DDG4_GEANT4GEOMETRYINFO_H
