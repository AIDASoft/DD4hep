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
#ifndef DD4HEP_DETECTOR_DETECTOR_H
#define DD4HEP_DETECTOR_DETECTOR_H

#include "DD4hep/Version.h"

// Framework includes
#include "DD4hep/Handle.h"
#include "DD4hep/Fields.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Readout.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/VolumeManager.h"
#include "DD4hep/ExtensionEntry.h"
#include "DD4hep/BuildType.h"

// C/C++ include files
#include <map>
#include <vector>
#include <string>
#include <cstdio>

// Forward declarations
class TGeoManager;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// return a string with the current dd4hep version in the form vXX-YY.
  std::string versionString();
  
  // Foward declarations
  class NamedObject;

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {
    class UriReader;
  }

  /// The main interface to the dd4hep detector description package
  /**
   *  Note: The usage of the factory method:
   *
   *      static Detector& getInstance(void);
   *
   *  is DEPRECATED!
   *
   *  You should rather use the plugin mechanism to create a new instance.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class Detector {
  public:
    /// Type definition of a map of named handles
    typedef std::map<std::string, Handle<NamedObject> > HandleMap;
    typedef std::map<std::string, std::string>          PropertyValues;
    typedef std::map<std::string, PropertyValues>       Properties;

    /// Destructor
    virtual ~Detector() = default;

    /// Access flag to steer the detail of building of the geometry/detector description
    virtual DetectorBuildType buildType() const = 0;
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
    /// Accessor to the map of field entries, which together form the global field
    virtual const HandleMap& fields() const = 0;
    /// Accessor to the map of ID specifications
    virtual const HandleMap& idSpecifications() const = 0;

#ifndef __MAKECINT__
    /** Access to predefined caches of subdetectors according to the sensitive type */
    /// Access a set of subdetectors according to the sensitive type.
    /**
       Please note:
       - The sensitive type of a detector is set in the 'detector constructor'.
       - Not sensitive detector structures have the name 'passive'
       - Compounds (ie. nested detectors) are of type 'compound'
       - If throw_exc is set to true, an exception is thrown if the type
       is not present. Otherwise an empty detector container is returned.
    */
    virtual const std::vector<DetElement>& detectors(const std::string& type,
                                                     bool throw_exc=false) = 0;

    /// Access a set of subdetectors according to several sensitive types.
    virtual std::vector<DetElement> detectors(const std::string& type1,
                                              const std::string& type2,
                                              const std::string& type3="",
                                              const std::string& type4="",
                                              const std::string& type5="" ) = 0;

    /// Access the availible detector types
    virtual std::vector<std::string> detectorTypes() const = 0;


    /** return a vector with all detectors that have all the type properties in
     *  includeFlag set but none of the properties given in excludeFlag
     */
    virtual std::vector<DetElement> detectors(unsigned int includeFlag, 
                                              unsigned int excludeFlag=0 ) const = 0 ;
#endif

    /** Miscaneleous accessors to the detexctor description  */

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
    /// Retrieve a sensitive detector by it's name from the detector description
    virtual SensitiveDetector sensitiveDetector(const std::string& name) const = 0;
    /// Retrieve a subdetector element by it's name from the detector description
    virtual CartesianField field(const std::string& name) const = 0;
    /// Retrieve a field component by it's name from the detector description
    virtual DetElement detector(const std::string& name) const = 0;

    /// Add a new constant to the detector description
    virtual Detector& add(Constant constant) = 0;
    /// Add a new visualisation attribute to the detector description
    virtual Detector& add(VisAttr attr) = 0;
    /// Add a new limit set to the detector description
    virtual Detector& add(LimitSet limitset) = 0;
    /// Add a new detector region to the detector description
    virtual Detector& add(Region region) = 0;
    /// Add a new id descriptor to the detector description
    virtual Detector& add(IDDescriptor spec) = 0;
    /// Add a new detector readout to the detector description
    virtual Detector& add(Readout readout) = 0;
    /// Add a new sensitive detector to the detector description
    virtual Detector& add(SensitiveDetector entry) = 0;
    /// Add a new subdetector to the detector description
    virtual Detector& add(DetElement detector) = 0;
    /// Add a field component to the detector description
    virtual Detector& add(CartesianField entry) = 0;

    /// Add a new constant by named reference to the detector description
    virtual Detector& addConstant(const Handle<NamedObject>& element) = 0;
    /// Add a new visualisation attribute by named reference to the detector description
    virtual Detector& addVisAttribute(const Handle<NamedObject>& element) = 0;
    /// Add a new limit set by named reference to the detector description
    virtual Detector& addLimitSet(const Handle<NamedObject>& limset) = 0;
    /// Add a new id descriptor by named reference to the detector description
    virtual Detector& addIDSpecification(const Handle<NamedObject>& element) = 0;
    /// Add a new detector region by named reference to the detector description
    virtual Detector& addRegion(const Handle<NamedObject>& region) = 0;
    /// Add a new detector readout by named reference to the detector description
    virtual Detector& addReadout(const Handle<NamedObject>& readout) = 0;
    /// Add a new sensitive detector by named reference to the detector description
    virtual Detector& addSensitiveDetector(const Handle<NamedObject>& element) = 0;
    /// Add a new subdetector by named reference to the detector description
    virtual Detector& addDetector(const Handle<NamedObject>& detector) = 0;
    /// Add a field component by named reference to the detector description
    virtual Detector& addField(const Handle<NamedObject>& field) = 0;

    /// Deprecated call (use fromXML): Read compact geometry description or alignment file
    virtual void fromCompact(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT) = 0;
    /// Read any geometry description or alignment file
    virtual void fromXML(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT) = 0;
    /// Read any geometry description or alignment file with external XML entity resolution
    virtual void fromXML(const std::string& fname,
                         xml::UriReader* entity_resolver,
                         DetectorBuildType type = BUILD_DEFAULT) = 0;

    /// Stupid legacy method
    virtual void dump() const = 0;
    /// Manipulate geometry using factory converter
    virtual long apply(const char* factory, int argc, char** argv) = 0;

    /// Add an extension object to the detector element (low level member function)
    virtual void* addUserExtension(unsigned long long int key, ExtensionEntry* entry) = 0;

    /// Remove an existing extension object from the Detector instance.
    /** If not destroyed, the instance is returned  (low level member function) */
    virtual void* removeUserExtension(unsigned long long int key, bool destroy) = 0;

    /// Access an existing extension object from the detector element (low level member function)
    virtual void* userExtension(unsigned long long int key, bool alert=true) const = 0;

    /// Extend the sensitive detector element with an arbitrary structure accessible by the type
    template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c)  {
      return (IFACE*) addUserExtension(detail::typeHash64<IFACE>(),
                                       new detail::DeleteExtension<IFACE,CONCRETE>(c));
    }

    /// Remove an existing extension object from the Detector instance. If not destroyed, the instance is returned
    template <class IFACE> IFACE* removeExtension(bool destroy=true)  {
      return (IFACE*) removeUserExtension(detail::typeHash64<IFACE>(),destroy);
    }

    /// Access extension element by the type
    template <class IFACE> IFACE* extension(bool alert=true) const {
      return (IFACE*) userExtension(detail::typeHash64<IFACE>(),alert);
    }

    ///---Factory method-------
    static Detector& getInstance(void);
    /// Destroy the instance
    static void destroyInstance();

  };

  /*
   *   The following are convenience implementations to access constants by type.
   *   I do not think this violates the interface approach, but it is so much
   *   more intuitiv to say constant<int>(name) than constantAsInt(name).
   */
#ifndef __CINT__
  /// Typed access to constants: short values
  template <> inline short Detector::constant<short>(const std::string& name) const {
    return (short) constantAsLong(name);
  }

  /// Typed access to constants: unsigned short values
  template <> inline unsigned short Detector::constant<unsigned short>(const std::string& name) const {
    return (unsigned short) constantAsLong(name);
  }

  /// Typed access to constants: integer values
  template <> inline int Detector::constant<int>(const std::string& name) const {
    return (int) constantAsLong(name);
  }

  /// Typed access to constants: unsigned integer values
  template <> inline unsigned int Detector::constant<unsigned int>(const std::string& name) const {
    return (unsigned int) constantAsLong(name);
  }

  /// Typed access to constants: long values
  template <> inline long Detector::constant<long>(const std::string& name) const {
    return constantAsLong(name);
  }

  /// Typed access to constants: unsigned long values
  template <> inline unsigned long Detector::constant<unsigned long>(const std::string& name) const {
    return (unsigned long) constantAsLong(name);
  }

  /// Typed access to constants: float values
  template <> inline float Detector::constant<float>(const std::string& name) const {
    return (float) constantAsDouble(name);
  }

  /// Typed access to constants: double values
  template <> inline double Detector::constant<double>(const std::string& name) const {
    return constantAsDouble(name);
  }

  /// Typed access to constants: string values
  template <> inline std::string Detector::constant<std::string>(const std::string& name) const {
    return constantAsString(name);
  }
#endif
}         /* End namespace dd4hep           */
#endif    /* DD4HEP_DETECTOR_DETECTOR_H     */
