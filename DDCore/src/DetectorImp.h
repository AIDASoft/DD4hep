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

#ifndef DD4HEP_DetectorGEOIMP_H
#define DD4HEP_DetectorGEOIMP_H

// Framework include files
#include "DD4hep/DetectorData.h"
#include "DD4hep/DetectorLoad.h"

// Forward declarations
class TGeoManager;

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Concrete implementation class of the Detector interface
  /** @class DetectorImp   DetectorImp.h  src/DetectorImp.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class DetectorImp: public Detector, public DetectorData, public DetectorLoad  {
  protected:
    /// Cached map with detector types:
    typedef std::map<std::string, std::vector<DetElement> > DetectorTypeMap;

    /// Inventory of detector types
    DetectorTypeMap m_detectorTypes;

    /// VolumeManager m_volManager;
    DetectorBuildType m_buildType;

  private:
    /// Disable copy constructor
    DetectorImp(const DetectorImp& copy) = delete;

    /// Disable assignment operator
    DetectorImp& operator=(const DetectorImp& copy) = delete;

    /// Internal helper to map detector types once the geometry is closed
    void mapDetectorTypes();
  public:

    /// Local method (no interface): Load volume manager.
    void imp_loadVolumeManager();

    /// Default constructor
    DetectorImp();

    /// Standard destructor
    virtual ~DetectorImp();

    /// Access flag to steer the detail of building of the geometry/detector description
    virtual DetectorBuildType buildType() const {
      return m_buildType;
    }

    /// Read compact geometry description or alignment file
    virtual void fromCompact(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT) {
      fromXML(fname, type);
    }

    /// Read any XML file
    virtual void fromXML(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT);

    /// Read any geometry description or alignment file with external XML entity resolution
    virtual void fromXML(const std::string& fname,
                         xml::UriReader* entity_resolver,
                         DetectorBuildType type = BUILD_DEFAULT);

    virtual void dump() const;

    /// Manipulate geometry using facroy converter
    virtual long apply(const char* factory, int argc, char** argv);

    virtual void init();
    virtual void endDocument();

    /// Add an extension object to the Detector instance
    virtual void* addUserExtension(unsigned long long int key, ExtensionEntry* entry);

    /// Remove an existing extension object from the Detector instance. If not destroyed, the instance is returned
    virtual void* removeUserExtension(unsigned long long int key, bool destroy=true);

    /// Access an existing extension object from the Detector instance
    virtual void* userExtension(unsigned long long int key, bool alert=true) const;

    virtual Handle<NamedObject> getRefChild(const HandleMap& e, const std::string& name, bool throw_if_not = true) const;

    /// Register new mother volume using the detector name.
    virtual void   declareMotherVolume(const std::string& detector_name, const Volume& vol);

    /// Access mother volume by detector element
    virtual Volume pickMotherVolume(const DetElement& sd) const;

    /// Access the geometry manager of this instance
    virtual TGeoManager& manager() const {
      return *m_manager;
    }
    /// Access to properties
    Properties& properties() const {
      return *(Properties*)&m_properties;
    }
    /// Return handle to material describing air
    virtual Material air() const {
      return m_materialAir;
    }
    /// Return handle to material describing vacuum
    virtual Material vacuum() const {
      return m_materialVacuum;
    }
    /// Return handle to "invisible" visualization attributes
    virtual VisAttr invisible() const {
      return m_invisibleVis;
    }
    /// Return reference to the top-most (world) detector element
    virtual DetElement world() const {
      return m_world;
    }
    /// Return reference to detector element with all tracker devices.
    virtual DetElement trackers() const {
      return m_trackers;
    }
    /// Return handle to the world volume containing everything
    virtual Volume worldVolume() const {
      return m_worldVol;
    }
    /// Return handle to the world volume containing the volume with the tracking devices
    virtual Volume trackingVolume() const {
      return m_trackingVol;
    }
    /// Return handle to the VolumeManager
    virtual VolumeManager volumeManager() const {
      return m_volManager;
    }
    /// Return handle to the combined electromagentic field description.
    virtual OverlayedField field() const {
      return m_field;
    }
    /// Accessor to the header entry
    virtual Header header() const {
      return m_header;
    }
    /// Accessor to the header entry
    virtual void setHeader(Header h) {
      m_header = h;
    }

    /// Typed access to constants: access string values
    virtual std::string constantAsString(const std::string& name) const;

    /// Typed access to constants: long values
    virtual long constantAsLong(const std::string& name) const;

    /// Typed access to constants: double values
    virtual double constantAsDouble(const std::string& name) const;

    /// Retrieve a constant by it's name from the detector description
    virtual Constant constant(const std::string& name) const;

    /// Retrieve a limitset by it's name from the detector description
    virtual LimitSet limitSet(const std::string& name) const {
      return getRefChild(m_limits, name);
    }
    /// Retrieve a visualization attribute by it's name from the detector description
    virtual VisAttr visAttributes(const std::string& name) const {
      return getRefChild(m_display, name, false);
    }
    /// Retrieve a matrial by it's name from the detector description
    virtual Material material(const std::string& name) const;

    /// Retrieve a region object by it's name from the detector description
    virtual Region region(const std::string& name) const {
      return getRefChild(m_regions, name);
    }
    /// Retrieve a id descriptor by it's name from the detector description
    virtual IDDescriptor idSpecification(const std::string& name) const {
      return getRefChild(m_idDict, name);
    }
    /// Retrieve a readout object by it's name from the detector description
    virtual Readout readout(const std::string& name) const {
      return getRefChild(m_readouts, name);
    }
    /// Retrieve a subdetector element by it's name from the detector description
    virtual DetElement detector(const std::string& name) const {
      return getRefChild(m_detectors, name);
    }
    /// Retrieve a sensitive detector by it's name from the detector description
    virtual SensitiveDetector sensitiveDetector(const std::string& name) const {
      return getRefChild(m_sensitive, name, false);
    }
    /// Retrieve a subdetector element by it's name from the detector description
    virtual CartesianField field(const std::string& name) const {
      return getRefChild(m_fields, name, false);
    }

    /// Accessor to the map of constants
    virtual const HandleMap& constants() const {
      return m_define;
    }
    /// Accessor to the map of visualisation attributes
    virtual const HandleMap& visAttributes() const {
      return m_display;
    }
    /// Accessor to the map of limit settings
    virtual const HandleMap& limitsets() const {
      return m_limits;
    }
    /// Accessor to the map of region settings
    virtual const HandleMap& regions() const {
      return m_regions;
    }
    /// Accessor to the map of readout structures
    virtual const HandleMap& readouts() const {
      return m_readouts;
    }
    /// Accessor to the map of sub-detectors
    virtual const HandleMap& detectors() const {
      return m_detectors;
    }
    /// Retrieve a sensitive detector by it's name from the detector description
    virtual const HandleMap& sensitiveDetectors() const {
      return m_sensitive;
    }
    /// Accessor to the map of field entries, which together form the global field
    virtual const HandleMap& fields() const {
      return m_fields;
    }
    /// Accessor to the map of ID specifications
    virtual const HandleMap& idSpecifications() const {
      return m_idDict;
    }

    /// Access a set of subdetectors according to the sensitive type.
    /**
       Please note:
       - The sensitive type of a detector is set in the 'detector constructor'.
       - Not sensitive detector structures have the name 'passive'
       - Compounds (ie. nested detectors) are of type 'compound'
       - If throw_exc is set to true, an exception is thrown if the type
       is not present. Otherwise an empty detector container is returned.
    */
    virtual const std::vector<DetElement>& detectors(const std::string& type, bool throw_exc);

    /// Access a set of subdetectors according to several sensitive types.
    virtual std::vector<DetElement> detectors(const std::string& type1,
                                              const std::string& type2,
                                              const std::string& type3="",
                                              const std::string& type4="",
                                              const std::string& type5="" );

    /// Access the availible detector types
    virtual std::vector<std::string> detectorTypes() const;

    /** return a vector with all detectors that have all the type properties in
     *  includeFlag set but none of the properties given in excludeFlag
     */
    virtual std::vector<DetElement> detectors(unsigned int includeFlag, 
                                              unsigned int excludeFlag=0 ) const ;


#define __R  return *this
    /// Add a new constant to the detector description
    virtual Detector& add(Constant x) {
      return addConstant(x);
    }
    /// Add a new limit set to the detector description
    virtual Detector& add(LimitSet x) {
      return addLimitSet(x);
    }
    /// Add a new detector region to the detector description
    virtual Detector& add(Region x) {
      return addRegion(x);
    }
    /// Add a new visualisation attribute to the detector description
    virtual Detector& add(VisAttr x) {
      return addVisAttribute(x);
    }
    /// Add a new id descriptor to the detector description
    virtual Detector& add(IDDescriptor x) {
      return addIDSpecification(x);
    }
    /// Add a new detector readout to the detector description
    virtual Detector& add(Readout x) {
      return addReadout(x);
    }
    /// Add a new sensitive detector to the detector description
    virtual Detector& add(SensitiveDetector x) {
      return addSensitiveDetector(x);
    }
    /// Add a new subdetector to the detector description
    virtual Detector& add(DetElement x) {
      return addDetector(x);
    }
    /// Add a field component to the detector description
    virtual Detector& add(CartesianField x) {
      return addField(x);
    }

    /// Add a new constant by named reference to the detector description
    virtual Detector& addConstant(const Handle<NamedObject>& x);

    /// Add a new limit set by named reference to the detector description
    virtual Detector& addLimitSet(const Handle<NamedObject>& x) {
      m_limits.append(x);
      __R;
    }
    /// Add a new detector region by named reference to the detector description
    virtual Detector& addRegion(const Handle<NamedObject>& x) {
      m_regions.append(x);
      __R;
    }
    /// Add a new id descriptor by named reference to the detector description
    virtual Detector& addIDSpecification(const Handle<NamedObject>& x) {
      m_idDict.append(x);
      __R;
    }
    /// Add a new detector readout by named reference to the detector description
    virtual Detector& addReadout(const Handle<NamedObject>& x) {
      m_readouts.append(x);
      __R;
    }
    /// Add a new visualisation attribute by named reference to the detector description
    virtual Detector& addVisAttribute(const Handle<NamedObject>& x) {
      m_display.append(x);
      __R;
    }
    /// Add a new sensitive detector by named reference to the detector description
    virtual Detector& addSensitiveDetector(const Handle<NamedObject>& x) {
      m_sensitive.append(x);
      __R;
    }
    /// Add a new subdetector by named reference to the detector description
    virtual Detector& addDetector(const Handle<NamedObject>& x);
    /// Add a field component by named reference to the detector description
    virtual Detector& addField(const Handle<NamedObject>& x);
#undef __R

  };
} /* End namespace dd4hep   */

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link C++ class dd4hep::DetectorImp+;
#endif
#endif    /* dd4hep_DetectorGEOIMP_H    */
