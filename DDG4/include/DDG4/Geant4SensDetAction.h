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
#ifndef DDG4_GEANT4SENSDETACTION_H
#define DDG4_GEANT4SENSDETACTION_H

// Framework include files
#include <DD4hep/Detector.h>
#include <DDG4/Geant4Action.h>
#include <DDG4/Geant4HitCollection.h>

// Geant4 include files
#include <G4ThreeVector.hh>
#include <G4VTouchable.hh>

// C/C++ include files
#include <vector>

// Forward declarations
class G4HCofThisEvent;
class G4Step;
class G4Event;
class G4TouchableHistory;
class G4VHitsCollection;
class G4VReadOutGeometry;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;
  class DetElement;
  class SensitiveDetector;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4Sensitive;
    class Geant4FastSimSpot;
    class Geant4StepHandler;
    class Geant4HitCollection;
    class Geant4SensDetActionSequence;
    class Geant4SensDetSequences;

    /// Interface class to access properties of the underlying Geant4 sensitive detector structure
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ActionSD: virtual public Geant4Action {
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4ActionSD);
      /// Standard action constructor
      Geant4ActionSD(const std::string& name);
      /// Default destructor
      virtual ~Geant4ActionSD();
    public:
      /// Initialize the usage of a hit collection. Returns the collection identifier
      virtual std::size_t defineCollection(const std::string& name) = 0;
      /// Access to the readout geometry of the sensitive detector
      virtual G4VReadOutGeometry* readoutGeometry() const = 0;
      /// This is a utility method which returns the hits collection ID
      virtual G4int GetCollectionID(G4int i) = 0;
      /// Is the detector active?
      virtual bool isActive() const = 0;
      /// Access to the Detector sensitive detector handle
      virtual SensitiveDetector sensitiveDetector() const = 0;
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string path() const = 0;
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string fullPath() const = 0;
      /// Access to the sensitive type of the detector
      virtual const std::string& sensitiveType() const = 0;
      /// Access the DDG4 action sequence
      virtual Geant4SensDetActionSequence* sequence() const = 0;
    };

    /// Base class to construct filters for Geant4 sensitive detectors
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Filter: public Geant4Action {
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4Filter);

    public:
      /// Standard constructor
      Geant4Filter(Geant4Context* context, const std::string& name);
      /// Standard destructor
      virtual ~Geant4Filter();

      /// Filter action. Return true if hits should be processed. Default returns true
      virtual bool operator()(const G4Step* step) const;

      /// GFLASH/FastSim interface: Filter action. Return true if hits should be processed.
      /** The default implementation throws an exception that the 
       *  GFLASH / FastSim interface is not implemented.
       */
      virtual bool operator()(const Geant4FastSimSpot* spot) const;
    };

    /// The base class for Geant4 sensitive detector actions implemented by users
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Sensitive: public Geant4Action {
    public:
      enum HitCreationFlags {
        SIMPLE_MODE = 0,
        MEDIUM_MODE = 1<<0,
        DETAILED_MODE = 1<<1
      };

    private:
      /// Reference to G4 sensitive detector
      Geant4ActionSD* m_sensitiveDetector     { nullptr };
      /// Reference to the containing action sequence
      Geant4SensDetActionSequence* m_sequence { nullptr };

    protected:
      /// Property: Hit creation mode. Maybe one of the enum HitCreationFlags
      int  m_hitCreationMode                  {       0 };
      /// Property: Use the volume manager to access CellID and VolumeID
      bool m_useVolumeManager                 {    true };

#if defined(G__ROOT) || defined(__CLING__) || defined(__ROOTCLING__)
      /// Reference to the detector description object
      Detector*            m_detDesc          { nullptr };
#else
      /// Reference to the detector description object
      Detector&            m_detDesc;
#endif
      /// Reference to the detector element describing this sensitive element
      DetElement           m_detector         {  };
      /// Reference to the sensitive detector element
      SensitiveDetector    m_sensitive        {  };
      /// Reference to the readout structure
      Readout              m_readout          {  };
      /// Reference to segmentation
      Segmentation         m_segmentation     {  };
      /// The list of sensitive detector filter objects
      Actors<Geant4Filter> m_filters;

      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4Sensitive);

    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4Sensitive(Geant4Context* context, const std::string& name, DetElement det, Detector& description);

      /// Standard destructor
      virtual ~Geant4Sensitive();

      /// Get the detector identifier (DetElement::id())
      int id()  const;
  
      /// Access to the sensitive detector object
      Geant4ActionSD& detector() const;

      /// Access to the sensitive detector object
      void setDetector(Geant4ActionSD* sens_det);

      /// Access volume manager usage flag. This is a subdetector specific flag
      bool useVolumeManager()  const  {
        return m_useVolumeManager;
      }

      /// Property access to the hit creation mode
      int hitCreationMode() const  {
        return m_hitCreationMode;
      }

      /// G4VSensitiveDetector internals: Access to the detector name
      std::string detectorName() const {
        return detector().name();
      }

      /// G4VSensitiveDetector internals: Access to the detector path name
      std::string path() const {
        return detector().path();
      }

      /// G4VSensitiveDetector internals: Access to the detector path name
      std::string fullPath() const {
        return detector().fullPath();
      }

      /// G4VSensitiveDetector internals: Is the detector active?
      bool isActive() const {
        return detector().isActive();
      }

      /// Access the dd4hep sensitive detector
      SensitiveDetector sensitiveDetector() const  {
        return m_sensitive;
      }

      /// Access to the readout geometry of the sensitive detector
      G4VReadOutGeometry* readoutGeometry() const {
        return detector().readoutGeometry();
      }

      /// Access the detector description object
      Detector& detectorDescription()   const;

      /// Mark the track to be kept for MC truth propagation during hit processing
      void mark(const G4Track* track) const;

      /// Mark the track of this step to be kept for MC truth propagation during hit processing
      void mark(const G4Step* step) const;

      /// Access to the hosting sequence
      Geant4SensDetActionSequence& sequence() const;

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4Filter* filter);

      /// Add an actor responding to all callbacks to the sequence front. Sequence takes ownership.
      void adopt_front(Geant4Filter* filter);

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adoptFilter(Geant4Action* filter);

      /// Add an actor responding to all callbacks to the sequence front. Sequence takes ownership.
      void adoptFilter_front(Geant4Action* filter);

      /// Callback before hit processing starts. Invoke all filters.
      /** Return false if any filter returns false
       */
      bool accept(const G4Step* step) const;

      /// GFLASH/FastSim interface: Callback before hit processing starts. Invoke all filters.
      /** Return false if any filter returns false
       */
      bool accept(const Geant4FastSimSpot* step) const;

      /// Initialize the usage of a single hit collection. Returns the collection ID
      template <typename TYPE> std::size_t defineCollection(const std::string& coll_name);

      /// Access HitCollection container names
      const std::string& hitCollectionName(std::size_t which) const;

      /// Retrieve the hits collection associated with this detector by its serial number
      Geant4HitCollection* collection(std::size_t which);

      /// Retrieve the hits collection associated with this detector by its collection identifier
      Geant4HitCollection* collectionByID(std::size_t id);

      /// Define collections created by this sensitivie action object
      virtual void defineCollections();

      /// G4VSensitiveDetector interface: Method invoked at the beginning of each event.
      /** The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void begin(G4HCofThisEvent* hce);

      /// G4VSensitiveDetector interface: Method invoked at the end of each event.
      virtual void end(G4HCofThisEvent* hce);

      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      /** Hits collections created but not being set to G4HCofThisEvent
       *  at the event should be deleted.
       *  Collection(s) which have already set to G4HCofThisEvent
       *  will be deleted automatically.
       */
      virtual void clear(G4HCofThisEvent* hce);

      /// Returns the volumeID of the sensitive volume corresponding to the step
      /** Combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
       * from the current sensitive volume to the world volume
       */
      long long int volumeID(const G4Step* step);

      /// Returns the volumeID of the sensitive volume corresponding to the G4VTouchable
      /** Combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
       * from the current sensitive volume to the world volume
       */
      long long int volumeID(const G4VTouchable* touchable);

      /// Returns the cellID of the sensitive volume corresponding to the step
      /** The CellID is the VolumeID + the local coordinates of the sensitive area.
       *  Calculated by combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
       *  from the current sensitive volume to the world volume
       */
      long long int cellID(const G4Step* step);

      /// Returns the cellID of the sensitive volume corresponding to the G4VTouchable
      /** The CellID is the VolumeID + the local coordinates of the sensitive area.
       *  Calculated by combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
       *  from the current sensitive volume to the world volume
       */
      long long int cellID(const G4VTouchable* touchable, const G4ThreeVector& global);

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual bool process(const G4Step* step, G4TouchableHistory* history);

      /// GFLASH/FastSim interface: Method for generating hit(s) using the information of the fast simulation spot object.
      /** The default implementation throws an exception that the 
       *  GFLASH/FastSim interface is not implemented.
       */
      virtual bool processFastSim(const Geant4FastSimSpot* spot, G4TouchableHistory* history);
    };

    /// The sequencer to host Geant4 sensitive actions called if particles interact with sensitive elements
    /**
     * Concrete implementation of the sensitive detector action sequence
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SensDetActionSequence: public Geant4Action {
    public:
      typedef Geant4HitCollection* (*create_t)(const std::string&, const std::string&, Geant4Sensitive*);
      typedef std::pair<std::string, std::pair<Geant4Sensitive*,create_t> > HitCollection;
      typedef std::vector<HitCollection> HitCollections;

    protected:
      /// Geant4 hit collection context
      G4HCofThisEvent*        m_hce  { nullptr };
      /// Callback sequence for event initialization action
      CallbackSequence        m_begin;
      /// Callback sequence for event finalization action
      CallbackSequence        m_end;
      /// Callback sequence for step processing
      CallbackSequence        m_process;
      /// Callback sequence to invoke the event deletion
      CallbackSequence        m_clear;
      /// The list of sensitive detector objects
      Actors<Geant4Sensitive> m_actors;
      /// The list of sensitive detector filter objects
      Actors<Geant4Filter>    m_filters;

      /// Hit collection creators
      HitCollections          m_collections;
      /// Reference to the sensitive detector element
      SensitiveDetector       m_sensitive;
      /// Reference to G4 sensitive detector
      Geant4ActionSD*         m_detector;
      /// The true sensitive type of the detector
      std::string             m_sensitiveType;
      /// Create a new typed hit collection
      template <typename TYPE> static 
      Geant4HitCollection* _create(const std::string& det, const std::string& coll, Geant4Sensitive* sd) {
        return new Geant4HitCollection(det, coll, sd, (TYPE*) 0);
      }

    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SensDetActionSequence);

    public:
      /// Standard constructor
      Geant4SensDetActionSequence(Geant4Context* context, const std::string& name);

      /// Default destructor
      virtual ~Geant4SensDetActionSequence();

      /// Access to the sensitive type of the detector
      virtual const std::string& sensitiveType() const   {
        return m_sensitiveType;
      }

      /// Set or update client context
      virtual void updateContext(Geant4Context* ctxt)  override;

      /// Called at construction time of the sensitive detector to declare all hit collections
      std::size_t defineCollections(Geant4ActionSD* sens_det);

      /// Initialize the usage of a hit collection. Returns the collection identifier
      std::size_t defineCollection(Geant4Sensitive* owner, const std::string& name, create_t func);

      /// Define a named collection containing hist of a specified type
      template <typename TYPE> std::size_t defineCollection(Geant4Sensitive* owner, const std::string& coll_name) {
        return defineCollection(owner, coll_name, Geant4SensDetActionSequence::_create<TYPE>);
      }

      /// Access HitCollection container names
      const std::string& hitCollectionName(std::size_t which) const;

      /// Retrieve the hits collection associated with this detector by its serial number
      Geant4HitCollection* collection(std::size_t which) const;

      /// Retrieve the hits collection associated with this detector by its collection identifier
      Geant4HitCollection* collectionByID(std::size_t id) const;

      /// Register begin-of-event callback
      template <typename T> void callAtBegin(T* p, void (T::*f)(G4HCofThisEvent*)) {
        m_begin.add(p, f);
      }

      /// Register end-of-event callback
      template <typename T> void callAtEnd(T* p, void (T::*f)(G4HCofThisEvent*)) {
        m_end.add(p, f);
      }

      /// Register process-hit callback
      template <typename T> void callAtProcess(T* p, void (T::*f)(G4Step*, G4TouchableHistory*)) {
        m_process.add(p, f);
      }

      /// Register clear callback
      template <typename T> void callAtClear(T* p, void (T::*f)(G4HCofThisEvent*)) {
        m_clear.add(p, f);
      }

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4Sensitive* sensitive);

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4Filter* filter);

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adoptFilter(Geant4Action* filter);

      /// Callback before hit processing starts. Invoke all filters.
      bool accept(const G4Step* step) const;

      /// GFLASH/FastSim interface: Callback before hit processing starts. Invoke all filters.
      bool accept(const Geant4FastSimSpot* step) const;

      /// G4VSensitiveDetector interface: Method invoked at the beginning of each event.
      /** The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void begin(G4HCofThisEvent* hce);

      /// G4VSensitiveDetector interface: Method invoked at the end of each event.

      virtual void end(G4HCofThisEvent* hce);
      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      /** Hits collections created but not beibg set to G4HCofThisEvent
       *  at the event should be deleted.
       *  Collection(s) which have already set to G4HCofThisEvent
       *  will be deleted automatically.
       */
      virtual void clear();

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual bool process(const G4Step* step, G4TouchableHistory* history);

      /// GFLASH/FastSim interface: Method for generating hit(s) using the information of the fast simulation spot object.
      virtual bool processFastSim(const Geant4FastSimSpot* spot, G4TouchableHistory* history);
    };

    /// Geant4SensDetSequences: class to access groups of sensitive actions
    /**
     * Concrete implementation of the sensitive detector action sequence
     *
     * Note Multi-Threading issue:
     * Neither callbacks not the action list is protected against multiple 
     * threads calling the Geant4 callbacks!
     * These must be protected in the user actions themselves.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SensDetSequences {
    public:
      friend class Geant4ActionContainer;
      friend class Geant4SensDetActionSequence;
      typedef std::map<std::string, Geant4SensDetActionSequence*> Members;
    private:
      Members m_sequences;
      /// Insert sequence member
      void insert(const std::string& name, Geant4SensDetActionSequence* seq);

    public:
      /// Default constructor
      Geant4SensDetSequences() = default;
      /// Default destructor
      virtual ~Geant4SensDetSequences();
      /// Access sequence member by name
      Geant4SensDetActionSequence* operator[](const std::string& name) const;
      /// Access sequence member by name
      Geant4SensDetActionSequence* find(const std::string& name) const;
      /// Access to the members
      Members& sequences() {
        return m_sequences;
      }
      /// Access to the members CONST
      const Members& sequences() const {
        return m_sequences;
      }
      /// Clear the sequence list
      void clear();
    };

    /// Initialize the usage of a single hit collection. Returns the collection ID
    template <typename TYPE> inline std::size_t Geant4Sensitive::defineCollection(const std::string& coll_name) {
      return sequence().defineCollection<TYPE>(this, coll_name);
    }

    /// Template class to ease the construction of sensitive detectors using particle template specialization
    /**
     * Templated implementation to realize sensitive detectors.
     * Default implementations for all functions are provided in the file
     * DDG4/Geant4SensDetAction.inl.
     *
     * Users may override any of the templated callbacks or the of the virtual functions
     * of the base class using explicit template specialization.
     * An example may be found in DDG4/plugins/Geant4SDActions.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename T> class Geant4SensitiveAction : public Geant4Sensitive  {
    public:
      typedef T UserData;
    protected:
      /// Property: collection name. If not set default is readout name!
      std::string m_collectionName  {   };
      /// Property: segmentation name for parallel readouts. If not set readout segmentation is used!
      std::string m_readoutName     {   };

      /// Collection identifier
      std::size_t m_collectionID    { 0 };
      /// User data block
      UserData    m_userData        {   };

    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4SensitiveAction);

    public:
      /// Standard , initializing constructor
      Geant4SensitiveAction(Geant4Context* context,
                            const std::string& name,
                            DetElement det,
                            Detector& description);

      /// Default destructor
      virtual ~Geant4SensitiveAction();

      /// Declare optional properties from embedded structure
      void declareOptionalProperties();

      /// Define collections created by this sensitivie action object
      virtual void defineCollections();

      /// Access the readout object. Note: if m_readoutName is set, the m_readout != m_sensitive.readout()
      Readout readout();
      
      /// Define readout specific hit collection with volume ID filtering
      /** 
       *  Convenience function. To be called by specialized sensitive actions inheriting this class.
       *
       *  - If the property CollectionName ist NOT set, one default collection is declared,
       *    with the readout name as the collection name.
       *  - If the property CollectionName is set, the readout structure is searched for
       *    the corresponding entry and a collection with the corrsponding name is declared.
       *    At the same time a VolumeID filter is injected at the front of the sensitive's 
       *    filter queue to ONLY act on volume IDs matching this criterium.
       */
      template <typename HIT> std::size_t declareReadoutFilteredCollection();

      /// Define readout specific hit collection. matching name must be present in readout structure
      template <typename HIT> 
      std::size_t defineReadoutCollection(const std::string collection_name);

      /// Initialization overload for specialization
      virtual void initialize()  final;
      /// Finalization overload for specialization
      virtual void finalize()  final;

      /// G4VSensitiveDetector interface: Method invoked at the beginning of each event.
      virtual void begin(G4HCofThisEvent* hce)  final;
      /// G4VSensitiveDetector interface: Method invoked at the end of each event.
      virtual void end(G4HCofThisEvent* hce)  final;
      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      virtual void clear(G4HCofThisEvent* hce)  final;

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the G4Step object.
      virtual bool process(const G4Step* step,G4TouchableHistory* history)  final;

      /// GFLASH/FastSim interface: Method for generating hit(s) using the information of the fast simulation spot object.
      virtual bool processFastSim(const Geant4FastSimSpot* spot, G4TouchableHistory* history)  final;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DDG4_GEANT4SENSDETACTION_H
