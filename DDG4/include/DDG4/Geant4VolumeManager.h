// $Id: Geant4VolumeManager.h 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H
#define DD4HEP_DDG4_GEANT4VOLUMEMANAGER_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/IDDescriptor.h"
#include "DDG4/Geant4Primitives.h"
#include "DDG4/Geant4GeometryInfo.h"

// Geant4 forward declarations
class G4VTouchable;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4VolumeManager;
    class Geant4GeometryInfo;

    /** @class Geant4VolumeManager Geant4VolumeManager.h DDG4/Geant4VolumeManager.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4VolumeManager : public Geometry::Handle<Geant4GeometryInfo>  {
    public:
      // Forward declarations
      typedef Geometry::Handle<Geant4GeometryInfo>  Base;
      typedef Geometry::PlacedVolume                PlacedVolume;
      typedef Geometry::IDDescriptor                IDDescriptor;
      typedef IDDescriptor::VolIDFields             VolIDFields;
      typedef std::pair<VolumeID, VolIDFields>      VolIDDescriptor;
      typedef Geant4GeometryInfo::PlacementPath     PlacementPath;
      typedef Geant4GeometryInfo                    Object;

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
      Geant4VolumeManager() : Base(), m_isValid(false) {}
      /// Constructor to be used when reading the already parsed object
      Geant4VolumeManager(const Base& e) : Base(e), m_isValid(false) {}
      /// Constructor to be used when reading the already parsed object
      Geant4VolumeManager(const Geant4VolumeManager& e) : Base(e), m_isValid(false) {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Geant4VolumeManager(const Geometry::Handle<Q>& e) : Base(e), m_isValid(false) {}

      /// Helper: Generate placement path from touchable object
      PlacementPath placementPath(const G4VTouchable* touchable, bool exception=true) const;

      /// Accessor to resolve TGeo geometry placements from Geant4 placements
      PlacedVolume placement(const G4VPhysicalVolume* node)  const;
      /// Accessor to resolve Geant4 geometry placements from TGeo placements
      G4VPhysicalVolume* placement(const TGeoNode* node)  const;
      /// Accessor to resolve Geant4 geometry placements from TGeo placements
      G4VPhysicalVolume* placement(const PlacedVolume& node)  const   
      {  	return placement(node.ptr());      }
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
