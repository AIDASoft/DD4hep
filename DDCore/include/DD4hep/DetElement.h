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
#ifndef DD4HEP_DETECTOR_H
#define DD4HEP_DETECTOR_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Callback.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/ObjectExtensions.h"

// C/C++ include files
#include <map>
#include <typeinfo>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;
  class DetElementObject;
  class SensitiveDetectorObject;

  /// Handle class to hold the information of a sensitive detector.
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class SensitiveDetector: public Handle<SensitiveDetectorObject> {
  public:

    /// Default constructor
    SensitiveDetector() : Handle<SensitiveDetectorObject>() {     }

    /// Constructor to copy handled object
    SensitiveDetector(Object* obj_pointer)
      : Handle<SensitiveDetectorObject>(obj_pointer) {      }

    /// Copy from named handle
    SensitiveDetector(const Handle<SensitiveDetectorObject>& sd)
      : Handle<SensitiveDetectorObject>(sd) {      }

    /// Copy from handle
    SensitiveDetector(const SensitiveDetector& sd) = default;

    /// Templated constructor for handle conversions
    template <typename Q> SensitiveDetector(const Handle<Q>& e)
      : Handle<SensitiveDetectorObject>(e) { }

    /// Constructor for a new sensitive detector element
    SensitiveDetector(const std::string& name, const std::string& type = "sensitive");

    /// Assignment operator
    SensitiveDetector& operator=(const SensitiveDetector& sd)  = default;

    /// Access the type of the sensitive detector
    std::string type() const;

    ///  Set detector type (structure, tracker, calorimeter, etc.).
    SensitiveDetector& setType(const std::string& typ);

    /// Set flag to handle hits collection
    SensitiveDetector& setVerbose(bool value);

    /// Access flag to combine hist
    bool verbose() const;

    /// Set flag to handle hits collection
    SensitiveDetector& setCombineHits(bool value);

    /// Access flag to combine hist
    bool combineHits() const;

    /// Assign the name of the hits collection
    SensitiveDetector& setHitsCollection(const std::string& spec);

    /// Access the hits collection name
    const std::string& hitsCollection() const;

    /// Assign the IDDescriptor reference
    SensitiveDetector& setReadout(Readout readout);

    /// Access readout structure of the sensitive detector
    Readout readout() const;

    /// Set energy cut off
    SensitiveDetector& setEnergyCutoff(double value);

    /// Access energy cut off
    double energyCutoff() const;

    /// Set the regional attributes to the sensitive detector
    SensitiveDetector& setRegion(Region reg);

    /// Access to the region setting of the sensitive detector (not mandatory)
    Region region() const;

    /// Set the limits to the sensitive detector
    SensitiveDetector& setLimitSet(LimitSet limits);

    /// Access to the limit set of the sensitive detector (not mandatory).
    LimitSet limits() const;

    /// Add an extension object to the detector element
    void* addExtension(unsigned long long int key, ExtensionEntry* entry)  const;

    /// Access an existing extension object from the detector element
    void* extension(unsigned long long int key) const;

    /// Extend the sensitive detector element with an arbitrary structure accessible by the type
    template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c)  const {
      return (IFACE*) this->addExtension(detail::typeHash64<IFACE>(),new detail::DeleteExtension<CONCRETE>(c));
    }

    /// Access extension element by the type
    template <typename IFACE> IFACE* extension() const {
      return (IFACE*) this->extension(detail::typeHash64<IFACE>());
    }
  };

  /// Handle class describing a detector element
  /**
   * Detector elements (class DetElement are entities which represent
   * subdetectors or sizable parts of a subdetector.
   * A DetElement instance has the means to provide to clients information about
   *
   *    -  the detector hierarchy by exposing its children.
   *    -  its placement within the overall experiment if it represents an
   *       entire subdetector or its placement with respect to its parent
   *       if the \em DetElement represents a part of a subdetector.
   *    -  information about the \em Readout structure if the object is
   *       instrumented and read-out. Otherwise this link is empty.
   *    -  information about the environmental conditions etc. \em conditons.
   *       The access to conditions is exposed via the DetConditions interface.
   *       See dd4hep/DetConditions.h for further details.
   *    -  alignment information.
   *    .
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class DetElement: public Handle<DetElementObject>  {
  public:
    /// Abstract base for processing callbacks to DetElement objects
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class Processor {
    public:
      /// Default constructor
      Processor();
      /// Default destructor
      virtual ~Processor();
      /// Container callback for object processing
      virtual int processElement(DetElement detector) = 0;
    };

    typedef std::map<std::string, DetElement> Children;

    enum CopyParameters {
      COPY_NONE      = 0,
      COPY_PLACEMENT = 1 << 0,
      COPY_PARENT    = 1 << 1,
      COPY_ALIGNMENT = 1 << 2,
      LAST
    };

    enum UpdateParam {
      CONDITIONS_CHANGED = 1<<0,
      PLACEMENT_CHANGED  = 1<<1,
      SOMETHING_CHANGED  = 1<<2,
      PLACEMENT_ELEMENT  = 1<<20,
      PLACEMENT_HIGHEST  = 1<<21,
      PLACEMENT_DETECTOR = 1<<22,
      PLACEMENT_NONE
    };

    /// Internal assert function to check conditions
    void check(bool condition, const std::string& msg) const;

  protected:

    template <typename T> struct DetElementExtension : public ExtensionEntry  {
    protected:
      T* ptr;
    public:
      DetElementExtension() = delete;
      DetElementExtension(T* p) : ptr(p) {}
      DetElementExtension(const DetElementExtension& copy) = default;
      DetElementExtension& operator=(const DetElementExtension& copy) = default;
      virtual ~DetElementExtension() = default;
      // This one ensures we have the correct signatures
      T* copy(DetElement de)  const            { return new T(*ptr,de);   }
      virtual void* object()  const override        { return ptr;              }
      virtual void* copy(void* det)  const override { return copy(DetElement((Object*)det));  }
      virtual void  destruct()  const override      { delete ptr;              }
      virtual ExtensionEntry* clone(void* det)  const  override
      {  return new DetElementExtension<T>((T*)this->copy(det));           }
    };

    /// Internal call to extend the detector element with an arbitrary structure accessible by the type
    void i_addUpdateCall(unsigned int callback_type, const Callback& callback)  const;

  public:

    /// Default constructor
    DetElement() = default;

    /// Constructor to copy handle
    DetElement(const DetElement& e) = default;

    /// Constructor to hold handled object
    DetElement(Object* obj) : Handle<DetElementObject>(obj) { }

    /// Clone constructor
    DetElement(Object* obj, const std::string& name, const std::string& type);

    /// Templated constructor for handle conversions
    template <typename Q> DetElement(const Handle<Q>& e) : Handle<DetElementObject>(e) {}

    /// Constructor to hold handled object
    DetElement(NamedObject* obj) : Handle<DetElementObject>(obj) { }

#ifdef __MAKECINT__
    /// Constructor to copy handle
    DetElement(const Ref_t& e) : Handle<DetElementObject>(e) {  }
#endif
    /// Constructor for a new subdetector element
    DetElement(const std::string& name, const std::string& type, int id);

    /// Constructor for a new subdetector element
    DetElement(const std::string& name, int id);

    /// Constructor for a new subdetector element
    DetElement(DetElement parent, const std::string& name, int id);

    /// Additional data accessor
    Object& _data() const {
      return object<Object>();
    }

    /// Operator less to insert into a map
    bool operator <(const DetElement e) const {
      return ptr() < e.ptr();
    }

    /// Equality operator
    bool operator ==(const DetElement e) const {
      return ptr() == e.ptr();
    }

    /// Assignment operator
    DetElement& operator=(const DetElement& e) = default;

    /// Clone (Deep copy) the DetElement structure with a new name
    DetElement clone(const std::string& new_name) const;

    /// Clone (Deep copy) the DetElement structure with a new name and new identifier
    DetElement clone(const std::string& new_name, int new_id) const;

    /// Add an extension object to the detector element
    void* addExtension(unsigned long long int key,ExtensionEntry* entry) const;

    /// Access an existing extension object from the detector element
    void* extension(unsigned long long int key) const;

    /// Extend the detector element with an arbitrary structure accessible by the type
    template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c) const {
      CallbackSequence::checkTypes(typeid(IFACE), typeid(CONCRETE), dynamic_cast<IFACE*>(c));
      return (IFACE*) this->addExtension(detail::typeHash64<IFACE>(),new DetElementExtension<CONCRETE>(c));
    }
    /// Access extension element by the type
    template <typename IFACE> IFACE* extension() const {
      return (IFACE*) this->extension(detail::typeHash64<IFACE>());
    }
    /// Extend the detector element with an arbitrary callback
    template <typename Q, typename T>
    void callAtUpdate(unsigned int typ, Q* pointer,
                      void (T::*pmf)(unsigned long typ, DetElement& det, void* opt_par)) const
    {
      CallbackSequence::checkTypes(typeid(T), typeid(Q), dynamic_cast<T*>(pointer));
      i_addUpdateCall(typ, Callback(pointer).make(pmf));
    }
    /// Remove callback from object
    void removeAtUpdate(unsigned int type, void* pointer) const;

    /// Get the detector identifier
    int id() const;
    /// Setter: Combine hits attribute
    DetElement& setCombineHits(bool value, SensitiveDetector& sens);
    /// Getter: Combine hits attribute
    bool combineHits() const;

    /** Access detector type (structure, tracker, calorimeter, etc.).
     *  Required for determination of G4 sensitive detector.
     */
    std::string type() const;
    ///  Set detector type (structure, tracker, calorimeter, etc.).
    DetElement& setType(const std::string& typ);

    // Return flag word encoding detector types ( ideally use dd4hep::DetType for decoding )
    unsigned int typeFlag() const;

    ///  Set the flag word encoding detector types ( ideally use dd4hep::DetType for encoding )
    DetElement& setTypeFlag(unsigned int types);

    /// Access hash key of this detector element (Only valid once geometry is closed!)
    unsigned int key()  const;
    /// Access the hierarchical level of the detector element (Only valid once geometry is closed!)
    int level()  const;
    /// Path of the detector element (not necessarily identical to placement path!)
    const std::string& path() const;
    /// Access to the full path to the placed object
    const std::string& placementPath() const;

    /// Set all attributes in one go
    DetElement& setAttributes(const Detector& description,
                              const Volume& volume,
                              const std::string& region,
                              const std::string& limits,
                              const std::string& vis);

    /// Set Visualization attributes to the detector element
    DetElement& setVisAttributes(const Detector& description, const std::string& name, const Volume& volume);
    /// Set the regional attributes to the detector element
    DetElement& setRegion(const Detector& description, const std::string& name, const Volume& volume);
    /// Set the limits to the detector element
    DetElement& setLimitSet(const Detector& description, const std::string& name, const Volume& volume);

    /// Access to the logical volume of the detector element's placement
    Volume volume() const;

    /// Access to the physical volume of this detector element
    /** This is the current placement value of the detector eleemnt.
     *  A possible global re-alignment may alter the value.
     *  Hence, it should hence not be cached.
     */
    PlacedVolume placement() const;
    /// Access to the ideal physical volume of this detector element
    /** This is the original placement set in the detector constructor.
     *  A possible global re-alignment make this value different
     *  from the regular placement() call.
     */
    PlacedVolume idealPlacement() const;
    /// Set the physical volumes of the detector element
    DetElement&  setPlacement(const PlacedVolume& volume);
    /// The cached VolumeID of this subdetector element
    VolumeID     volumeID() const;

    /// Add new child to the detector structure
    DetElement&  add(DetElement sub_element);
    /// Access to the list of children
    const Children& children() const;
    /// Access to individual children by name
    DetElement child(const std::string& name) const;
    /// Access to the detector elements's parent
    DetElement parent() const;
    /// Access to the world object. Only possible once the geometry is closed.
    DetElement world()  const;

    /// Access to the constant ideal (nominal) alignment information
    Alignment nominal() const;
    /// Access to the constant survey alignment information
    Alignment survey() const;
  };
} /* End namespace dd4hep        */

#include "DD4hep/AlignmentData.h"

#endif    /* DD4HEP_DETECTOR_H      */
