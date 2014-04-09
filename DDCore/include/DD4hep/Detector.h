// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_LCDD_DETECTOR_H
#define DD4HEP_LCDD_DETECTOR_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Alignment.h"
#include "DD4hep/Segmentations.h"

// C/C++ include files
#include <map>
#include <typeinfo>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    // struct SensitiveDetector;
    //  struct Detector;
    struct LCDD;

    /** @class SensitiveDetector Detector.h DD4hep/lcdd/Detector.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct SensitiveDetector: public Ref_t {
    public:
      /// Definition of the extension type
      typedef std::map<const std::type_info*, void*> Extensions;

      struct Object: public TNamed {
        unsigned int magic;
        int verbose;
        int combineHits;
        double ecut;
        std::string hitsCollection;
        Readout readout;
        Region region;
        LimitSet limits;
        Extensions extensions;

        /// Default constructor
        Object();
        /// Internal object destructor: release extension object(s)
        virtual ~Object();
      };
    protected:

      /// Templated destructor function
      template <typename T> static void _delete(void* ptr) {
        delete (T*) (ptr);
      }

      /// Add an extension object to the detector element
      void* i_addExtension(void* ptr, const std::type_info& info, void (*destruct)(void*));

      /// Access an existing extension object from the detector element
      void* i_extension(const std::type_info& info) const;

    public:

      /// Default constructor
      SensitiveDetector()
          : Ref_t() {
      }

      /// Copy from handle
      SensitiveDetector(const SensitiveDetector& sd)
          : Ref_t(sd) {
      }

      /// Copy from named handle
      SensitiveDetector(const Ref_t& sd)
          : Ref_t(sd) {
      }

      /// Templated constructor for handle conversions
      template <typename Q>
      SensitiveDetector(const Handle<Q>& e)
          : Ref_t(e) {
      }

      /// Constructor for a new sensitive detector element
      SensitiveDetector(const std::string& name, const std::string& type = "sensitive");

      /// Assignment operator
      SensitiveDetector& operator=(const SensitiveDetector& sd) {
        m_element = sd.m_element;
        return *this;
      }

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

      /// Extend the sensitive detector element with an arbitrary structure accessible by the type
      template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c) {
        return (IFACE*) i_addExtension(dynamic_cast<IFACE*>(c), typeid(IFACE), _delete<IFACE>);
      }

      /// Access extension element by the type
      template <class T> T* extension() const {
        return (T*) i_extension(typeid(T));
      }
    };

    /** @class SubDetector Detector.h DD4hep/lcdd/Detector.h
     *
     * Detector elements (class DetElement are entities which represent 
     * subdetectors or sizable parts of a subdetector.
     * A DetElement instance has the means to provide to clients information about
     *
     * --  the detector hierarchy by exposing its children.
     * --  its placement within the overall experiment if it represents an 
     * entire subdetector or its placement with respect to its parent
     * if the \tw{DetElement} represents a part of a subdetector.
     * --  information about the \tw{Readout} structure if the object is 
     * instrumented and read-out. Otherwise this link is empty.
     * --  information about the environmental conditions etc. (\tw{conditons}).
     * --  alignment information.
     *
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct DetElement: public Ref_t {
      typedef Ref_t Parent;
      typedef std::map<std::string, DetElement> Children;
      typedef std::map<const std::type_info*, void*> Extensions;

      enum CopyParameters {
        COPY_NONE = 0, COPY_PLACEMENT = 1 << 0, COPY_PARENT = 1 << 1, COPY_ALIGNMENT = 1 << 2, LAST
      };
      struct Object: public TNamed {
	/// Magic number to ensure data integrity
        unsigned int magic;
	/// Unique integer identifier of the detector instance
        int id;
        /// Full path to this detector element. May be invalid
        std::string path;
	/// Flag to process hits
        int combineHits;

	/// The subdetector placement corresponding to the detector element's volume
        PlacedVolume placement;
	/** The cached VolumeID of this subdetector element
	 *  Please note:
	 *  These values are set when populating the volume manager.
	 *  There are restrictions: e.g. only sensitive subdetectors are present.
	 */
	VolumeID volumeID;
	/// Reference to the parent element
        Parent parent;
	/// Reference element for stored transformations
        Parent reference;
	/// The array of children
        Children children;
	/// User extensions
        Extensions extensions;

	/**@info: Additional information set externally to facilitate the processing of event data */
	/// Basic detector element alignment entry
        Alignment alignment;
	/// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
	std::vector<Alignment> volume_alignments;
	/// The detector elements condition entry
        Conditions conditions;

	/**@info: Cached information of the detector element  */
        /// Intermediate buffer to store the transformation to the world coordination system
        TGeoMatrix* worldTrafo;
        /// Intermediate buffer to store the transformation to the parent detector element
        TGeoMatrix* parentTrafo;
        /// Intermediate buffer for the transformation to an arbitrary DetElement
        TGeoMatrix* referenceTrafo;
        /// The path to the placement of the detector element (if placed)
        std::string placementPath;

	/**@info: Public methods to ease the usage of the data. */
        /// Default constructor
        Object();
        /// Internal object destructor: release extension object(s)
        virtual ~Object();
        /// Deep object copy to replicate DetElement trees e.g. for reflection
        virtual Object* clone(int new_id, int flag) const;
        /// Conversion to reference object
        operator Ref_t();
        /// Conversion to reference object
        Ref_t asRef();
        /// Create cached matrix to transform to world coordinates
        TGeoMatrix* worldTransformation();
        /// Create cached matrix to transform to parent coordinates
        TGeoMatrix* parentTransformation();
        /// Create cached matrix to transform to reference coordinates
        TGeoMatrix* referenceTransformation();
      };

      /// Internal assert function to check conditions
      void check(bool condition, const std::string& msg) const;

    protected:

      /// Templated destructor function
      template <typename T> static void _delete(void* ptr) {
        delete (T*) (ptr);
      }
      /// Templated copy constructor
      template <typename T> static void* _copy(const void* ptr, DetElement elt) {
        return new T(*(dynamic_cast<const T*>((T*) ptr)), elt);
      }

      /// Add an extension object to the detector element
      void* i_addExtension(void* ptr, const std::type_info& info, void* (*copy)(const void*, DetElement),
          void (*destruct)(void*));
      /// Access an existing extension object from the detector element
      void* i_extension(const std::type_info& info) const;

    public:

      /// Default constructor
      DetElement()
          : Ref_t() {
      }

      /// Default constructor
      template <typename Q> DetElement(Q* data, const std::string& name, const std::string& type)
          : Ref_t(data) {
        this->assign(data, name, type);
      }

      template <typename Q> DetElement(const std::string& name, const std::string& type, int id, const Q&) {
        assign(new Q(), name, type);
        object<Object>().id = id;
      }

      /// Construction function for a new subdetector element
      template <typename Q>
      static Q* createObject(const std::string& name, const std::string& type, int id) {
        DetElement det;
        Q *p = new Q();
        Object* o = p;
        if (o) {                  // This should cause a compilation error if Q is
          det.assign(p, name, type);   // not a subclass of Object, which is mandatoryyyy
        }
        det.object<Object>().id = id;
        return p;
      }

      /// Construction function for a new subdetector element
      template <typename Q>
      static DetElement create(const std::string& name, const std::string& type, int id, Q** ptr = 0) {
        Q* p = createObject<Q>(name, type, id);
        if (ptr)
          *ptr = p;
        return DetElement(Ref_t(p));
      }

      /// Templated constructor for handle conversions
      template <typename Q> DetElement(const Handle<Q>& e)
          : Ref_t(e) {
      }

      /// Constructor to copy handle
      DetElement(const DetElement& e)
          : Ref_t(e) {
      }

#ifdef __MAKECINT__
      /// Constructor to copy handle
      DetElement(const Ref_t& e)
          : Ref_t(e) {
      }
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
      DetElement& operator=(const DetElement& e) {
        m_element = e.m_element;
        return *this;
      }

      /// Clone (Deep copy) the DetElement structure with a new name
      DetElement clone(const std::string& new_name) const;

      /// Clone (Deep copy) the DetElement structure with a new name and new identifier
      DetElement clone(const std::string& new_name, int new_id) const;

      /// Extend the detector element with an arbitrary structure accessible by the type
      template <typename IFACE, typename CONCRETE> IFACE* addExtension(CONCRETE* c) {
        return (IFACE*) i_addExtension(dynamic_cast<IFACE*>(c), typeid(IFACE), _copy<CONCRETE>, _delete<IFACE>);
      }

      /// Access extension element by the type
      template <class T> T* extension() const {
        return (T*) i_extension(typeid(T));
      }

      /// Set the detector identifier
      int id() const;
      /// Setter: Combine hits attribute
      DetElement& setCombineHits(bool value, SensitiveDetector& sens);
      /// Getter: Combine hits attribute
      bool combineHits() const;

      /// Access detector type (structure, tracker, calorimeter, etc.).
      /** Required for determination of G4 sensitive detector.
       */
      std::string type() const;
      ///  Set detector type (structure, tracker, calorimeter, etc.).
      DetElement& setType(const std::string& typ);

      /// Path of the detector element (not necessarily identical to placement path!)
      std::string path() const;
      /// Access to the full path to the placed object
      std::string placementPath() const;

      /// Set all attributes in one go
      DetElement& setAttributes(const LCDD& lcdd, const Volume& volume, const std::string& region, const std::string& limits,
          const std::string& vis);

      /// Set Visualization attributes to the detector element
      DetElement& setVisAttributes(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the regional attributes to the detector element
      DetElement& setRegion(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the limits to the detector element
      DetElement& setLimitSet(const LCDD& lcdd, const std::string& name, const Volume& volume);

      /// Access to the logical volume of the detector element's placement
      Volume volume() const;

      /// Access to the physical volume of this detector element
      PlacedVolume placement() const;
      /// Set the physical volumes of the detector element
      DetElement& setPlacement(const PlacedVolume& volume);
      /// The cached VolumeID of this subdetector element
      VolumeID volumeID() const;

      /// Add new child to the detector structure
      DetElement& add(DetElement sub_element);
      /// Access to the list of children
      const Children& children() const;
      /// Access to individual children by name
      DetElement child(const std::string& name) const;
      /// Access to the detector elements's parent
      DetElement parent() const;

      /// Access to the alignment information
      Alignment alignment() const;
      /// Access to the conditions information 
      Conditions conditions() const;

      /// Set detector element for reference transformations. Will delete existing reference trafo.
      DetElement& setReference(DetElement reference);

      /// Transformation from local coordinates of the placed volume to the world system
      bool localToWorld(const Position& local, Position& global) const;
      /// Transformation from local coordinates of the placed volume to the parent system
      bool localToParent(const Position& local, Position& parent) const;
      /// Transformation from local coordinates of the placed volume to arbitrary parent system set as reference
      bool localToReference(const Position& local, Position& reference) const;

      /// Transformation from world coordinates of the local placed volume coordinates
      bool worldToLocal(const Position& global, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      bool parentToLocal(const Position& parent, Position& local) const;
      /// Transformation from world coordinates of the local placed volume coordinates
      bool referenceToLocal(const Position& reference, Position& local) const;

    };

  } /* End namespace Geometry      */
} /* End namespace DD4hep        */
#endif    /* DD4HEP_LCDD_DETECTOR_H      */
