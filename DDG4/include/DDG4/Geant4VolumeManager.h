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
#ifndef DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H
#define DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/IDDescriptor.h"
#include "DDG4/Geant4Primitives.h"
#include "DDG4/Geant4GeometryInfo.h"

// Geant4 forward declarations
class G4VTouchable;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4VolumeManager;
    class Geant4GeometryInfo;

    /// The Geant4VolumeManager to facilitate optimized lookups of cell IDs from touchables.
    /** @class Geant4VolumeManager Geant4VolumeManager.h DDG4/Geant4VolumeManager.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4VolumeManager: public Geometry::Handle<Geant4GeometryInfo> {
    public:
      // Forward declarations
      typedef Geometry::Handle<Geant4GeometryInfo> Base;
      typedef Geometry::PlacedVolume PlacedVolume;
      typedef Geometry::IDDescriptor IDDescriptor;
      typedef IDDescriptor::VolIDFields VolIDFields;
      typedef std::pair<VolumeID, VolIDFields> VolIDDescriptor;
      typedef Geant4GeometryMaps::Geant4PlacementPath PlacementPath;
      typedef Geant4GeometryInfo Object;

    protected:
      /// Optimization flag to shortcut object checks
      mutable bool m_isValid;

      /// Check the validity of the information before accessing it.
      bool checkValidity() const;

    public:
      static const VolumeID InvalidPath = VolumeID(-1LL);
      static const VolumeID Insensitive = VolumeID(-2LL);
      static const VolumeID NonExisting = 0ULL;

      /// Initializing constructor. The tree will automatically be built if possible
      Geant4VolumeManager(Geometry::LCDD& lcdd, Geant4GeometryInfo* info);
      /// Default constructor
      Geant4VolumeManager()
        : Base(), m_isValid(false) {
      }
      /// Constructor to be used when reading the already parsed object
      Geant4VolumeManager(const Base& e)
        : Base(e), m_isValid(false) {
      }
      /// Constructor to be used when reading the already parsed object
      Geant4VolumeManager(const Geant4VolumeManager& e)
        : Base(e), m_isValid(false) {
      }
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Geant4VolumeManager(const Geometry::Handle<Q>& e)
        : Base(e), m_isValid(false) {
      }
      /// Assignment operator
      Geant4VolumeManager& operator=(const Geant4VolumeManager& c)  {
        if ( this != &c ) {
          m_element = c.m_element;
          m_isValid = c.m_isValid;
        }
        return *this;
      }

      /// Helper: Generate placement path from touchable object
      PlacementPath placementPath(const G4VTouchable* touchable, bool exception = true) const;
      /// Access CELLID by placement path
      VolumeID volumeID(const PlacementPath& path) const;
      /// Access CELLID by Geant4 touchable object
      VolumeID volumeID(const G4VTouchable* touchable) const;
      /// Accessfully decoded volume fields  by placement path
      void volumeDescriptor(const PlacementPath& path, VolIDDescriptor& volume_desc) const;
      /// Access fully decoded volume fields by Geant4 touchable object
      void volumeDescriptor(const G4VTouchable* touchable, VolIDDescriptor& volume_desc) const;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H
