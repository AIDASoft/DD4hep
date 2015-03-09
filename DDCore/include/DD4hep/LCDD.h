// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_LCDD_LCDD_H
#define DD4HEP_LCDD_LCDD_H

// Framework includes
#include "DD4hep/Handle.h"
#include "DD4hep/Fields.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/NamedObject.h"

// C/C++ include files
#include <map>

// Forward declarations
class TGeoManager;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Foward declarations
  class NamedObject;

  /// Detector description build types.
  /** enum LCDDBuildType LCDD.h  DD4hep/LCDD.h
   * The corresponding flag is ONLY valid while parsing the
   * compact description. If no eometry, ie. at all other times
   * the accessor to the flag returns BUILD_NONE.
   */
  enum LCDDBuildType {
    BUILD_NONE = 0, BUILD_DEFAULT = 1, BUILD_SIMU = BUILD_DEFAULT, BUILD_RECO, BUILD_DISPLAY
  };

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// The main interface to the DD4hep detector description package
    /**
     *  Note: The usage of the factory method:
     *
     *      static LCDD& getInstance(void);
     *
     *  is DEPRECATED!
     *
     *  You should rather use the plugin mechanism to create a new instance.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class LCDD {
    public:
      /// Type definition of a map of named handles
      typedef std::map<std::string, Handle<NamedObject> > HandleMap;
      typedef std::map<std::string, std::string> PropertyValues;
      typedef std::map<std::string, PropertyValues> Properties;

      /// Destructor
      virtual ~LCDD() {
      }

      /// Access flag to steer the detail of building of the geometry/detector description
      virtual LCDDBuildType buildType() const = 0;
      /// Initialize geometry
      virtual void init() = 0;
      /// Finalize the geometry
      virtual void endDocument() = 0;

      /// Access the geometry manager of this instance
      virtual TGeoManager& manager() const = 0;
      /// Access to properties map
      virtual Properties& properties() const = 0;
      /// Return handle to material describing air
      virtual Material air() const = 0;
      /// Return handle to material describing vacuum
      virtual Material vacuum() const = 0;
      /// Return handle to "invisible" visualization attributes
      virtual VisAttr invisible() const = 0;

      /// Return reference to the top-most (world) detector element
      virtual DetElement world() const = 0;
      /// Return reference to detector element with all tracker devices.
      virtual DetElement trackers() const = 0;

      /// Return handle to the world volume containing everything
      virtual Volume worldVolume() const = 0;
      /// Return handle to the volume containing the tracking devices
      virtual Volume trackingVolume() const = 0;

      /// Return handle to the VolumeManager
      virtual VolumeManager volumeManager() const = 0;

      /// Accessor to the map of header entries
      virtual Header header() const = 0;
      /// Accessor to the header entry
      virtual void setHeader(Header h) = 0;

      /// Return handle to the combined electromagentic field description.
      virtual OverlayedField field() const = 0;

      /// Accessor to the map of constants
      virtual const HandleMap& constants() const = 0;
      /// Accessor to the map of region settings
      virtual const HandleMap& regions() const = 0;
      /// Accessor to the map of sub-detectors
      virtual const HandleMap& detectors() const = 0;
      /// Accessor to the map of sub-detectors
      virtual const HandleMap& sensitiveDetectors() const = 0;
      /// Accessor to the map of readout structures
      virtual const HandleMap& readouts() const = 0;
      /// Accessor to the map of visualisation attributes
      virtual const HandleMap& visAttributes() const = 0;
      /// Accessor to the map of limit settings
      virtual const HandleMap& limitsets() const = 0;
      /// Accessor to the map of aligment entries
      virtual const HandleMap& alignments() const = 0;
      /// Accessor to the map of field entries, which together form the global field
      virtual const HandleMap& fields() const = 0;
      /// Accessor to the map of ID specifications
      virtual const HandleMap& idSpecifications() const = 0;

      /// Register new mother volume using the detector name.
      /** Volumes must be registered/declared PRIOR to be picked up!
       *  The method throws an exception if another volume was already declared for this subdetector
       *  The method throws an exception if the volume to be registered is invalid.
       */
      virtual void   declareMotherVolume(const std::string& detector_name, const Volume& vol) = 0;
      /// Access mother volume by detector element
      /** The method uses the detector element's name for volume identification. 
       *  Unregistered detectors are hosted by the world volume.
       */
      virtual Volume pickMotherVolume(const DetElement& sd) const = 0;

      /// Typed access to constants: access string values
      virtual std::string constantAsString(const std::string& name) const = 0;
      /// Typed access to constants: long values
      virtual long constantAsLong(const std::string& name) const = 0;
      /// Typed access to constants: double values
      virtual double constantAsDouble(const std::string& name) const = 0;

      /// Retrieve a constant by it's name from the detector description
      virtual Constant constant(const std::string& name) const = 0;
      /// Typed access to constants: access any type values
      template <class T> T constant(const std::string& name) const;

      /// Retrieve a matrial by it's name from the detector description
      virtual Material material(const std::string& name) const = 0;
      /// Retrieve a id descriptor by it's name from the detector description
      virtual IDDescriptor idSpecification(const std::string& name) const = 0;
      /// Retrieve a region object by it's name from the detector description
      virtual Region region(const std::string& name) const = 0;
      /// Retrieve a visualization attribute by it's name from the detector description
      virtual VisAttr visAttributes(const std::string& name) const = 0;
      /// Retrieve a limitset by it's name from the detector description
      virtual LimitSet limitSet(const std::string& name) const = 0;
      /// Retrieve a readout object by it's name from the detector description
      virtual Readout readout(const std::string& name) const = 0;
      /// Retrieve an alignment entry by it's name from the detector description
      virtual AlignmentEntry alignment(const std::string& path) const = 0;
      /// Retrieve a sensitive detector by it's name from the detector description
      virtual SensitiveDetector sensitiveDetector(const std::string& name) const = 0;
      /// Retrieve a subdetector element by it's name from the detector description
      virtual CartesianField field(const std::string& name) const = 0;
      /// Retrieve a field component by it's name from the detector description
      virtual DetElement detector(const std::string& name) const = 0;

      /// Add a new constant to the detector description
      virtual LCDD& add(Constant constant) = 0;
      /// Add a new visualisation attribute to the detector description
      virtual LCDD& add(VisAttr attr) = 0;
      /// Add a new limit set to the detector description
      virtual LCDD& add(LimitSet limitset) = 0;
      /// Add a new detector region to the detector description
      virtual LCDD& add(Region region) = 0;
      /// Add a new id descriptor to the detector description
      virtual LCDD& add(IDDescriptor spec) = 0;
      /// Add a new detector readout to the detector description
      virtual LCDD& add(Readout readout) = 0;
      /// Add a new sensitive detector to the detector description
      virtual LCDD& add(SensitiveDetector entry) = 0;
      /// Add a new subdetector to the detector description
      virtual LCDD& add(DetElement detector) = 0;
      /// Add alignment entry to the detector description
      virtual LCDD& add(AlignmentEntry entry) = 0;
      /// Add a field component to the detector description
      virtual LCDD& add(CartesianField entry) = 0;

      /// Add a new constant by named reference to the detector description
      virtual LCDD& addConstant(const Ref_t& element) = 0;
      /// Add a new visualisation attribute by named reference to the detector description
      virtual LCDD& addVisAttribute(const Ref_t& element) = 0;
      /// Add a new limit set by named reference to the detector description
      virtual LCDD& addLimitSet(const Ref_t& limset) = 0;
      /// Add a new id descriptor by named reference to the detector description
      virtual LCDD& addIDSpecification(const Ref_t& element) = 0;
      /// Add a new detector region by named reference to the detector description
      virtual LCDD& addRegion(const Ref_t& region) = 0;
      /// Add a new detector readout by named reference to the detector description
      virtual LCDD& addReadout(const Ref_t& readout) = 0;
      /// Add a new sensitive detector by named reference to the detector description
      virtual LCDD& addSensitiveDetector(const Ref_t& element) = 0;
      /// Add a new subdetector by named reference to the detector description
      virtual LCDD& addDetector(const Ref_t& detector) = 0;
      /// Add alignment entry by named reference to the detector description
      virtual LCDD& addAlignment(const Ref_t& alignment) = 0;
      /// Add a field component by named reference to the detector description
      virtual LCDD& addField(const Ref_t& field) = 0;

      /// Read compact geometry description or alignment file
      virtual void fromCompact(const std::string& fname, LCDDBuildType type = BUILD_DEFAULT) = 0;
      /// Read any geometry description or alignment file
      virtual void fromXML(const std::string& fname, LCDDBuildType type = BUILD_DEFAULT) = 0;
      ///
      virtual void dump() const = 0;
      /// Manipulate geometry using facroy converter
      virtual void apply(const char* factory, int argc, char** argv) = 0;

      /// Extend the sensitive detector element with an arbitrary structure accessible by the type
      template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c) {
        return (IFACE*) addUserExtension(dynamic_cast<IFACE*>(c), typeid(IFACE), _delete<IFACE>);
      }

      /// Remove an existing extension object from the LCDD instance. If not destroyed, the instance is returned
      template <class T> T* removeExtension(bool destroy=true)  {
        return (T*) removeUserExtension(typeid(T),destroy);
      }

      /// Access extension element by the type
      template <class T> T* extension(bool alert=true) const {
        return (T*) userExtension(typeid(T),alert);
      }

      ///---Factory method-------
      static LCDD& getInstance(void);
      /// Destroy the instance
      static void destroyInstance();

    protected:
      /// Templated destructor function
      template <typename T> static void _delete(void* ptr) {
        delete (T*) (ptr);
      }
      /// Add an extension object to the detector element
      virtual void* addUserExtension(void* ptr, const std::type_info& info, void (*destruct)(void*)) = 0;
      /// Remove an existing extension object from the LCDD instance. If not destroyed, the instance is returned
      virtual void* removeUserExtension(const std::type_info& info, bool destroy) = 0;
      /// Access an existing extension object from the detector element
      virtual void* userExtension(const std::type_info& info, bool alert=true) const = 0;

    };

    /*
     *   The following are convenience implementations to access constants by type.
     *   I do not think this violates the interface approach, but it is so much
     *   more intuitiv to say constant<int>(name) than constantAsInt(name).
     */
#ifndef __CINT__
    /// Typed access to constants: short values
    template <> inline short LCDD::constant<short>(const std::string& name) const {
      return (short) constantAsLong(name);
    }

    /// Typed access to constants: unsigned short values
    template <> inline unsigned short LCDD::constant<unsigned short>(const std::string& name) const {
      return (unsigned short) constantAsLong(name);
    }

    /// Typed access to constants: integer values
    template <> inline int LCDD::constant<int>(const std::string& name) const {
      return (int) constantAsLong(name);
    }

    /// Typed access to constants: unsigned integer values
    template <> inline unsigned int LCDD::constant<unsigned int>(const std::string& name) const {
      return (unsigned int) constantAsLong(name);
    }

    /// Typed access to constants: long values
    template <> inline long LCDD::constant<long>(const std::string& name) const {
      return constantAsLong(name);
    }

    /// Typed access to constants: unsigned long values
    template <> inline unsigned long LCDD::constant<unsigned long>(const std::string& name) const {
      return (unsigned long) constantAsLong(name);
    }

    /// Typed access to constants: float values
    template <> inline float LCDD::constant<float>(const std::string& name) const {
      return (float) constantAsDouble(name);
    }

    /// Typed access to constants: double values
    template <> inline double LCDD::constant<double>(const std::string& name) const {
      return constantAsDouble(name);
    }

    /// Typed access to constants: string values
    template <> inline std::string LCDD::constant<std::string>(const std::string& name) const {
      return constantAsString(name);
    }
#endif
  } /* End namespace Geometry  */
} /* End namespace DD4hep   */
#endif    /* DD4HEP_LCDD_LCDD_H     */
