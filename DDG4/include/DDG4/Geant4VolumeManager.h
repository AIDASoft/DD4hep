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
#ifndef DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H
#define DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/IDDescriptor.h"
#include "DDG4/Geant4Primitives.h"

// Geant4 forward declarations
class G4VTouchable;
class G4VPhysicalVolume;


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4VolumeManager;
    class Geant4GeometryInfo;

    /// The Geant4VolumeManager to facilitate optimized lookups of cell IDs from touchables.
    /** @class Geant4VolumeManager Geant4VolumeManager.h DDG4/Geant4VolumeManager.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4VolumeManager : public Handle<Geant4GeometryInfo>   {
    protected:
      /// Optimization flag to shortcut object checks
      mutable bool m_isValid = false;

      /// Check the validity of the information before accessing it.
      bool checkValidity() const;

    public:
      static const VolumeID InvalidPath = VolumeID(-1LL);
      static const VolumeID Insensitive = VolumeID(-2LL);
      static const VolumeID NonExisting = 0ULL;

      /// Initializing constructor. The tree will automatically be built if possible
      Geant4VolumeManager(const Detector& description, Geant4GeometryInfo* info);
      /// Default constructor
      Geant4VolumeManager() = default;
      /// Constructor to be used when reading the already parsed object
      Geant4VolumeManager(const Handle<Geant4GeometryInfo>& e)
        : Handle<Geant4GeometryInfo>(e), m_isValid(false) {  }
      /// Constructor to be used when reading the already parsed object
      Geant4VolumeManager(const Geant4VolumeManager& e) = default;
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Geant4VolumeManager(const Handle<Q>& e)
        : Handle<Geant4GeometryInfo>(e), m_isValid(false) {
      }
      /// Assignment operator
      Geant4VolumeManager& operator=(const Geant4VolumeManager& c) = default;

      /// Helper: Generate placement path from touchable object
      std::vector<const G4VPhysicalVolume*>
      placementPath(const G4VTouchable* touchable, bool exception = true) const;
      /// Access CELLID by placement path
      VolumeID volumeID(const std::vector<const G4VPhysicalVolume*>& path) const;
      /// Access CELLID by Geant4 touchable object
      VolumeID volumeID(const G4VTouchable* touchable) const;
      /// Accessfully decoded volume fields  by placement path
      void volumeDescriptor(const std::vector<const G4VPhysicalVolume*>&   path,
                            std::pair<VolumeID,std::vector<std::pair<const BitFieldElement*, VolumeID> > >& volume_desc) const;
      /// Access fully decoded volume fields by Geant4 touchable object
      void volumeDescriptor(const G4VTouchable* touchable,
                            std::pair<VolumeID,std::vector<std::pair<const BitFieldElement*, VolumeID> > >& volume_desc) const;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H
