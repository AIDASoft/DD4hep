//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4HEP_DDG4_GEANT4MAPPING_H
#define DD4HEP_DDG4_GEANT4MAPPING_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/GeoHandler.h"
#include "DDG4/Geant4GeometryInfo.h"
#include "DDG4/Geant4VolumeManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Geometry mapping from DD4hep to Geant 4.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Mapping: public Geometry::GeoHandlerTypes {
    public:
      typedef Geometry::LCDD LCDD;
      typedef Geometry::DetElement DetElement;
      typedef Geometry::SensitiveDetector SensitiveDetector;
      typedef Geometry::Solid Solid;
      typedef Geometry::Volume Volume;
      typedef Geometry::PlacedVolume PlacedVolume;
      typedef Geometry::Atom Atom;
      typedef Geometry::Material Material;
      typedef Geometry::Region Region;
      typedef Geometry::VisAttr VisAttr;
      typedef Geometry::LimitSet LimitSet;

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

      /// Access to the data pointer
      Geant4GeometryInfo* ptr() const {
        return m_dataPtr;
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

#endif // DD4HEP_DDG4_GEANT4MAPPING_H
