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
#ifndef DD4HEP_VOLUMES_H
#define DD4HEP_VOLUMES_H

// Framework include files
#include <DD4hep/Handle.h>
#include <DD4hep/Shapes.h>
#include <DD4hep/Objects.h>
#include <DD4hep/BitField64.h>

// C/C++ include files
#include <map>
#include <memory>

// ROOT include file (includes TGeoVolume + TGeoShape)
#include <TGeoNode.h>
#include <TGeoPatternFinder.h>
#include <TGeoExtension.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class  Detector;
  class  Region;
  class  LimitSet;
  class  Material;
  class  VisAttr;
  class  DetElement;
  class  SensitiveDetector;

  // Forward declarations
  class  Volume;
  class  PlacedVolume;

  /// Scan geometry and create reflected volumes
  /**
   *   Build reflections the ROOT way. 
   *   To be called once the geometry is closed.
   *   This entity can only be invoked once.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoManager.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class  ReflectionBuilder   {
    Detector& detector;
  public:
    /// Initializing constructor
    ReflectionBuilder(Detector& desc) : detector(desc) {}
    /// Default descructor
    ~ReflectionBuilder() = default;
    /// Perform scan
    void execute()  const;
  };
    
  /// Implementation class extending the ROOT placed volume
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoExtension.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PlacedVolumeExtension : public TGeoExtension  {
  public:
    typedef std::pair<std::string, int> VolID;
    /// Volume ID container
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_CORE
     */
    class VolIDs: public std::vector<VolID> {
    public:
      typedef std::vector<VolID> Base;
      /// Default constructor
      VolIDs() = default;
      /// Move constructor
      VolIDs(VolIDs&& copy) = default;
      /// Copy constructor
      VolIDs(const VolIDs& copy) = default;
      /// Move assignment
      VolIDs& operator=(VolIDs&& copy)  = default;
      /// Assignment operator
      VolIDs& operator=(const VolIDs& c)  = default;
      /// Find entry
      std::vector<VolID>::const_iterator find(const std::string& name) const;
      /// Insert new entry
      std::pair<std::vector<VolID>::iterator, bool> insert(const std::string& name, int value);
      /// Insert bunch of entries
      template< class InputIt>
      iterator insert(InputIt first, InputIt last)
      {  return this->Base::insert(this->Base::end(), first, last);    }
#if !defined __GNUCC__ || (defined __GNUCC__ && __GNUC__ > 5)
      /// Insert bunch of entries
      template< class InputIt>
      iterator insert(std::vector<VolID>::const_iterator pos, InputIt first, InputIt last)
      {  return this->Base::insert(pos, first, last);    }
#endif
      /// String representation for debugging
      std::string str()  const;
    };
    class Parameterisation;

    /// Magic word to detect memory corruptions
    unsigned long magic { 0 };
    /// Reference count on object (used to implement Grab/Release)
    long   refCount     { 0 };
    /// Reference to the parameterised transformation
    Parameterisation* params   { nullptr };
    /// ID container
    VolIDs volIDs;

  public:
    /// Default constructor
    PlacedVolumeExtension();
    /// Default move
    PlacedVolumeExtension(PlacedVolumeExtension&& copy);
    /// Copy constructor
    PlacedVolumeExtension(const PlacedVolumeExtension& c);
    /// Default destructor
    virtual ~PlacedVolumeExtension();
    /// Move assignment
    PlacedVolumeExtension& operator=(PlacedVolumeExtension&& copy);
    /// Assignment operator
    PlacedVolumeExtension& operator=(const PlacedVolumeExtension& copy);
    /// TGeoExtension overload: Method called whenever requiring a pointer to the extension
    virtual TGeoExtension *Grab()  override;
    /// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
    virtual void Release() const  override;
    /// Enable ROOT persistency
    ClassDefOverride(PlacedVolumeExtension,200);
  };
  class PlacedVolumeFeatureExtension : public  PlacedVolumeExtension {
  public:
  public:
  };

  /// Handle class holding a placed volume (also called physical volume)
  /**
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoNode.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PlacedVolume : public Handle<TGeoNode> {
  public:
    typedef PlacedVolumeExtension         Object;
    typedef PlacedVolumeExtension::VolIDs VolIDs;

    /// Abstract base for processing callbacks to PlacedVolume objects
    /** Helper to facilitate building plugins, which instrument
     *  placements and volumes e.g. during geometry scans.
     *
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
      virtual int processPlacement(PlacedVolume pv) = 0;
    };

    /// Default constructor
    PlacedVolume() = default;
    /// Move constructor
    PlacedVolume(PlacedVolume&& e) = default;
    /// Copy assignment
    PlacedVolume(const PlacedVolume& e) = default;
    /// Copy assignment from other handle type
    template <typename T> PlacedVolume(const Handle<T>& e) : Handle<TGeoNode>(e) {  }
    /// Constructor taking implementation object pointer
    PlacedVolume(const TGeoNode* e) : Handle<TGeoNode>(e) {  }
    /// Assignment operator (must match copy constructor)
    PlacedVolume& operator=(PlacedVolume&& v)  = default;
    /// Assignment operator (must match copy constructor)
    PlacedVolume& operator=(const PlacedVolume& v)  = default;
    /// Equality operator
    template <typename T> bool operator ==(const Handle<T>& e) const {
      return ptr() == e.ptr();
    }
    /// Non-Equality operator
    template <typename T> bool operator !=(const Handle<T>& e) const {
      return ptr() != e.ptr();
    }

    /// Check if placement is properly instrumented
    Object* data() const;
    /// Access the object type from the class information
    const char* type() const;
    /// Access the copy number of this placement within its mother
    int copyNumber() const;
    /// Volume material
    Material material() const;
    /// Logical volume of this placement
    Volume volume() const;
    /// Parent volume (envelope)
    Volume motherVol() const;
    /// Number of daughters placed in this volume
    std::size_t num_daughters()  const;
    /// Access the daughter by index
    PlacedVolume daughter(std::size_t which)  const;
    /// Access the full transformation matrix to the parent volume
    const TGeoMatrix& matrix()  const;
    /// Access the translation vector to the parent volume
    Position position()  const;
    /// Access to the volume IDs
    const PlacedVolumeExtension::VolIDs& volIDs() const;
    /// Add identifier
    PlacedVolume& addPhysVolID(const std::string& name, int value);
    /// String dump
    std::string toString() const;
  };

  // This needs full knowledge of the PlacedVolume class, at least for ROOT 6.28/04
  // so we place it here
  /// Optional parameters to implement special features such as parametrization
  /**
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PlacedVolumeExtension::Parameterisation {
  public:
    /** Feature: Geant4 parameterised volumes  */
    using Dimension = std::pair<Transform3D, size_t>;
    /// Reference to the starting point of the parameterisation
    Transform3D   start    {   };
    /// Reference to the parameterised transformation for dimension 1
    Dimension     trafo1D  { {}, 0UL };
    /// Reference to the parameterised transformation for dimension 2
    Dimension     trafo2D  { {}, 0UL };
    /// Reference to the parameterised transformation for dimension 3
    Dimension     trafo3D  { {}, 0UL };
    /// Number of entries for the parameterisation in dimension 2
    unsigned long flags    { 0 };
    /// Number of entries for the parameterisation in dimension 2
    unsigned long refCount { 0 };
    /// Reference to the placements of this volume
    std::vector<PlacedVolume> placements {  };
    /// Bitfield from sensitive detector to encode the volume ID on the fly
    const detail::BitFieldElement* field { nullptr };

  public:
    /// Default constructor
    Parameterisation() = default;
    /// Default move
    Parameterisation(Parameterisation&& copy) = default;
    /// Copy constructor
    Parameterisation(const Parameterisation& c) = default;
    /// Default destructor
    virtual ~Parameterisation() = default;
    /// Move assignment
    Parameterisation& operator=(Parameterisation&& copy) = default;
    /// Assignment operator
    Parameterisation& operator=(const Parameterisation& copy) = default;
    /// Increase ref count
    Parameterisation* addref();
    /// Decrease ref count
    void release();
    /// Enable ROOT persistency
    ClassDef(Parameterisation,200);
  };


  /// Implementation class extending the ROOT volume (TGeoVolume)
  /**
   *   Internal data structure optional to TGeo data.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoExtension.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class VolumeExtension : public TGeoExtension {
  public:
    /// Magic word to detect memory corruptions
    unsigned long       magic      = 0;
    /// Reference count on object (used to implement Grab/Release)
    long                refCount   = 0;
    ///
    int                 referenced = 0;
    /// Bit field to determine the usage. Bit 0...15 reserverd for system usage. 16...31 user space.
    int                 flags      = 0;
    /// Region reference
    Region              region;
    /// Limit sets used for simulation
    LimitSet            limits;
    /// Reference to visualization attributes
    VisAttr             vis;
    /// Reference to the sensitive detector
    Handle<NamedObject> sens_det;
    /// Reference to the reflected volume (or to the original volume for reflections)
    Handle<TGeoVolume>  reflected;
    /// Reference to properties
    TList* properties  { nullptr };
    /// Geant4 optimization flag: Smartless
    unsigned char       smartLess  = 0xFF;  // MUST match Volume::NO_SMARTLESS_OPTIMIZATION

    /// Default destructor
    virtual ~VolumeExtension();
    /// Default constructor
    VolumeExtension();
    /// No move
    VolumeExtension(VolumeExtension&& copy) = delete;
    /// Copy constructor
    VolumeExtension(const VolumeExtension& copy);
    /// No move assignment
    VolumeExtension& operator=(VolumeExtension&& copy) = delete;
    /// Copy assignment
    VolumeExtension& operator=(const VolumeExtension& copy);
    /// Copy the object
    void copy(const VolumeExtension& c);
    /// TGeoExtension overload: Method called whenever requiring a pointer to the extension
    virtual TGeoExtension *Grab()  override;
    /// TGeoExtension overload: Method called always when the pointer to the extension is not needed anymore
    virtual void Release() const  override;
    /// Enable ROOT persistency
    ClassDefOverride(VolumeExtension,200);
  };

  /// Handle class holding a placed volume (also called physical volume)
  /**
   *   Handle describing a Volume
   *
   *   One note about divisions:
   *   =========================
   *   Since dd4hep requires Volumes (aka TGeoVolume) and PlacedVolumes (aka TGeoNode)
   *   to be enhaced with the user extension mechanism shape divisions MUST be
   *   done using the division mechanism of the dd4hep shape or volume wrapper.
   *   Otherwise the enhancements are not added and you shall get exception
   *   when dd4hep is closing the geometry.
   *   The same argument holds when a division is made from a Volume.
   *   Unfortunately there is no reasonable way to intercept this call to the
   *   TGeo objects - except to sub-class each of them, which is not really 
   *   acceptable either.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoVolume.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Volume: public Handle<TGeoVolume> {
  public:
    typedef VolumeExtension Object;
    /// Flag bit numbers for special volume treatments
    enum  {
      VETO_SIMU     = 1,
      VETO_RECO     = 2,
      VETO_DISPLAY  = 3,
      REFLECTED     = 10,
    };
    enum ReplicationAxis  {
      REPLICATED    = 1UL << 4,
      PARAMETERIZED = 1UL << 5,
      Undefined     = 1UL << 7,
      X_axis        = 1UL << 8,
      Y_axis        = 1UL << 9,
      Z_axis        = 1UL << 10,
      Rho_axis      = 1UL << 11,
      Phi_axis      = 1UL << 12,
    };
    enum g4_optimizations  {
      NO_SMARTLESS_OPTIMIZATION = 0xFF,
    };
  public:
    /// Default constructor
    Volume() = default;
    /// Move from handle
    Volume(Volume&& v) = default;
    /// Copy from handle
    Volume(const Volume& v) = default;
    /// Copy from handle
    Volume(const TGeoVolume* v) : Handle<TGeoVolume>(v) { }
    /// Copy from arbitrary Element
    template <typename T> Volume(const Handle<T>& v) : Handle<TGeoVolume>(v) {  }

    /// Constructor to be used when creating a new geometry tree.
    Volume(const std::string& name);
    /// Constructor to be used when creating a new geometry tree. Sets also title
    Volume(const std::string& name, const std::string& title);

    /// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
    Volume(const std::string& name, const Solid& s, const Material& m);

    /// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
    Volume(const std::string& name, const std::string& title, const Solid& s, const Material& m);

    /// Assignment operator (must match move constructor)
    Volume& operator=(Volume&& a)  = default;
    /// Assignment operator (must match copy constructor)
    Volume& operator=(const Volume& a)  = default;
    /// Equality operator
    template <typename T> bool operator ==(const Handle<T>& e) const {
      return ptr() == e.ptr();
    }
    /// Non-Equality operator
    template <typename T> bool operator !=(const Handle<T>& e) const {
      return ptr() != e.ptr();
    }

    /// Set flag to enable copy number checks when inserting new nodes
    /** By default checks are enabled. If you want to disable, call this function */
    static void enableCopyNumberCheck(bool value);
    
    /// Check if placement is properly instrumented
    Object* data() const;

    /// Access the object type from the class information
    const char* type() const;

    /// Create a reflected volume tree. The reflected volume has left-handed coordinates
    Volume reflect()  const;

    /// Create a reflected volume tree. The reflected volume has left-handed coordinates.
    /** Swap the sensitive detector - if valid - on all sensitive sub-volumes
     */
    Volume reflect(SensitiveDetector sd)  const;
    
    /// If we import volumes from external sources, we have to attach the extensions to the tree
    Volume& import();

    /// Divide volume into subsections (See the ROOT manual for details)
    Volume divide(const std::string& divname, int iaxis, int ndiv, double start, double step, int numed = 0, const char* option = "");
    /** Daughter placements with auto-generated copy number for the daughter volume  */
    /// Place daughter volume. The position and rotation are the identity
    PlacedVolume placeVolume(const Volume& volume) const;
    /// Place daughter volume according to a generic Transform3D
    PlacedVolume placeVolume(const Volume& volume, const Transform3D& tr) const;
    /// Place un-rotated daughter volume at the given position.
    PlacedVolume placeVolume(const Volume& volume, const Position& pos) const;
    /// Place rotated daughter volume. The position is automatically the identity position
    PlacedVolume placeVolume(const Volume& volume, const RotationZYX& rot) const;
    /// Place rotated daughter volume. The position is automatically the identity position
    PlacedVolume placeVolume(const Volume& volume, const Rotation3D& rot) const;

    /** Daughter placements with user supplied copy number for the daughter volume  */
    /// Place daughter volume. The position and rotation are the identity
    PlacedVolume placeVolume(const Volume& volume, int copy_no) const;
    /// Place daughter volume according to a generic Transform3D
    PlacedVolume placeVolume(const Volume& volume, int copy_no, const Transform3D& tr) const;
    /// Place un-rotated daughter volume at the given position.
    PlacedVolume placeVolume(const Volume& volume, int copy_no, const Position& pos) const;
    /// Place rotated daughter volume. The position is automatically the identity position
    PlacedVolume placeVolume(const Volume& volume, int copy_no, const RotationZYX& rot) const;
    /// Place rotated daughter volume. The position is automatically the identity position
    PlacedVolume placeVolume(const Volume& volume, int copy_no, const Rotation3D& rot) const;
    /// Place daughter volume with generic TGeo matrix
    PlacedVolume placeVolume(const Volume& volume, TGeoMatrix* tr) const;
    /// Place daughter volume with generic TGeo matrix
    PlacedVolume placeVolume(const Volume& volume, int copy_nr, TGeoMatrix* tr) const;
    /// 1D volume replication implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity Daughter volume to be placed
     *  @param axis   Replication axis direction in the frame of the mother
     *  @param count  Number of entities to be placed
     *  @param inc    Transformation increment for each iteration
     *  @param start  start transormation for the first placement
     */
    PlacedVolume replicate(const Volume entity, ReplicationAxis axis, size_t count, double inc, double start=0e0);
    /// 1D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param start  start transormation for the first placement
     *  @param entity Daughter volume to be placed
     *  @param count  Number of entities to be placed
     *  @param inc    Transformation increment for each iteration
     */
    PlacedVolume paramVolume1D(const Transform3D& start, Volume entity, size_t count, const Transform3D& inc);
    /// 1D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity Daughter volume to be placed
     *  @param count  Number of entities to be placed
     *  @param inc    Transformation increment for each iteration
     */
    PlacedVolume paramVolume1D(Volume entity, size_t count, const Transform3D& trafo);
    /// 1D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity Daughter volume to be placed
     *  @param count  Number of entities to be placed
     *  @param inc    Transformation increment for each iteration
     */
    PlacedVolume paramVolume1D(Volume entity, size_t count, const Position& inc);
    /// 1D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity Daughter volume to be placed
     *  @param count  Number of entities to be placed
     *  @param inc    Transformation increment for each iteration
     */
    PlacedVolume paramVolume1D(Volume entity, size_t count, const RotationZYX& inc);

    /// 2D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     */
    PlacedVolume paramVolume2D(Volume entity, 
			       size_t count_1, const Transform3D& inc_1,
			       size_t count_2, const Transform3D& inc_2);

    /// Constructor to be used when creating a new parameterised volume object
    /** Embedding parameterised daughter placements in a mother volume
     *  @param start    start transormation for the first placement
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     */
    PlacedVolume paramVolume2D(const Transform3D& start,
			       Volume entity,
			       size_t count_1,
			       const Position& inc_1,
			       size_t count_2,
			       const Position& inc_2);

    /// Constructor to be used when creating a new parameterised volume object
    /** Embedding parameterised daughter placements in a mother volume
     *  @param start    start transormation for the first placement
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     */
    PlacedVolume paramVolume2D(Volume entity,
			       size_t count_1,
			       const Position& inc_1,
			       size_t count_2,
			       const Position& inc_2);

    /// 2D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param start    start transormation for the first placement
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     */
    PlacedVolume paramVolume2D(const Transform3D& start, Volume entity, 
			       size_t count_1, const Transform3D& inc_1,
			       size_t count_2, const Transform3D& inc_2);

    /// 3D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     *  @param count_3  Number of entities to be placed in dimension 3
     *  @param inc_3    Transformation increment for each iteration in dimension 3
     */
    PlacedVolume paramVolume3D(Volume entity, 
			       size_t count_1, const Transform3D& inc_1,
			       size_t count_2, const Transform3D& inc_2,
			       size_t count_3, const Transform3D& inc_3);

    /// 3D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param start    start transormation for the first placement
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     *  @param count_3  Number of entities to be placed in dimension 3
     *  @param inc_3    Transformation increment for each iteration in dimension 3
     */
    PlacedVolume paramVolume3D(const Transform3D& start, Volume entity, 
			       size_t count_1, const Transform3D& inc_1,
			       size_t count_2, const Transform3D& inc_2,
			       size_t count_3, const Transform3D& inc_3);

    /// 3D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     *  @param count_3  Number of entities to be placed in dimension 3
     *  @param inc_3    Transformation increment for each iteration in dimension 3
     */
    PlacedVolume paramVolume3D(Volume entity, 
			       size_t count_1, const Position& inc_1,
			       size_t count_2, const Position& inc_2,
			       size_t count_3, const Position& inc_3);

    /// 3D Parameterised volume implementation
    /** Embedding parameterised daughter placements in a mother volume
     *  @param start    start transormation for the first placement
     *  @param entity   Daughter volume to be placed
     *  @param count_1  Number of entities to be placed in dimension 1
     *  @param inc_1    Transformation increment for each iteration in dimension 1
     *  @param count_2  Number of entities to be placed in dimension 2
     *  @param inc_2    Transformation increment for each iteration in dimension 2
     *  @param count_3  Number of entities to be placed in dimension 3
     *  @param inc_3    Transformation increment for each iteration in dimension 3
     */
    PlacedVolume paramVolume3D(const Transform3D& start, Volume entity, 
			       size_t count_1, const Position& inc_1,
			       size_t count_2, const Position& inc_2,
			       size_t count_3, const Position& inc_3);

    /// Set user flags in bit-field
    void setFlagBit(unsigned int bit);
    /// Test the user flag bit
    bool testFlagBit(unsigned int bit)   const;

    /// Test if this volume was reflected
    bool isReflected()   const;
    
    /// Test if this volume is an assembly structure
    bool isAssembly()   const;

    /// Set the smartless option for G4 voxelization. Returns previous value
    unsigned char setSmartlessValue(unsigned char value);
    /// access the smartless option for G4 voxelization
    unsigned char smartlessValue()  const;
    
    /// Set the volume's option value
    const Volume& setOption(const std::string& opt) const;
    /// Access the volume's option value
    std::string option() const;

    /// Attach attributes to the volume
    const Volume& setAttributes(const Detector& description, const std::string& region, const std::string& limits,
                                const std::string& vis) const;

    /// Set the regional attributes to the volume. Note: If the name string is empty, the action is ignored.
    const Volume& setRegion(const Detector& description, const std::string& name) const;
    /// Set the regional attributes to the volume
    const Volume& setRegion(const Region& obj) const;
    /// Access to the handle to the region structure
    Region region() const;

    /// Set the limits to the volume. Note: If the name string is empty, the action is ignored.
    const Volume& setLimitSet(const Detector& description, const std::string& name) const;
    /// Set the limits to the volume
    const Volume& setLimitSet(const LimitSet& obj) const;
    /// Access to the limit set
    LimitSet limitSet() const;

    /// Set Visualization attributes to the volume
    const Volume& setVisAttributes(const VisAttr& obj) const;
    /// Set Visualization attributes to the volume. Note: If the name string is empty, the action is ignored.
    const Volume& setVisAttributes(const Detector& description, const std::string& name) const;
    /// Access the visualisation attributes
    VisAttr visAttributes() const;

    /// Assign the sensitive detector structure
    const Volume& setSensitiveDetector(const SensitiveDetector& obj) const;
    /// Access to the handle to the sensitive detector
    Handle<NamedObject> sensitiveDetector() const;
    /// Accessor if volume is sensitive (ie. is attached to a sensitive detector)
    bool isSensitive() const;

    /// Set the volume's solid shape
    const Volume& setSolid(const Solid& s) const;
    /// Access to Solid (Shape)
    Solid solid() const;
    /// Access the bounding box of the volume (if available)
    Box boundingBox() const;

    /// Set the volume's material
    const Volume& setMaterial(const Material& m) const;
    /// Access to the Volume material
    Material material() const;

    /// Check for existence of properties
    bool hasProperties()  const;

    /// Add Volume property (name-value pair)
    void addProperty(const std::string& nam, const std::string& val)  const;

    /// Access property value. Returns default_value if the property is not present
    std::string getProperty(const std::string& nam, const std::string& default_val="")  const;

    /// Auto conversion to underlying ROOT object
    operator TGeoVolume*() const {
      return m_element;
    }
  };

  /// Implementation class extending the ROOT mulit-volumes (TGeoVolumeMulti)
  /**
   *  Handle describing a multi volume.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoVolumeMulti.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class VolumeMulti : public Volume   {
    /// Import volume from pointer as a result of Solid->Divide()
    void verifyVolumeMulti();

  public:
    /// Default constructor
    VolumeMulti() = default;
    /// Copy from handle
    VolumeMulti(const VolumeMulti& v) = default;
    /// Copy from pointer as a result of Solid->Divide()
    VolumeMulti(TGeoVolume* v) : Volume(v)  {
      verifyVolumeMulti();
    }
    /// Copy from arbitrary Element
    template <typename T> VolumeMulti(const Handle<T>& v) : Volume(v) {
      verifyVolumeMulti();
    }
    /// Constructor to be used when creating a new multi-volume object
    VolumeMulti(const std::string& name, Material material);
    /// Assignment operator (must match copy constructor)
    VolumeMulti& operator=(const VolumeMulti& a) = default;
  };

  /// Implementation class extending the ROOT assembly volumes (TGeoVolumeAssembly)
  /**
   *  Handle describing a volume assembly.
   *
   *   For any further documentation please see the following ROOT documentation:
   *   \see http://root.cern.ch/root/html/TGeoVolumeAssembly.html
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class Assembly: public Volume {
  public:
    /// Default constructor
    Assembly() = default;
    /// Copy from handle
    Assembly(const Assembly& v) = default;
    /// Copy from arbitrary Element
    template <typename T> Assembly(const Handle<T>& v) : Volume(v) {  }
    /// Constructor to be used when creating a new assembly object
    Assembly(const std::string& name);
    /// Assignment operator (must match copy constructor)
    Assembly& operator=(const Assembly& a) = default;
  };

  /// Output mesh vertices to string
  std::string toStringMesh(PlacedVolume solid, int precision=2);
}         /* End namespace dd4hep          */
#endif // DD4HEP_VOLUMES_H
