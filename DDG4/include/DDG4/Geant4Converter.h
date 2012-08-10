// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4CONVERTER_H
#define DD4HEP_GEANT4CONVERTER_H

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
class Geant4SensitiveDetector;
class G4UserLimits;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {


    class Geant4SensitiveDetector;

    /** @class Geant4Converter Geant4Converter.h DDG4/Geant4Converter.h
     * 
     * Geometry converter from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Converter : public Geometry::GeoHandler  {
      typedef std::map<const TNamed*,G4UserLimits*> LimitMap;

      struct G4GeometryInfo : public GeometryInfo {
	std::map<const TGeoElement*,G4Element*>                g4Elements;
	std::map<const TGeoMedium*, G4Material*>               g4Materials;
	std::map<const TGeoShape*,  G4VSolid*>                 g4Solids;
	std::map<const TGeoVolume*, G4LogicalVolume*>          g4Volumes;
	std::map<const TGeoNode*,   G4PVPlacement*>            g4Placements;
	std::map<const TNamed*,     G4Region*>                 g4Regions;
	LimitMap                                               g4Limits;
	std::map<const TNamed*,     Geant4SensitiveDetector*>  g4SensDets;

	SensitiveVolumes   sensitives;
	RegionVolumes      regions;
	LimitVolumes       limits;
      };

      LCDD&           m_lcdd;
      G4GeometryInfo* m_dataPtr;
      G4GeometryInfo& data() const { return *m_dataPtr; }
      
      /// Constructor
      Geant4Converter( LCDD& lcdd ) : m_lcdd(lcdd) {}
      /// Standard destructor
      virtual ~Geant4Converter() {}
      /// Create geometry dump
      void create(DetElement top);

      /// Convert the geometry type material into the corresponding Geant4 object(s).
      virtual void* handleMaterial(const std::string& name, const TGeoMedium* medium) const;
      /// Convert the geometry type element into the corresponding Geant4 object(s).
      virtual void* handleElement(const std::string& name, const TGeoElement* element) const;
      /// Convert the geometry type solid into the corresponding Geant4 object(s).
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;
      /// Convert the geometry type logical volume into the corresponding Geant4 object(s).
      virtual void* handleVolume(const std::string& name, const TGeoVolume* volume) const;
      /// Convert the geometry type volume placement into the corresponding Geant4 object(s).
      virtual void* handlePlacement(const std::string& name, const TGeoNode* node) const;

      /// Convert the geometry type region into the corresponding Geant4 object(s).
      virtual void* handleRegion(const TNamed* region, const std::set<const TGeoVolume*>& volumes) const;
      /// Convert the geometry type LimitSet into the corresponding Geant4 object(s).
      virtual void* handleLimitSet(const TNamed* limitset, const std::set<const TGeoVolume*>& volumes) const;
      /// Convert the geometry type SensitiveDetector into the corresponding Geant4 object(s).
      virtual void* handleSensitive(const TNamed* sens_det, const std::set<const TGeoVolume*>& volumes) const;
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4CONVERTER_H
