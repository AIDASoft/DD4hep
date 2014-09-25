// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_VOLUMES_H
#define DD4HEP_GEOMETRY_VOLUMES_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Objects.h"

// C/C++ include files
#include <map>

// ROOT include file (includes TGeoVolume + TGeoShape)
#include "TGeoNode.h"
#include "TGeoPatternFinder.h"

#if ROOT_VERSION_CODE > ROOT_VERSION(5,34,9)
// Recent ROOT versions
#include "TGeoExtension.h"

#else
// Older ROOT version
#define DD4HEP_EMULATE_TGEOEXTENSIONS
class TGeoExtension : public TObject  {
 public:  
  virtual ~TGeoExtension() {}
  /// TGeoExtension overload: Method called whenever requiring a pointer to the extension
  virtual TGeoExtension *Grab() = 0;
  /// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
  virtual void Release() const = 0;
};
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class  LCDD;
    class  Region;
    class  LimitSet;
    class  Material;
    class  VisAttr;
    class  Volume;
    class  DetElement;
    class  PlacedVolume;
    class  SensitiveDetector;

    /// Implementation class extending the ROOT placed volume
    /** 
     *  For any further documentation please see the following ROOT documentation:
     *  @see http://root.cern.ch/root/html/TGeoExtension.html 
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class PlacedVolumeExtension : public TGeoExtension  {
    public:
      typedef std::pair<std::string, int> VolID;
      /// Volume ID container
      class VolIDs: public std::vector<VolID> {
      public:
        typedef std::vector<VolID> Base;
        VolIDs() : std::vector<VolID>() {
        }
        ~VolIDs() {
        }
        std::vector<VolID>::const_iterator find(const std::string& name) const;
        std::pair<std::vector<VolID>::iterator, bool> insert(const std::string& name, int value);
      };
      /// Magic word to detect memory corruptions
      unsigned long magic;
      /// Reference count on object (used to implement Grab/Release)
      long refCount;
      /// ID container
      VolIDs volIDs;
      /// Default constructor
      PlacedVolumeExtension();
      /// Copy constructor
      PlacedVolumeExtension(const PlacedVolumeExtension& c);
      /// Default destructor
      virtual ~PlacedVolumeExtension();
      /// Assignment operator
      PlacedVolumeExtension& operator=(const PlacedVolumeExtension& c) {
	magic = c.magic;
	volIDs = c.volIDs;
	return *this;
      }
      /// TGeoExtension overload: Method called whenever requiring a pointer to the extension
      virtual TGeoExtension *Grab();
      /// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
      virtual void Release() const;
      /// Enable ROOT persistency
      ClassDef(PlacedVolumeExtension,1);
    };

    /// Handle class holding a placed volume (also called physical volume)
    /** 
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoNode.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    class PlacedVolume : public Handle<TGeoNode> {
    public:
      typedef PlacedVolumeExtension Object;
      typedef Object::VolIDs VolIDs;
      typedef Object::VolID  VolID;

      /// Constructor to be used when reading the already parsed DOM tree
      PlacedVolume(const TGeoNode* e)
          : Handle<TGeoNode>(e) {
      }
      /// Default constructor
      PlacedVolume()
          : Handle<TGeoNode>() {
      }
      /// Copy assignment
      PlacedVolume(const PlacedVolume& e)
          : Handle<TGeoNode>(e) {
      }
      /// Copy assignment from other handle type
      template <typename T> PlacedVolume(const Handle<T>& e)
          : Handle<TGeoNode>(e) {
      }
      /// Assignment operator (must match copy constructor)
      PlacedVolume& operator=(const PlacedVolume& v) {
        m_element = v.m_element;
        return *this;
      }
      /// Check if placement is properly instrumented
      Object* data() const;
      /// Add identifier
      PlacedVolume& addPhysVolID(const std::string& name, int value);
      /// Volume material
      Material material() const;
      /// Logical volume of this placement
      Volume volume() const;
      /// Parent volume (envelope)
      Volume motherVol() const;
      /// Access to the volume IDs
      const VolIDs& volIDs() const;
      /// String dump
      std::string toString() const;
    };

    /// Implementation class extending the ROOT volume (TGeoVolume)
    /**
     *  Internal data structure optional to TGeo data.
     *
     *  For any further documentation please see the following ROOT documentation:
     *  @see http://root.cern.ch/root/html/TGeoExtension.html 
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class VolumeExtension : public TGeoExtension {
    public:
      /// Magic word to detect memory corruptions
      unsigned long magic;
      /// Reference count on object (used to implement Grab/Release)
      long          refCount;
      Region region;
      LimitSet limits;
      VisAttr vis;
      Ref_t sens_det;
      int referenced;
      /// Default constructor
      VolumeExtension();
      /// Default destructor
      virtual ~VolumeExtension();
      /// Copy the object
      void copy(const VolumeExtension& c) {
	magic = c.magic;
	region = c.region;
	limits = c.limits;
	vis = c.vis;
	sens_det = c.sens_det;
	referenced = c.referenced;
      }
      /// TGeoExtension overload: Method called whenever requiring a pointer to the extension
      virtual TGeoExtension *Grab();
      /// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
      virtual void Release() const;
      /// Enable ROOT persistency
      ClassDef(VolumeExtension,1);
    };

    /// Handle class holding a placed volume (also called physical volume)
    /**
     *   Handle describing a Volume
     *
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoVolume.html 
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    class Volume: public Handle<TGeoVolume> {

    public:
      typedef Handle<TGeoVolume> Base;
      typedef VolumeExtension Object;

    public:
      /// Default constructor
      Volume()
          : Base(0) {
      }

      /// Copy from handle
      Volume(const TGeoVolume* v)
          : Base(v) {
      }

      /// Copy from handle
      Volume(const Volume& v)
          : Base(v) {
      }

      /// Copy from arbitrary Element
      template <typename T> Volume(const Handle<T>& v)
          : Base(v) {
      }

      /// Constructor to be used when creating a new geometry tree.
      Volume(const std::string& name);

      /// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
      Volume(const std::string& name, const Solid& s, const Material& m);

      /// Assignment operator (must match copy constructor)
      Volume& operator=(const Volume& a) {
        m_element = a.m_element;
        return *this;
      }

      /// Check if placement is properly instrumented
      Object* data() const;
      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol) const;
      /// Place daughter volume according to a generic Transform3D
      PlacedVolume placeVolume(const Volume& volume, const Transform3D& tr) const;
      /// Place un-rotated daughter volume at the given position.
      PlacedVolume placeVolume(const Volume& vol, const Position& pos) const;
      /// Place rotated daughter volume. The position is automatically the identity position
      PlacedVolume placeVolume(const Volume& vol, const RotationZYX& rot) const;
      /// Place rotated daughter volume. The position is automatically the identity position
      PlacedVolume placeVolume(const Volume& vol, const Rotation3D& rot) const;

      /// Attach attributes to the volume
      const Volume& setAttributes(const LCDD& lcdd, const std::string& region, const std::string& limits,
          const std::string& vis) const;

      /// Set the regional attributes to the volume. Note: If the name string is empty, the action is ignored.
      const Volume& setRegion(const LCDD& lcdd, const std::string& name) const;
      /// Set the regional attributes to the volume
      const Volume& setRegion(const Region& obj) const;
      /// Access to the handle to the region structure
      Region region() const;

      /// Set the limits to the volume. Note: If the name string is empty, the action is ignored.
      const Volume& setLimitSet(const LCDD& lcdd, const std::string& name) const;
      /// Set the limits to the volume
      const Volume& setLimitSet(const LimitSet& obj) const;
      /// Access to the limit set
      LimitSet limitSet() const;

      /// Set Visualization attributes to the volume
      const Volume& setVisAttributes(const VisAttr& obj) const;
      /// Set Visualization attributes to the volume. Note: If the name string is empty, the action is ignored.
      const Volume& setVisAttributes(const LCDD& lcdd, const std::string& name) const;
      /// Access the visualisation attributes
      VisAttr visAttributes() const;

      /// Assign the sensitive detector structure
      const Volume& setSensitiveDetector(const SensitiveDetector& obj) const;
      /// Access to the handle to the sensitive detector
      Ref_t sensitiveDetector() const;
      /// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
      bool isSensitive() const;

      /// Set the volume's solid shape
      const Volume& setSolid(const Solid& s) const;
      /// Access to Solid (Shape)
      Solid solid() const;

      /// Set the volume's material
      const Volume& setMaterial(const Material& m) const;
      /// Access to the Volume material
      Material material() const;

      /// Auto conversion to underlying ROOT object
      operator TGeoVolume*() const {
        return m_element;
      }
    };

    /// Implementation class extending the ROOT assembly volumes (TGeoVolumeAsembly)
    /** 
     *  Handle describing a volume assembly. 
     *
     *   For any further documentation please see the following ROOT documentation:
     *   @see http://root.cern.ch/root/html/TGeoVolumeAssembly.html
     *
     *   @author  M.Frank
     *   @version 1.0
     */
    class Assembly: public Volume {
    public:
      /// Default constructor
      Assembly()
          : Volume() {
      }

      /// Copy from handle
      Assembly(const Assembly& v)
          : Volume(v) {
      }

      /// Copy from arbitrary Element
      template <typename T> Assembly(const Handle<T>& v)
          : Volume(v) {
      }

      /// Constructor to be used when creating a new geometry tree.
      Assembly(const std::string& name);

      /// Assignment operator (must match copy constructor)
      Assembly& operator=(const Assembly& a) {
        m_element = a.m_element;
        return *this;
      }
    };

  } /* End namespace Geometry          */
}   /* End namespace DD4hep            */
#endif    /* DD4HEP_GEOMETRY_VOLUMES_H       */
