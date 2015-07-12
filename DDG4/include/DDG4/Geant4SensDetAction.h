// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4HEP_DDG4_GEANT4SENSDETACTION_H
#define DD4HEP_DDG4_GEANT4SENSDETACTION_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDG4/Geant4Action.h"
#include "DDG4/Geant4HitCollection.h"

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
namespace DD4hep {

  // Forward declarations
  namespace Geometry  {
    class LCDD;
    class DetElement;
    class SensitiveDetector;
  }

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4StepHandler;
    class Geant4HitCollection;
    class Geant4Sensitive;
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
      /// Standard action constructor
      Geant4ActionSD(const std::string& name);
      /// Default destructor
      virtual ~Geant4ActionSD();
    public:
      /// Definition: Sensitive detector type
      typedef Geometry::SensitiveDetector SensitiveDetector;

      /// Initialize the usage of a hit collection. Returns the collection identifier
      virtual size_t defineCollection(const std::string& name) = 0;
      /// Access to the readout geometry of the sensitive detector
      virtual G4VReadOutGeometry* readoutGeometry() const = 0;
      /// This is a utility method which returns the hits collection ID
      virtual G4int GetCollectionID(G4int i) = 0;
      /// Is the detector active?
      virtual bool isActive() const = 0;
      /// Access to the LCDD sensitive detector handle
      virtual SensitiveDetector sensitiveDetector() const = 0;
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string path() const = 0;
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string fullPath() const = 0;
      /// Access to the sensitive type of the detector
      virtual const std::string& sensitiveType() const = 0;
    };

    /// Base class to construct filters for Geant4 sensitive detectors
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Filter: public Geant4Action {
    public:
      /// Standard constructor
      Geant4Filter(Geant4Context* context, const std::string& name);
      /// Standard destructor
      virtual ~Geant4Filter();
      /// Filter action. Return true if hits should be processed
      virtual bool operator()(const G4Step* step) const;
    };

    /// The base class for Geant4 sensitive detector actions implemented by users
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Sensitive: public Geant4Action {
    public:
      typedef Geometry::LCDD LCDD;
      typedef Geometry::Readout Readout;
      typedef Geometry::DetElement DetElement;
      typedef Geometry::Segmentation Segmentation;
      /// Definition: Sensitive detector type
      typedef Geometry::SensitiveDetector SensitiveDetector;

      typedef Geant4StepHandler StepHandler;
      typedef Geant4HitCollection HitCollection;

      enum HitCreationFlags {
        SIMPLE_MODE = 0,
        MEDIUM_MODE = 1<<0,
        DETAILED_MODE = 1<<1
      };

    private:
      /// Reference to G4 sensitive detector
      Geant4ActionSD* m_sensitiveDetector;
      /// Reference to the containing action sequence
      Geant4SensDetActionSequence* m_sequence;

    protected:
      /// Property: Hit creation mode. Maybe one of the enum HitCreationFlags
      int  m_hitCreationMode;
      /// Reference to the detector description object
      LCDD& m_lcdd;
      /// Reference to the detector element describing this sensitive element
      DetElement m_detector;
      /// Reference to the sensitive detector element
      SensitiveDetector m_sensitive;
      /// Reference to the readout structure
      Readout m_readout;
      /// Reference to segmentation
      Segmentation m_segmentation;
      /// The list of sensitive detector filter objects
      Actors<Geant4Filter> m_filters;

    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4Sensitive(Geant4Context* context, const std::string& name, DetElement det, LCDD& lcdd);

      /// Standard destructor
      virtual ~Geant4Sensitive();

      /// Access to the sensitive detector object
      Geant4ActionSD& detector() const;

      /// Access to the sensitive detector object
      void setDetector(Geant4ActionSD* sens_det);

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

      /// Access the DD4hep sensitive detector
      SensitiveDetector sensitiveDetector() const  {
        return m_sensitive;
      }

      /// Access to the readout geometry of the sensitive detector
      G4VReadOutGeometry* readoutGeometry() const {
        return detector().readoutGeometry();
      }

      /// Mark the track to be kept for MC truth propagation during hit processing
      void mark(const G4Track* track) const;

      /// Mark the track of this step to be kept for MC truth propagation during hit processing
      void mark(const G4Step* step) const;

      /// Access to the hosting sequence
      Geant4SensDetActionSequence& sequence() const;

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4Filter* filter);

      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adoptFilter(Geant4Action* filter);

      /// Callback before hit processing starts. Invoke all filters.
      /** Return fals if any filter returns false
       */
      bool accept(const G4Step* step) const;

      /// Initialize the usage of a single hit collection. Returns the collection ID
      template <typename TYPE> size_t defineCollection(const std::string& coll_name);

      /// Access HitCollection container names
      const std::string& hitCollectionName(size_t which) const;

      /// Retrieve the hits collection associated with this detector by its serial number
      HitCollection* collection(size_t which);

      /// Retrieve the hits collection associated with this detector by its collection identifier
      HitCollection* collectionByID(size_t id);

      /// G4VSensitiveDetector interface: Method invoked at the begining of each event.
      /** The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void begin(G4HCofThisEvent* hce);

      /// G4VSensitiveDetector interface: Method invoked at the end of each event.
      virtual void end(G4HCofThisEvent* hce);

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual bool process(G4Step* step, G4TouchableHistory* history);

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
      long long int volumeID(G4Step* step);

      /// Returns the cellID of the sensitive volume corresponding to the step
      /** The CellID is the VolumeID + the local coordinates of the sensitive area.
       *  Calculated by combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
       *  from the current sensitive volume to the world volume
       */
      long long int cellID(G4Step* step);

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

      typedef Geometry::SensitiveDetector SensitiveDetector;
      typedef Geant4HitCollection* (*create_t)(const std::string&, const std::string&, Geant4Sensitive*);
      typedef std::pair<std::string, std::pair<Geant4Sensitive*,create_t> > HitCollection;
      typedef std::vector<HitCollection> HitCollections;

    protected:
      /// Geant4 hit collection context
      G4HCofThisEvent* m_hce;
      /// Callback sequence for event initialization action
      CallbackSequence m_begin;
      /// Callback sequence for event finalization action
      CallbackSequence m_end;
      /// Callback sequence for step processing
      CallbackSequence m_process;
      /// Callback sequence to invoke the event deletion
      CallbackSequence m_clear;
      /// The list of sensitive detector objects
      Actors<Geant4Sensitive> m_actors;
      /// The list of sensitive detector filter objects
      Actors<Geant4Filter>    m_filters;

      /// Hit collection creators
      HitCollections m_collections;
      /// Reference to the sensitive detector element
      SensitiveDetector m_sensitive;
      /// Reference to G4 sensitive detector
      Geant4ActionSD* m_detector;
      /// The true sensitive type of the detector
      std::string m_sensitiveType;
      /// Create a new typed hit collection
      template <typename TYPE> static Geant4HitCollection* _create(const std::string& det, const std::string& coll, Geant4Sensitive* sd) {
        return new Geant4HitCollection(det, coll, sd, (TYPE*) 0);
      }

    public:
      /// Standard constructor
      Geant4SensDetActionSequence(Geant4Context* context, const std::string& name);

      /// Default destructor
      virtual ~Geant4SensDetActionSequence();

      /// Access to the sensitive type of the detector
      virtual const std::string& sensitiveType() const   {
        return m_sensitiveType;
      }

      /// Called at construction time of the sensitive detector to declare all hit collections
      size_t defineCollections(Geant4ActionSD* sens_det);

      /// Initialize the usage of a hit collection. Returns the collection identifier
      size_t defineCollection(Geant4Sensitive* owner, const std::string& name, create_t func);

      /// Define a named collection containing hist of a specified type
      template <typename TYPE> size_t defineCollection(Geant4Sensitive* owner, const std::string& coll_name) {
        return defineCollection(owner, coll_name, Geant4SensDetActionSequence::_create<TYPE>);
      }

      /// Access HitCollection container names
      const std::string& hitCollectionName(size_t which) const;
      /// Retrieve the hits collection associated with this detector by its serial number
      Geant4HitCollection* collection(size_t which) const;
      /// Retrieve the hits collection associated with this detector by its collection identifier
      Geant4HitCollection* collectionByID(size_t id) const;
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
      /** Return fals if any filter returns false
       */
      bool accept(const G4Step* step) const;

      /// Function to process hits
      virtual bool process(G4Step* step, G4TouchableHistory* hist);

      /// G4VSensitiveDetector interface: Method invoked at the begining of each event.
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
    };

    /// Geant4SensDetSequences: class to access groups of sensitive actions
    /**
     * Concrete implementation of the sensitive detector action sequence
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SensDetSequences {
    public:
      friend class Geant4Kernel;
      friend class Geant4SensDetActionSequence;
      typedef std::map<std::string, Geant4SensDetActionSequence*> Members;
    private:
      Members m_sequences;
      /// Insert sequence member
      void insert(const std::string& name, Geant4SensDetActionSequence* seq);
    public:
      /// Default constructor
      Geant4SensDetSequences();
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
    template <typename TYPE> inline size_t Geant4Sensitive::defineCollection(const std::string& coll_name) {
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
     * An example may be found in DDG4/plugins/eant4SDActions.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename T> class Geant4SensitiveAction : public Geant4Sensitive  {
    public:
      typedef T UserData;
    protected:
      /// Collection identifier
      size_t   m_collectionID;
      /// User data block
      UserData m_userData;
    public:
      /// Standard , initializing constructor
      Geant4SensitiveAction(Geant4Context* context,
                            const std::string& name,
                            Geometry::DetElement det,
                            Geometry::LCDD& lcdd);
      /// Default destructor
      virtual ~Geant4SensitiveAction();
      /// Initialization overload for specialization
      void initialize();
      /// Finalization overload for specialization
      void finalize();
      /// Define collections created by this sensitivie action object
      virtual void defineCollections() {}
      /// G4VSensitiveDetector interface: Method invoked at the begining of each event.
      virtual void begin(G4HCofThisEvent* hce);
      /// G4VSensitiveDetector interface: Method invoked at the end of each event.
      virtual void end(G4HCofThisEvent* hce);
      /// G4VSensitiveDetector interface: Method for generating hit(s) using the G4Step object.
      virtual bool process(G4Step* step,G4TouchableHistory* history);
      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      virtual void clear(G4HCofThisEvent* hce);
    };


  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4RUNACTION_H
