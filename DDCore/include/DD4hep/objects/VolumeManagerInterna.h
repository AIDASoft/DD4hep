// $Id: VolumeManager.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_VOLUMEMANAGERINTERNA_H
#define DD4HEP_GEOMETRY_VOLUMEMANAGERINTERNA_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/VolumeManager.h"

// ROOT include files
#include "TGeoMatrix.h"
/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class VolumeManagerContext;
    class VolumeManagerObject;

    /** @class VolumeManagerContext  VolumeManager.h DD4hep/lcdd/VolumeManager.h
     *
     *  This structure describes the cached data for one placement
     *
     * @author  M.Frank
     * @version 1.0
     */
    class VolumeManagerContext {
    public:
      typedef std::vector<const TGeoNode*> Path;
      typedef PlacedVolume::VolIDs::Base VolIDs;

      /// Placement identifier
      VolumeID identifier;
      /// Ignore mask of the placement identifier
      VolumeID mask;
      /// The placement
      PlacedVolume placement;
      /// Handle to the subdetector element handle
      DetElement detector;
      /// Handle to the closest Detector element
      DetElement element;
      /// The transformation of space-points to the corrdinate system of the closests detector element
      TGeoHMatrix toDetector;
      /// The transformation of space-points to the world corrdinate system
      TGeoHMatrix toWorld;
      /// Volume IDS corresponding to this element
      VolIDs volID;
      /// Path of placements to this sensitive volume
      Path path;
    public:
      /// Default constructor
      VolumeManagerContext();
      /// Default destructor
      virtual ~VolumeManagerContext();
    };

    /** @class VolumeManagerObject  VolumeManager.h DD4hep/lcdd/VolumeManager.h
     *
     * This structure describes the internal data of the volume manager object
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct VolumeManagerObject: public NamedObject {
    public:
      typedef IDDescriptor::Field Field;
      typedef VolumeManager::Managers Managers;
      typedef VolumeManager::Detectors Detectors;
      typedef VolumeManager::Volumes Volumes;
      typedef VolumeManager::Context Context;

    public:
      /// Reference to the LCDD instance
      LCDD* lcdd;
      /// The container of subdetector elements
      Detectors subdetectors;
      /// The volume managers for the individual subdetector elements
      Managers managers;
      /// The container of placements managed by this instance
      Volumes volumes;
      /// The Detector element handle managed by this instance
      DetElement detector;
      /// The ID descriptor object
      IDDescriptor id;
      /// The reference to the TOP level VolumeManager
      VolumeManagerObject* top;
      /// The system field descriptor
      Field system;
      /// System identifier
      VolumeID sysID;
      /// Sub-detector mask
      VolumeID detMask;
      /// Population flags
      int flags;
    public:
      /// Default constructor
      VolumeManagerObject(LCDD& lcdd);
      /// Default destructor
      virtual ~VolumeManagerObject();
      /// Search the locally cached volumes for a matching ID
      Context* search(const VolumeID& id) const;
      /// Update callback when alignment has changed (called only for subdetectors....)
      void update(unsigned long tags, DetElement& det, void* param);
    };

    /// Enable printouts for debugging
    std::ostream& operator<<(std::ostream& os, const VolumeManager& m);

  } /* End namespace Geometry               */
} /* End namespace DD4hep                */
#endif    /* DD4hep_GEOMETRY_VOLUMEMANAGERINTERNA_H           */
