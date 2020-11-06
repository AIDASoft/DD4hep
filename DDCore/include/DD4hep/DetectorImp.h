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
#ifndef DD4HEP_DETECTORIMP_H
#define DD4HEP_DETECTORIMP_H

//==========================================================================
//
// Please note:
//
// This header file is purely DD4hep internal. It should never be used
// independently outside DetectorImp.cpp and certain DD4hep specific plugins.
// This header files is only publicly present to satisfy the ROOT interpreter
// when loading precompiled dictionary libraries.
//
//==========================================================================
#if !defined(DD4HEP_MUST_USE_DETECTORIMP_H) && !defined(G__ROOT)
#error "DetectorImp.h is a dd4hep internal header. Never use it in a depending compilation unit!"
#endif

// Framework include files
#include "DD4hep/DetectorData.h"
#include "DD4hep/DetectorLoad.h"
#include "TNamed.h"

// Forward declarations
class TGeoManager;

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail  {
    class OpticalSurfaceManagerObject;
  }
  
  /// Concrete implementation class of the Detector interface
  /** The main entry point to the DD4hep detector description
   *
   *  Please note:
   *  The inheritance of the TNamed is necessary to properly access the 
   *  object when loaded from ROOT.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class DetectorImp: public TNamed, public Detector, public DetectorData, public DetectorLoad  {
  protected:
    /// Cached map with detector types:
    typedef std::map<std::string, std::vector<DetElement> > DetectorTypeMap;

    /// Standard conditions
    mutable STD_Conditions m_std_conditions;
    
    /// Inventory of detector types
    DetectorTypeMap   m_detectorTypes;

    /// VolumeManager m_volManager;
    DetectorBuildType m_buildType;

    /// Optical surface manager
    detail::OpticalSurfaceManagerObject* m_surfaceManager = 0;   //! not persistent

  private:
    /// Disable move constructor
    DetectorImp(DetectorImp&& copy) = delete;

    /// Disable copy constructor
    DetectorImp(const DetectorImp& copy) = delete;

    /// Disable assignment operator
    DetectorImp& operator=(const DetectorImp& copy) = delete;

    /// Internal helper to map detector types once the geometry is closed
    void mapDetectorTypes();

    /// ROOT I/O call
    Int_t saveObject(const char *name=0, Int_t option=0, Int_t bufsize=0) const;
  public:

    /// Local method (no interface): Load volume manager.
    void imp_loadVolumeManager();
    
    /// Default constructor used by ROOT I/O
    DetectorImp();

    /// Initializing constructor
    DetectorImp(const std::string& name);

    /// Standard destructor
    virtual ~DetectorImp();

    /// Access the state of the geometry
    virtual State state()  const  override   {
      return m_state;
    }

    /// Access flag to steer the detail of building of the geometry/detector description
    virtual DetectorBuildType buildType() const   override {
      return m_buildType;
    }

    /// Read compact geometry description or alignment file
    virtual void fromCompact(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT)   override {
      fromXML(fname, type);
    }

    /// Read any XML file
    virtual void fromXML(const std::string& fname, DetectorBuildType type = BUILD_DEFAULT)  override;

    /// Read any geometry description or alignment file with external XML entity resolution
    virtual void fromXML(const std::string& fname,
                         xml::UriReader* entity_resolver,
                         DetectorBuildType type = BUILD_DEFAULT)  override;

    virtual void dump() const  override;

    /// Manipulate geometry using facroy converter
    virtual long apply(const char* factory, int argc, char** argv)  const  override;

    /// Open the geometry at startup.
    virtual void init()  override;

    /// Close the geometry
    virtual void endDocument(bool close_geometry)  override;

    /// Add an extension object to the Detector instance
    virtual void* addUserExtension(unsigned long long int key, ExtensionEntry* entry)  override;

    /// Remove an existing extension object from the Detector instance. If not destroyed, the instance is returned
    virtual void* removeUserExtension(unsigned long long int key, bool destroy=true)  override;

    /// Access an existing extension object from the Detector instance
    virtual void* userExtension(unsigned long long int key, bool alert=true) const  override;

    virtual Handle<NamedObject> getRefChild(const HandleMap& e, const std::string& name, bool throw_if_not = true) const;

    /// Register new mother volume using the detector name.
    virtual void   declareParent(const std::string& detector_name, const DetElement& parent)  override;

    /// Access mother volume by detector element
    virtual Volume pickMotherVolume(const DetElement& sd) const  override;

    /// Access the geometry manager of this instance
    virtual TGeoManager& manager() const   override  {
      return *m_manager;
    }
    /// Access to properties
    Properties& properties() const  override {
      return *(Properties*)&m_properties;
    }
    /// Return handle to material describing air
    virtual Material air() const  override {
      return m_materialAir;
    }
    /// Return handle to material describing vacuum
    virtual Material vacuum() const  override {
      return m_materialVacuum;
    }
    /// Return handle to "invisible" visualization attributes
    virtual VisAttr invisible() const  override {
      return m_invisibleVis;
    }
    /// Return reference to the top-most (world) detector element
    virtual DetElement world() const  override {
      return m_world;
    }
    /// Return reference to detector element with all tracker devices.
    virtual DetElement trackers() const  override {
      return m_trackers;
    }
    /// Return handle to the world volume containing everything
    virtual Volume worldVolume() const  override {
      return m_worldVol;
    }
    /// Return handle to the world volume containing the volume with the tracking devices
    virtual Volume parallelWorldVolume() const override  {
      return m_parallelWorldVol;
    }
    /// Return handle to the world volume containing the volume with the tracking devices
    virtual Volume trackingVolume() const  override {
      return m_trackingVol;
    }
    /// Set the tracking volume of the detector
    virtual void setTrackingVolume(Volume vol)  override {
      m_trackingVol = vol;
    }
    /// Return handle to the VolumeManager
    virtual VolumeManager volumeManager() const  override {
      return m_volManager;
    }
    /// Access the optical surface manager
    virtual OpticalSurfaceManager surfaceManager()  const  override  {
      return OpticalSurfaceManager(m_surfaceManager);
    }

    /// Return handle to the combined electromagentic field description.
    virtual OverlayedField field() const  override {
      return m_field;
    }

    /// Access default conditions (temperature and pressure
    virtual const STD_Conditions& stdConditions()  const  override;
    /// Set the STD temperature and pressure
    virtual void setStdConditions(double temp, double pressure) override;
    /// Set the STD conditions according to defined types (STP or NTP)
    virtual void setStdConditions(const std::string& type)  override;

    /// Accessor to the header entry
    virtual Header header() const  override {
      return m_header;
    }
    /// Accessor to the header entry
    virtual void setHeader(Header h)  override {
      m_header = h;
    }

    /// Typed access to constants: access string values
    virtual std::string constantAsString(const std::string& name) const  override;

    /// Typed access to constants: long values
    virtual long constantAsLong(const std::string& name) const  override;

    /// Typed access to constants: double values
    virtual double constantAsDouble(const std::string& name) const  override;

    /// Retrieve a constant by it's name from the detector description
    virtual Constant constant(const std::string& name) const  override;

    /// Retrieve a limitset by it's name from the detector description
    virtual LimitSet limitSet(const std::string& name) const  override {
      return getRefChild(m_limits, name);
    }
    /// Retrieve a visualization attribute by it's name from the detector description
    virtual VisAttr visAttributes(const std::string& name) const  override {
      return getRefChild(m_display, name, false);
    }
    /// Retrieve a matrial by it's name from the detector description
    virtual Material material(const std::string& name) const  override;

    /// Retrieve a region object by it's name from the detector description
    virtual Region region(const std::string& name) const  override {
      return getRefChild(m_regions, name);
    }
    /// Retrieve a id descriptor by it's name from the detector description
    virtual IDDescriptor idSpecification(const std::string& name) const  override {
      return getRefChild(m_idDict, name);
    }
    /// Retrieve a readout object by it's name from the detector description
    virtual Readout readout(const std::string& name) const  override {
      return getRefChild(m_readouts, name);
    }
    /// Retrieve a subdetector element by it's name from the detector description
    virtual DetElement detector(const std::string& name) const  override;
    //{      return getRefChild(m_detectors, name);    }
    /// Retrieve a sensitive detector by it's name from the detector description
    virtual SensitiveDetector sensitiveDetector(const std::string& name) const  override {
      return getRefChild(m_sensitive, name, false);
    }
    /// Retrieve a subdetector element by it's name from the detector description
    virtual CartesianField field(const std::string& name) const  override {
      return getRefChild(m_fields, name, false);
    }

    /// Accessor to the map of constants
    virtual const HandleMap& constants() const  override {
      return m_define;
    }
    /// Accessor to the map of visualisation attributes
    virtual const HandleMap& visAttributes() const  override {
      return m_display;
    }
    /// Accessor to the map of limit settings
    virtual const HandleMap& limitsets() const  override {
      return m_limits;
    }
    /// Accessor to the map of region settings
    virtual const HandleMap& regions() const  override {
      return m_regions;
    }
    /// Accessor to the map of readout structures
    virtual const HandleMap& readouts() const  override {
      return m_readouts;
    }
    /// Accessor to the map of sub-detectors
    virtual const HandleMap& detectors() const  override {
      return m_detectors;
    }
    /// Retrieve a sensitive detector by it's name from the detector description
    virtual const HandleMap& sensitiveDetectors() const  override {
      return m_sensitive;
    }
    /// Accessor to the map of field entries, which together form the global field
    virtual const HandleMap& fields() const  override {
      return m_fields;
    }
    /// Accessor to the map of ID specifications
    virtual const HandleMap& idSpecifications() const  override {
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
    virtual const std::vector<DetElement>& detectors(const std::string& type, bool throw_exc)  override;

    /// Access a set of subdetectors according to several sensitive types.
    virtual std::vector<DetElement> detectors(const std::string& type1,
                                              const std::string& type2,
                                              const std::string& type3="",
                                              const std::string& type4="",
                                              const std::string& type5="" )  override;

    /// Access the availible detector types
    virtual std::vector<std::string> detectorTypes() const  override;

    /** return a vector with all detectors that have all the type properties in
     *  includeFlag set but none of the properties given in excludeFlag
     */
    virtual std::vector<DetElement> detectors(unsigned int includeFlag, 
                                              unsigned int excludeFlag=0 ) const   override;


#define __R  return *this
    /// Add a new constant to the detector description
    virtual Detector& add(Constant x)  override {
      return addConstant(x);
    }
    /// Add a new limit set to the detector description
    virtual Detector& add(LimitSet x)  override {
      return addLimitSet(x);
    }
    /// Add a new detector region to the detector description
    virtual Detector& add(Region x)  override {
      return addRegion(x);
    }
    /// Add a new visualisation attribute to the detector description
    virtual Detector& add(VisAttr x)  override {
      return addVisAttribute(x);
    }
    /// Add a new id descriptor to the detector description
    virtual Detector& add(IDDescriptor x)  override {
      return addIDSpecification(x);
    }
    /// Add a new detector readout to the detector description
    virtual Detector& add(Readout x)  override {
      return addReadout(x);
    }
    /// Add a new sensitive detector to the detector description
    virtual Detector& add(SensitiveDetector x)  override {
      return addSensitiveDetector(x);
    }
    /// Add a new subdetector to the detector description
    virtual Detector& add(DetElement x)  override {
      return addDetector(x);
    }
    /// Add a field component to the detector description
    virtual Detector& add(CartesianField x)  override {
      return addField(x);
    }

    /// Add a new constant by named reference to the detector description
    virtual Detector& addConstant(const Handle<NamedObject>& x)  override;

    /// Add a new limit set by named reference to the detector description
    virtual Detector& addLimitSet(const Handle<NamedObject>& x)  override {
      m_limits.append(x);
      __R;
    }
    /// Add a new detector region by named reference to the detector description
    virtual Detector& addRegion(const Handle<NamedObject>& x)  override {
      m_regions.append(x);
      __R;
    }
    /// Add a new id descriptor by named reference to the detector description
    virtual Detector& addIDSpecification(const Handle<NamedObject>& x)  override {
      m_idDict.append(x);
      __R;
    }
    /// Add a new detector readout by named reference to the detector description
    virtual Detector& addReadout(const Handle<NamedObject>& x)  override {
      m_readouts.append(x);
      __R;
    }
    /// Add a new visualisation attribute by named reference to the detector description
    virtual Detector& addVisAttribute(const Handle<NamedObject>& x)  override {
      m_display.append(x);
      __R;
    }
    /// Add a new sensitive detector by named reference to the detector description
    virtual Detector& addSensitiveDetector(const Handle<NamedObject>& x)  override {
      m_sensitive.append(x);
      __R;
    }
    /// Add a new subdetector by named reference to the detector description
    virtual Detector& addDetector(const Handle<NamedObject>& x)  override;
    /// Add a field component by named reference to the detector description
    virtual Detector& addField(const Handle<NamedObject>& x)  override;
#undef __R
    /// TObject overload: We need to set the Volume and PlacedVolume extensions to be persistent
    virtual Int_t       Write(const char *name=0, Int_t option=0, Int_t bufsize=0)  override  {
      return saveObject(name, option, bufsize);
    }
    /// TObject overload: We need to set the Volume and PlacedVolume extensions to be persistent
    virtual Int_t       Write(const char *name=0, Int_t option=0, Int_t bufsize=0) const override  {
      return saveObject(name, option, bufsize);
    }

    ClassDefOverride(DetectorImp,100);
    
  };
} /* End namespace dd4hep   */

#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link C++ class dd4hep::DetectorImp+;
#endif
#endif // DD4HEP_DETECTORIMP_H
