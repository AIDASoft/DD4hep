// $Id: VolumeManager.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_GEOMETRY_VOLUMEMANAGER_H
#define DD4hep_GEOMETRY_VOLUMEMANAGER_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/Detector.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/IDDescriptor.h"

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
     *  LCDD instance and also managed by this instance.
     *  If you wish to create instances yourself, you must ensure that the
     *  handle is destroyed once no other references are present.
     *  [call DestroyHandle(VolumeManager)]
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_GEOMETRY
     */
    struct VolumeManager: public Handle<VolumeManagerObject> {
    public:
      typedef DD4hep::VolumeID VolumeID;
      typedef VolumeID VolIdentifier;
      // Creation flags
      enum PopulateFlags {
        NONE = 0, 
	TREE = 1 << 1,   // Build 1 level DetElement hierarchy while populating
        ONE = 1 << 2,    // Populate all daughter volumes into one big lookup-container
        // This flag may be in parallel with 'TREE'
        LAST
      };

      /// Some useful Container abbreviations used by the VolumeManager
      typedef VolumeManagerObject Object;
      typedef VolumeManagerContext Context;
      typedef std::map<VolumeID, VolumeManager> Managers;
      typedef std::map<DetElement, VolumeManager> Detectors;
      typedef std::map<VolumeID, VolumeManagerContext*> Volumes;
      typedef PlacedVolume::VolIDs VolIDs;
      typedef std::map<TGeoNode*, Context*> PhysVolumes;

    protected:
      /// Additional data accessor
      Object& _data() const {
        return *(Object*) ptr();
      }

    public:
      /// Default constructor
      VolumeManager()
          : Handle<VolumeManagerObject>() {
      }
      /// Constructor to be used when reading the already parsed object
      VolumeManager(const VolumeManager& e)
          : Handle<VolumeManagerObject>(e) {
      }
#ifndef __CINT__
      /// Constructor to be used when reading the already parsed object
      VolumeManager(const Handle<VolumeManagerObject>& e)
          : Handle<VolumeManagerObject>(e) {
      }
#endif
      /// Constructor to be used when reading the already parsed object
      template <typename Q> VolumeManager(const Handle<Q>& e)
          : Handle<VolumeManagerObject>(e) {
      }
      /** Initializing constructor. The tree will automatically be built if the detelement is valid
       *  Please see enum PopulateFlags for further info.
       *  No action whatsoever is performed here, if the detector element is not valid.
       */
      VolumeManager(LCDD& lcdd, const std::string& name, DetElement world = DetElement(), Readout ro = Readout(), int flags =
          NONE);
      /// Initializing constructor for subdetector volume managers.
      VolumeManager(DetElement subdetector, Readout ro);

      /// Assignment operator
      VolumeManager& operator=(const VolumeManager& m)  {
	if ( this != &m ) m_element = m.m_element;
	return *this;
      }      
      /// Add a new Volume manager section according to a new subdetector
      VolumeManager addSubdetector(DetElement detector, Readout ro);
      /// Access the volume manager by cell id
      VolumeManager subdetector(VolumeID id) const;

      /// Access the top level detector element
      DetElement detector() const;
      /// Access IDDescription structure
      IDDescriptor idSpec() const;

      /// Register physical volume with the manager (normally: section manager)
      bool adoptPlacement(Context* context);
      /// Register physical volume with the manager and pre-computed volume id
      bool adoptPlacement(VolumeID volume_id, Context* context);

      /** This set of functions is required when reading/analyzing
       *  already created hits which have a VolumeID attached.
       */
      /// Lookup the context, which belongs to a registered physical volume.
      Context* lookupContext(VolumeID volume_id) const;
      /// Lookup a physical (placed) volume identified by its 64 bit hit ID
      PlacedVolume lookupPlacement(VolumeID volume_id) const;
      /// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
      DetElement lookupDetector(VolumeID volume_id) const;
      /// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
      DetElement lookupDetElement(VolumeID volume_id) const;
      /// Access the transformation of a physical volume to the world coordinate system
      const TGeoMatrix& worldTransformation(VolumeID volume_id) const;
    };

    /// Enable printouts for debugging
    std::ostream& operator<<(std::ostream& os, const VolumeManager& m);

  } /* End namespace Geometry               */
} /* End namespace DD4hep                */
#endif    /* DD4hep_GEOMETRY_READOUT_H           */
