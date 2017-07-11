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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the VolumeManager include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_VOLUMEMANAGERINTERNA_H
#define DD4HEP_DDCORE_VOLUMEMANAGERINTERNA_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/VolumeManager.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// This structure describes the internal data of the volume manager object
    /**
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_CORE
     */
    class VolumeManagerObject: public NamedObject {
    public:
      /// The container of subdetector elements
      std::map<DetElement, VolumeManager>       subdetectors;
      /// The volume managers for the individual subdetector elements
      std::map<VolumeID, VolumeManager>         managers;
      /// The container of placements managed by this instance
      std::map<VolumeID, VolumeManagerContext*> volumes;
      /// The Detector element handle managed by this instance
      DetElement detector;
      /// The ID descriptor object
      IDDescriptor id;
      /// The reference to the TOP level VolumeManager
      VolumeManagerObject* top    = 0;
      /// The system field descriptor
      const BitFieldValue* system = 0;   //! Not ROOT persistent
      /// System identifier
      VolumeID sysID              = 0;
      /// Sub-detector mask
      VolumeID detMask            = ~0x0ULL;
      /// Population flags
      int flags                   = VolumeManager::NONE;
    public:
      /// Default constructor
      VolumeManagerObject() = default;
      /// No move constructor
      VolumeManagerObject(VolumeManagerObject&& copy) = delete;
      /// No copy constructor
      VolumeManagerObject(const VolumeManagerObject& copy) = delete;
      /// Default destructor
      virtual ~VolumeManagerObject();
      /// No move assignment
      VolumeManagerObject& operator=(VolumeManagerObject&& copy) = delete;
      /// No copy assignment
      VolumeManagerObject& operator=(const VolumeManagerObject& copy) = delete;
      /// Search the locally cached volumes for a matching ID
      VolumeManagerContext* search(const VolumeID& id) const;
      /// Update callback when alignment has changed (called only for subdetectors....)
      void update(unsigned long tags, DetElement& det, void* param);
    };

  } /* End namespace detail               */
} /* End namespace dd4hep                */
#endif    /* DD4HEP_DDCORE_VOLUMEMANAGERINTERNA_H           */
