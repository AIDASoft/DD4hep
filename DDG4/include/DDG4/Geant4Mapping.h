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
#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include "DDG4/Geant4GeometryInfo.h"
#include "DDG4/Geant4VolumeManager.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4Mapping Geant4Mapping.h DDG4/Geant4Mapping.h
     *
     * Geometry mapping from DD4hep to Geant 4.
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Mapping: public Geometry::GeoHandlerTypes {
    public:
      typedef Geometry::LCDD LCDD;
      typedef Geometry::DetElement DetElement;
      typedef Geometry::SensitiveDetector SensitiveDetector;
      typedef Geometry::Solid Solid;
      typedef Geometry::Volume Volume;
      typedef Geometry::PlacedVolume PlacedVolume;
      typedef Geometry::Material Material;
      typedef Geometry::Region Region;

      typedef Geant4GeometryInfo::PlacementPath PlacementPath;
      typedef Geant4GeometryInfo::AssemblyChildren AssemblyChildren;
      typedef Geant4GeometryInfo::ElementMap ElementMap;
      typedef Geant4GeometryInfo::MaterialMap MaterialMap;
      typedef Geant4GeometryInfo::LimitMap LimitMap;
      typedef Geant4GeometryInfo::PlacementMap PlacementMap;
      typedef Geant4GeometryInfo::AssemblyChildMap AssemblyChildMap;
      typedef Geant4GeometryInfo::RegionMap RegionMap;
      typedef Geant4GeometryInfo::SensDetMap SensDetMap;
      typedef Geant4GeometryInfo::VolumeMap VolumeMap;
      typedef Geant4GeometryInfo::SolidMap SolidMap;
      typedef Geant4GeometryInfo::VisMap VisMap;
      typedef Geant4GeometryInfo::PathMap PathMap;

    protected:
      LCDD& m_lcdd;
      Geant4GeometryInfo* m_dataPtr;

      /// When resolving pointers, we must check for the validity of the data block
      void checkValidity() const;
    public:
      /// Initializing Constructor
      Geant4Mapping(LCDD& lcdd);

      /// Standard destructor
      virtual ~Geant4Mapping();

      /// Possibility to define a singleton instance
      static Geant4Mapping& instance();

      /// Accesor to the LCDD instance
      LCDD& lcdd() const {
        return m_lcdd;
      }

      /// Access to the data pointer
      Geant4GeometryInfo& data() const {
        return *m_dataPtr;
      }

      /// Create and attach new data block. Delete old data block if present.
      Geant4GeometryInfo& init();

      /// Release data and pass over the ownership
      Geant4GeometryInfo* detach();

      /// Set a new data block
      void attach(Geant4GeometryInfo* data);

      /// Access the volume manager
      Geant4VolumeManager volumeManager() const;

      /// Accessor to resolve geometry placements
      PlacedVolume placement(const G4VPhysicalVolume* node) const;
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4MAPPING_H
