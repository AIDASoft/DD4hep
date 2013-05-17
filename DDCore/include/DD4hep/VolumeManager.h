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
      * @author  M.Frank
      * @version 1.0
      */
    struct VolumeManager : public Ref_t {
      typedef unsigned long long int VolumeID;

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
       * @author  M.Frank
       * @version 1.0
       */
      struct Context   {
	VolumeID      identifier;
	VolumeID      mask;
	PlacedVolume  placement;
	DetElement    detector, element;
	TGeoHMatrix   toDetector;
	TGeoHMatrix   toWorld;
	/// Default constructor
	Context();
	/// Default destructor
	~Context();
      };
      typedef std::map<VolumeID,VolumeManager>   Managers;
      typedef std::map<DetElement,VolumeManager> Detectors;
      typedef std::map<VolIdentifier,Context*>   Volumes;

      /** @class VolumeManager::Object  VolumeManager.h DD4hep/lcdd/VolumeManager.h
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct Object {
	Detectors    subdetectors;
	Managers     managers;
	Volumes      volumes;
	DetElement   detector;
        IDDescriptor id;
	VolumeID     sysID;
	int          top;
	IDDescriptor::Field system;
	/// Default constructor
	Object();
	/// Default destructor
	~Object();
	/// Search the locally cached volumes for a matching ID
	Context* search(const VolIdentifier& id)  const;
      };

    protected:
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }

    public:
      /// Default constructor
      VolumeManager() : Ref_t() {}
      /// Constructor to be used when reading the already parsed object
      VolumeManager(const VolumeManager& e) : Ref_t(e) {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> VolumeManager(const Handle<Q>& e) : Ref_t(e) {}
      /// Initializing constructor. The tree will automatically be built if the detelement is valid
      VolumeManager(const std::string& name, DetElement world=DetElement());
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

  }       /* End namespace Geometry               */
}         /* End namespace DD4hep                */
#endif    /* DD4hep_GEOMETRY_READOUT_H           */
