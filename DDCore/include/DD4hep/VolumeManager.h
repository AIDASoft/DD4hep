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
#include "DD4hep/IDDescriptor.h"

// ROOT include files
#include "TGeoMatrix.h"
/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {

    /** @class VolumeManager  VolumeManager.h DD4hep/lcdd/VolumeManager.h
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
     * @author  M.Frank
     * @version 1.0
     */
    struct VolumeManager : public Ref_t {
    public:
      typedef unsigned long long int VolumeID;

      // Creation flags
      enum PopulateFlags {
	NONE = 0,
	TREE = 1<<1,  // Build 1 level DetElement hierarchy while populating
	ONE  = 1<<2,  // Populate all daughter volumes into one big lookup-container
	              // This flag may be in parallel with 'TREE'
	LAST
      };

#if 0
      /** @class VolumeManager::VolIdentifier  VolumeManager.h DD4hep/lcdd/VolumeManager.h
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct VolIdentifier  {
      public:
	/// The volume identifier and it's mask
	VolumeID identifier, mask;
      private:
	/// Equality operator is provate. Not to be used!
	bool operator==(const VolIdentifier& c) const {
	  return identifier == c.identifier && mask == c.mask; 
	}
      public:
	/// Default constructor
        VolIdentifier() : identifier(0), mask(~0x0ull)   {
	}
	/// Initializing constructor
        VolIdentifier(VolumeID id, VolumeID msk) : identifier(id), mask(msk) 	{
	}
	/// Copt constructor
        VolIdentifier(const VolIdentifier& c) : identifier(c.identifier), mask(c.mask) 	{
	}
	/** Operator less for map storage. 
	 *  Note, that this takes advantag, the only volumes with the same descriptor length are 
	 *  stored in one map, because some bits are masked out. Otherwise all breaks!
	 */
	bool operator< (const VolIdentifier& c) const { 
	  return identifier < (c.identifier&mask); 
	}
	/// Assignment operator
	VolIdentifier& operator=(const VolIdentifier& c) {
	  identifier=c.identifier; mask=c.mask; 
	  return *this;
	}
      };
#endif
      typedef VolumeID VolIdentifier;

      /** @class VolumeManager::Context  VolumeManager.h DD4hep/lcdd/VolumeManager.h
       *
       *  This structure describes the cached data for one placement
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Context   {
      public:
	/// Placement identifier
	VolumeID      identifier;
	/// Ignore mask of the placement identifier
	VolumeID      mask;
	/// The placement
	PlacedVolume  placement;
	/// Handle to the subdetector element handle
	DetElement    detector;
	/// Handle to the closest Detector element
	DetElement    element;
	/// The transformation of space-points to the corrdinate system of the closests detector element
	TGeoHMatrix   toDetector;
	/// The transformation of space-points to the world corrdinate system 
	TGeoHMatrix   toWorld;
      public:
	/// Default constructor
	Context();
	/// Default destructor
	~Context();
      };
      /// Some useful Container abbreviations used by the VolumeManager
      typedef std::map<VolumeID,VolumeManager>   Managers;
      typedef std::map<DetElement,VolumeManager> Detectors;
      typedef std::map<VolIdentifier,Context*>   Volumes;

      /** @class VolumeManager::Object  VolumeManager.h DD4hep/lcdd/VolumeManager.h
       *
       * This structure describes the internal data of the volume manager object
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Object {
      public:
	typedef IDDescriptor::Field Field;
      public:
	/// The container of subdetector elements
	Detectors     subdetectors;
	/// The volume managers for the individual subdetector elements
	Managers      managers;
	/// The container of placements managed by this instance
	Volumes       volumes;
	/// The Detector element handle managed by this instance
	DetElement    detector;
	/// The ID descriptor object
        IDDescriptor  id;
	/// The reference to the TOP level VolumeManager
        TNamed*       top;
	/// The system field descriptor
	Field         system;
	/// System identifier
	VolumeID      sysID;
	/// Population flags
	int           flags;
      public:
	/// Default constructor
	Object();
	/// Default destructor
	~Object();
	/// Search the locally cached volumes for a matching ID
	Context* search(const VolIdentifier& id)  const;
      };

    protected:
      /// Additional data accessor
      Object& _data()   const            {  return *data<Object>();      }

    public:
      /// Default constructor
      VolumeManager() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      VolumeManager(const VolumeManager& e) : Ref_t(e) {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> VolumeManager(const Handle<Q>& e) : Ref_t(e) {}
      /** Initializing constructor. The tree will automatically be built if the detelement is valid
       *  Please see enum PopulateFlags for further info.
       *  No action whatsoever is performed here, if the detector element is not valid.
       */
      VolumeManager(const std::string& name, DetElement world=DetElement(), int flags=NONE);
      /// Add a new Volume manager section according to a new subdetector
      VolumeManager addSubdetector(DetElement detector);
      /// Access the volume manager by cell id
      VolumeManager subdetector(VolumeID id) const;

      /// Assign the top level detector element to this manager
      void setDetector(DetElement e);
      /// Access the top level detector element
      DetElement detector() const;
      /// Assign IDDescription to VolumeManager structure
      void setIDDescriptor(IDDescriptor spec)   const;
      /// Access IDDescription structure
      IDDescriptor idSpec() const;

      /// Register physical volume with the manager (normally: section manager)
      void adoptPlacement(Context* context);
      /// Register physical volume with the manager and pre-computed volume id
      void adoptPlacement(VolumeID volume_id, Context* context);

      /// Lookup the context, which belongs to a registered physical volume.
      Context*     lookupContext(VolumeID volume_id) const throw();
      /// Lookup a physical (placed) volume identified by its 64 bit hit ID
      PlacedVolume lookupPlacement(VolumeID volume_id) const;
      /// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
      DetElement   lookupDetector(VolumeID volume_id)  const;
      /// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
      DetElement   lookupDetElement(VolumeID volume_id)  const;
      /// Access the transformation of a physical volume to the world coordinate system
      const TGeoMatrix& worldTransformation(VolumeID volume_id)  const;
    };

    /// Enable printouts for debugging
    std::ostream& operator<<(std::ostream& os, const VolumeManager& m);

  }       /* End namespace Geometry               */
}         /* End namespace DD4hep                */
#endif    /* DD4hep_GEOMETRY_READOUT_H           */
