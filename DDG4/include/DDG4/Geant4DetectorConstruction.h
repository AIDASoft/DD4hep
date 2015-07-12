// $Id$
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

#ifndef DD4HEP_GEANT4DETECTORCONSTRUCTION_H
#define DD4HEP_GEANT4DETECTORCONSTRUCTION_H

// Framework include files
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4GeometryInfo.h"

// Geant4 include files
#include "G4VUserDetectorConstruction.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
  }

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Kernel;

    /// Class to create Geant4 detector geometry from TGeo representation in memory
    /**
     *  On demand (ie. when calling "Construct") the DD4hep geometry is converted
     *  to Geant4 with all volumes, assemblies, shapes, materials etc.
     *  The actuak work is performed by the Geant4Converter class called by this method.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DetectorConstruction : public Geant4Action, public G4VUserDetectorConstruction {
    public:
      /// Instance accessor
      static Geant4DetectorConstruction* instance(Geant4Kernel& kernel);
      /// Initializing constructor for DDG4
      Geant4DetectorConstruction(Geant4Kernel& kernel);
      /// Initializing constructor for other clients
      Geant4DetectorConstruction(Geometry::LCDD& lcdd);
      /// Default destructor
      virtual ~Geant4DetectorConstruction();
      /// Geometry construction callback: Invoke the conversion to Geant4
      G4VPhysicalVolume* Construct();

      //@{ Accessor to the various geant4 maps after construction

      /// Access to the converted materials
      const Geant4GeometryMaps::MaterialMap& materials() const;
      /// Access to the converted elements
      const Geant4GeometryMaps::ElementMap& elements() const;
      /// Access to the converted shapes
      const Geant4GeometryMaps::SolidMap& shapes() const;
      /// Access to the converted volumes
      const Geant4GeometryMaps::VolumeMap& volumes() const;
      /// Access to the converted placements
      const Geant4GeometryMaps::PlacementMap& placements() const;
      /// Access to the converted assemblys
      const Geant4GeometryMaps::AssemblyMap& assemblies() const;

      /// Access to the converted limit sets
      const Geant4GeometryMaps::LimitMap& limits() const;
      /// Access to the converted regions
      const Geant4GeometryMaps::RegionMap& regions() const;
      /// Access to the converted sensitive detectors
      const Geant4GeometryMaps::SensDetMap& sensitives() const;

      //@}

    private:
      /// Printlevel used for the geometry conversion
      PrintLevel m_outputLevel;
      /// Reference to geometry object
      Geometry::LCDD& m_lcdd;
      /// Reference to the world after construction
      G4VPhysicalVolume* m_world;
    };
  }
}

#endif
