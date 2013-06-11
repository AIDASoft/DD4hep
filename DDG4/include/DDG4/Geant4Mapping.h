// $Id: Geant4Mapping.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4MAPPING_H
#define DD4HEP_GEANT4MAPPING_H

// Framework include files
#include "DD4hep/GeoHandler.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <set>
#include <map>
#include <vector>

// Forward declarations
class TGeoVolume;
class TGeoElement;
class TGeoShape;
class TGeoMedium;
class TGeoNode;

class G4Element;
class G4Material;
class G4VSolid;
class G4LogicalVolume;
class G4PVPlacement;
class G4Region;
class G4Field;
class G4FieldManager;
class G4UserLimits;
class G4VisAttributes;
class G4VPhysicalVolume;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    class Geant4SensitiveDetector;

    /** @class Geant4Mapping Geant4Mapping.h DDG4/Geant4Mapping.h
     * 
     * Geometry mapping from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Mapping : public Geometry::GeoHandlerTypes   {
    public:
      typedef Geometry::LCDD              LCDD;
      typedef Geometry::DetElement        DetElement;
      typedef Geometry::SensitiveDetector SensitiveDetector;
      typedef Geometry::Solid             Solid;
      typedef Geometry::Volume            Volume;
      typedef Geometry::PlacedVolume      PlacedVolume;
      typedef Geometry::Material          Material;
      typedef Geometry::Region            Region;

      typedef std::map<const TGeoElement*,G4Element*>               ElementMap;
      typedef std::map<const TGeoMedium*, G4Material*>              MaterialMap;
      typedef std::map<const TNamed*,     G4UserLimits*>            LimitMap;
      typedef std::map<const TGeoNode*,   G4PVPlacement*>           PlacementMap;
      typedef std::map<const TNamed*,     G4Region*>                RegionMap;
      typedef std::map<const TNamed*,     Geant4SensitiveDetector*> SensDetMap;
      typedef std::map<const TGeoVolume*, G4LogicalVolume*>         VolumeMap;
      typedef std::map<const TGeoShape*,  G4VSolid*>                SolidMap;
      typedef std::map<const TNamed*,     G4VisAttributes*>         VisMap;
      struct G4GeometryInfo : public GeometryInfo {
	ElementMap              g4Elements;
	MaterialMap             g4Materials;
	SolidMap                g4Solids;
	VolumeMap               g4Volumes;
	PlacementMap            g4Placements;
	RegionMap               g4Regions;
	VisMap                  g4Vis;
	LimitMap                g4Limits;
	SensDetMap              g4SensDets;

	SensitiveVolumes   sensitives;
	RegionVolumes      regions;
	LimitVolumes       limits;
      };
    protected:
      LCDD&           m_lcdd;
      G4GeometryInfo* m_dataPtr;

      /// When resolving pointers, we must check for the validity of the data block
      void checkValidity() const;
    public:
      /// Access to the data pointer
      G4GeometryInfo& data() const { return *m_dataPtr; }
      /// Release data and pass over the ownership
      G4GeometryInfo* detach();
      /// Set a new data block
      void attach(G4GeometryInfo* data);
      /// Initializing Constructor
      Geant4Mapping(LCDD& lcdd, G4GeometryInfo* data);
      /// Standard destructor
      virtual ~Geant4Mapping();
      /// Possibility to define a singleton instance
      static Geant4Mapping& instance();

      /// Accesor to the LCDD instance
      LCDD& lcdd() const {  return m_lcdd; }

      /// Accessor to resolve G4 placements
      G4PVPlacement* g4Placement(const TGeoNode* node)  const;
      /// Accessor to resolve geometry placements
      PlacedVolume placement(const G4VPhysicalVolume* node)  const;



    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4MAPPING_H
