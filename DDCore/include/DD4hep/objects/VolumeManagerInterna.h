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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the VolumeManager include file should be
// sufficient for all practical purposes.
//
//==========================================================================

#ifndef DD4HEP_GEOMETRY_VOLUMEMANAGERINTERNA_H
#define DD4HEP_GEOMETRY_VOLUMEMANAGERINTERNA_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/VolumeManager.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class VolumeManagerContext;
    class VolumeManagerObject;

    /// This structure describes the cached data for one placement held by the volume manager
    /**
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_GEOMETRY
     */
    class VolumeManagerContext {
    public:
      /// Placement identifier
      VolumeID     identifier;
      /// Ignore mask of the placement identifier
      [[gnu::deprecated("This member variable might get axed if it is not used, please tell us if you do")]]
      VolumeID mask;
      /// The placement
      [[gnu::deprecated("This member variable might get axed if it is not used, please tell us if you do")]]
      PlacedVolume placement;
      /// Handle to the subdetector element handle
      [[gnu::deprecated("This member variable might get axed if it is not used, please tell us if you do")]]
      DetElement detector;
      /// Handle to the closest Detector element
      DetElement   element;
      /// The transformation of space-points to the world corrdinate system
      TGeoHMatrix  toWorld;
      /// The transformation of space-points to the corrdinate system of the closests detector element
      [[gnu::deprecated("This member variable might get axed if it is not used, please tell us if you do")]]
      TGeoHMatrix toDetector;
      /// The transformation of space-points to the world corrdinate system
      TGeoHMatrix toWorld;
    public:
      /// Default constructor
      VolumeManagerContext();
      /// Default destructor
      virtual ~VolumeManagerContext();
    };

    /// This structure describes the internal data of the volume manager object
    /**
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_GEOMETRY
     */
    class VolumeManagerObject: public NamedObject {
    public:
      typedef IDDescriptor::Field Field;
      typedef VolumeManager::Managers Managers;
      typedef VolumeManager::Detectors Detectors;
      typedef VolumeManager::Volumes Volumes;
      typedef VolumeManager::Context Context;

    public:
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
      VolumeManagerObject();
      /// Default destructor
      virtual ~VolumeManagerObject();
      /// Search the locally cached volumes for a matching ID
      Context* search(const VolumeID& id) const;
      /// Update callback when alignment has changed (called only for subdetectors....)
      void update(unsigned long tags, DetElement& det, void* param);
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                */
#endif    /* DD4hep_GEOMETRY_VOLUMEMANAGERINTERNA_H           */
