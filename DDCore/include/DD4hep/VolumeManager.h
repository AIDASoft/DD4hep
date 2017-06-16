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

#ifndef DD4HEP_GEOMETRY_VOLUMEMANAGER_H
#define DD4HEP_GEOMETRY_VOLUMEMANAGER_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/ConditionsMap.h"

// ROOT include files
#include "TGeoMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {
    class VolumeManagerObject;
  }
  
  /// This structure describes the cached data for one placement held by the volume manager
  /**
   *  This structure is slightly optimized, since there are soooo many instances:
   *  If the sensitive volume is associated as a placement to the DetElement, then
   *  -- placement()  returns element.placement()
   *  -- toElement()  returns an identity matrix
   *
   *  If the sensitive volume is NOT the element's placement,
   *  then the calls are forwarded to an appended invisible structure at the end
   *  of the memory.
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_GEOMETRY
   */
  class VolumeManagerContext {
  public:
    /// Handle to the closest Detector element
    DetElement   element{0};
    /// Placement identifier
    VolumeID     identifier = 0;
    /// Ignore mask of the placement identifier
    VolumeID     mask       = ~0x0ULL;
    /// Flag to indicate optional information
    long         flag       = 0;
  public:
    /// Default constructor
    VolumeManagerContext() = default;
    /// Inhibit copy constructor
    VolumeManagerContext(const VolumeManagerContext& copy) = delete;      
    /// Inhibit move constructor
    VolumeManagerContext(VolumeManagerContext&& copy) = delete;      
    /// Inhibit assignment
    VolumeManagerContext& operator=(const VolumeManagerContext& copy) = delete;      
    /// Inhibit move assignment
    VolumeManagerContext& operator=(VolumeManagerContext&& copy) = delete;      
    /// Default destructor
    virtual ~VolumeManagerContext();
    /// Acces the sensitive volume placement
    PlacedVolume placement()  const;
    /// Access the transformation to the closest detector element
    const TGeoHMatrix& toElement()  const;
  };
    
  /// Class to support the retrieval of detector elements and volumes given a valid identifier
  /**
   *
   *  The VolumeManager manages the repository of sensitive physical
   *  volumes (placements). These volumes can then be accessed together with
   *  basic other elements, such as the top level Detector element of the
   *  subdetector or the coordinate trasformation to world coordinates.
   *    The basic key to access this information is the 'VolumeID', a 64 bit
   *  number, which uniquely identifies a placement.
   *
   *  There are 2 working modes:
   *
   *  1) Starting from the top level element, the next level is addressed
   *  separately and the volumes are stored in a 2-level 'tree'. Placements
   *  this way are partitioned e.g. 'by subdetector'. This tries to keep
   *  the number of mapped elements at a reasonable level.
   *
   *  2) All placements are mapped to the top-level element. Good for small
   *  setups. Note, that the system field of all IDDescriptors of the
   *  subdetectors must have the same length to ensure the uniqueness of the
   *  placement keys.
   *
   *  By default the volume manager in TREE mode (-> 1)) is attached to the
   *  Detector instance and also managed by this instance.
   *  If you wish to create instances yourself, you must ensure that the
   *  handle is destroyed once no other references are present.
   *  [call DestroyHandle(VolumeManager)]
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_GEOMETRY
   */
  class VolumeManager: public Handle<detail::VolumeManagerObject> {
  public:
    // Creation flags
    enum PopulateFlags {
      NONE = 0,
      TREE = 1 << 1,   // Build 1 level DetElement hierarchy while populating
      ONE  = 1 << 2,   // Populate all daughter volumes into one big lookup-container
      // This flag may be in parallel with 'TREE'
      LAST
    };

  protected:
    /// Additional data accessor
    Object& _data() const {
      return *(Object*) ptr();
    }

  public:
    /// Default constructor
    VolumeManager() = default;

    /// Constructor to be used when reading the already parsed object
    VolumeManager(const VolumeManager& e) = default;
#ifndef __CINT__
    /// Constructor to be used when reading the already parsed object
    VolumeManager(const Handle<detail::VolumeManagerObject>& e)
      : Handle<detail::VolumeManagerObject>(e) {
    }
#endif
    /// Constructor to be used when reading the already parsed object
    template <typename Q>
    VolumeManager(const Handle<Q>& e) : Handle<detail::VolumeManagerObject>(e) { }
    /** Initializing constructor. The tree will automatically be built if the detelement is valid
     *  Please see enum PopulateFlags for further info.
     *  No action whatsoever is performed here, if the detector element is not valid.
     */
    VolumeManager(Detector& description,
                  const std::string& name,
                  DetElement world = DetElement(),
                  Readout ro = Readout(),
                  int flags =
                  NONE);
    /// Initializing constructor for subdetector volume managers.
    VolumeManager(DetElement subdetector, Readout ro);

    /// static accessor calling dd4hepVolumeManagerPlugin if necessary
    static VolumeManager getVolumeManager(Detector& description);

    /// Assignment operator
    VolumeManager& operator=(const VolumeManager& m) = default;

    /// Add a new Volume manager section according to a new subdetector
    VolumeManager addSubdetector(DetElement detector, Readout ro);
    /// Access the volume manager by cell id
    VolumeManager subdetector(VolumeID id) const;

    /// Access the top level detector element
    DetElement detector() const;
    /// Access IDDescription structure
    IDDescriptor idSpec() const;

    /// Register physical volume with the manager (normally: section manager)
    bool adoptPlacement(VolumeManagerContext* context);
    /// Register physical volume with the manager and pre-computed volume id
    bool adoptPlacement(VolumeID volume_id, VolumeManagerContext* context);

    /** This set of functions is required when reading/analyzing
     *  already created hits which have a VolumeID attached.
     */
    /// Lookup the context, which belongs to a registered physical volume.
    VolumeManagerContext* lookupContext(VolumeID volume_id) const;
    /// Lookup a physical (placed) volume identified by its 64 bit hit ID
    PlacedVolume lookupPlacement(VolumeID volume_id) const;
    /// Convenience routine: Lookup a top level subdetector detector element according to a contained 64 bit hit ID
    DetElement lookupDetector(VolumeID volume_id) const;
    /// Convenience routine: Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
    DetElement lookupDetElement(VolumeID volume_id) const;
    /// Convenience routine: Access the transformation of a physical volume to the world coordinate system
    const TGeoMatrix& worldTransformation(const ConditionsMap& map,
                                          VolumeID volume_id) const;
  };

  /// Enable printouts for debugging
  std::ostream& operator<<(std::ostream& os, const VolumeManager& m);
}         /* End namespace dd4hep                */
#endif    /* dd4hep_GEOMETRY_READOUT_H           */
