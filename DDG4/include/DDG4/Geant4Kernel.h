// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4KERNEL_H
#define DD4HEP_DDG4_GEANT4KERNEL_H

// Framework include files
#include "DDG4/Geant4Primitives.h"
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <map>
#include <string>
#include <typeinfo>

// Forward declarations
class G4RunManager;
class G4UIdirectory;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4ActionPhase;
    class Geant4Context;
    class Geant4Action;
    class Geant4RunAction;
    class Geant4EventAction;
    class Geant4SteppingAction;
    class Geant4TrackingAction;
    class Geant4StackingAction;
    class Geant4GeneratorAction;
    class Geant4PhysicsList;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4GeneratorActionSequence;
    class Geant4PhysicsListActionSequence;
    class Geant4SensDetActionSequence;
    class Geant4SensDetSequences;
    class Geant4MonteCarloTruth;
    class Geant4MonteCarloRecordManager;

    /** @class Invoke Geant4Kernel.h DDG4/Geant4Kernel.h
     *
     * Default base class for all geant 4 actions and derivates thereof.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Kernel {
    public:
      typedef DD4hep::Geometry::LCDD LCDD;
      typedef std::map<std::string, Geant4ActionPhase*> Phases;
      typedef std::map<std::string, Geant4Action*> GlobalActions;

    protected:
      /// Reference to execution context
      Geant4Context* m_context;
      /// Reference to the run manager
      G4RunManager* m_runManager;
      /// Top level control directory
      G4UIdirectory* m_control;
      /// Property pool
      PropertyManager m_properties;
      /// Reference to the Geant4 primary generator action
      Geant4GeneratorActionSequence* m_generatorAction;
      /// Reference to the Geant4 run action
      Geant4RunActionSequence* m_runAction;
      /// Reference to the Geant4 event action
      Geant4EventActionSequence* m_eventAction;
      /// Reference to the Geant4 track action
      Geant4TrackingActionSequence* m_trackingAction;
      /// Reference to the Geant4 step action
      Geant4SteppingActionSequence* m_steppingAction;
      /// Reference to the Geant4 stacking action
      Geant4StackingActionSequence* m_stackingAction;
      /// Reference to the Geant4 sensitive action sequences
      Geant4SensDetSequences* m_sensDetActions;
      /// Reference to the geant4 physics list
      Geant4PhysicsListActionSequence* m_physicsList;
      /// Reference to track persistency manager
      Geant4MonteCarloTruth* m_mcTruthMgr;
      /// Reference to MC record manager
      Geant4MonteCarloRecordManager* m_mcRecordMgr;
      /// Reference to Geant4 track manager
      G4TrackingManager* m_trackMgr;

      /// Action phases
      Phases m_phases;
      /// Globally registered actions
      GlobalActions m_globalActions;
      /// Globally registered filters of sensitive detectors
      GlobalActions m_globalFilters;
      /// Detector description object
      LCDD& m_lcdd;
      /// Property: Name of the G4UI command tree
      std::string m_controlName;
      /// Property: Name of the UI action. Must be member of the global actions
      std::string m_uiName;
      /// Property: Number of events to be executed in batch mode
      long        m_numEvent;
      /// Helper to register an action sequence
      template <typename C> bool registerSequence(C*& seq, const std::string& name);

      /// Standard constructor
      Geant4Kernel(LCDD& lcdd);
    public:

      /** @class PhaseSelector Geant4Kernel.h DDG4/Geant4Kernel.h
       *
       * Embedded helper class to facilitate map access to the phases.
       *
       * @author  M.Frank
       * @version 1.0
       */
      struct PhaseSelector {
        /// Reference to embedding object
        Geant4Kernel* m_kernel;
        /// Standard constructor
        PhaseSelector(Geant4Kernel* kernel);
        /// Copy constructor
        PhaseSelector(const PhaseSelector& c);
	/// Assignment operator
	PhaseSelector& operator=(const PhaseSelector& c);
        /// Phase access to the map
        Geant4ActionPhase& operator[](const std::string& name) const;
      } phase;

      enum State {
        SETTING_UP, INITIALIZED
      };
      /// Default destructor
      virtual ~Geant4Kernel();
#ifndef __CINT__
      /// Instance accessor
      static Geant4Kernel& instance(LCDD& lcdd);
      /// Accessof the Geant4Kernel object from the LCDD reference extension (if present and registered)
      static Geant4Kernel& access(LCDD& lcdd);
#endif
      /// Access the context
      Geant4Context* context() const {
        return m_context;
      }
      /// Automatic conversion to the context
      operator Geant4Context*() const {
        return m_context;
      }
      /// Access to the properties of the object
      PropertyManager& properties() {
        return m_properties;
      }
      /// Access phase phases
      const Phases& phases() const {
        return m_phases;
      }
      /// Access to detector description
      LCDD& lcdd() const {
        return m_lcdd;
      }
      /// Access the tracking manager
      G4TrackingManager* trackMgr() const  {
	return m_trackMgr;
      }
      /// Access the tracking manager
      void setTrackMgr(G4TrackingManager* mgr)  {
	m_trackMgr = mgr;
      }
      /// Access to the Geant4 run manager
      G4RunManager& runManager();
      /// Access the command directory
      const std::string& directoryName() const {
        return m_controlName;
      }
      /// Declare property
      template <typename T> Geant4Kernel& declareProperty(const std::string& nam, T& val);
      /// Declare property
      template <typename T> Geant4Kernel& declareProperty(const char* nam, T& val);
      /// Check property for existence
      bool hasProperty(const std::string& name) const;
      /// Access single property
      Property& property(const std::string& name);

      /// Register action by name to be retrieved when setting up and connecting action objects
      /** Note: registered actions MUST be unique.
       *  However, not all actions need to registered....
       *  Only register those, you later need to retrieve by name.
       */
      Geant4Kernel& registerGlobalAction(Geant4Action* action);
      /// Retrieve action from repository
      Geant4Action* globalAction(const std::string& action_name, bool throw_if_not_present = true);
      /// Register filter by name to be retrieved when setting up and connecting filter objects
      /** Note: registered filters MUST be unique.
       *  However, not all filters need to registered....
       *  Only register those, you later need to retrieve by name.
       */
      Geant4Kernel& registerGlobalFilter(Geant4Action* filter);
      /// Retrieve filter from repository
      Geant4Action* globalFilter(const std::string& filter_name, bool throw_if_not_present = true);

      /// Access phase by name
      Geant4ActionPhase* getPhase(const std::string& name);
      /// Add a new phase to the phase
      virtual Geant4ActionPhase* addPhase(const std::string& name, const std::type_info& arg1, const std::type_info& arg2,
          const std::type_info& arg3, bool throw_on_exist);
      /// Add a new phase to the phase
      template <typename A0> Geant4ActionPhase* addPhase(const std::string& name, bool throw_on_exist = true) {
        return addPhase(name, typeid(A0), typeid(void), typeid(void), throw_on_exist);
      }
      /// Add a new phase to the phase
      template <typename A0, typename A1>
      Geant4ActionPhase* addPhase(const std::string& name, bool throw_on_exist = true) {
        return addPhase(name, typeid(A0), typeid(A1), typeid(void), throw_on_exist);
      }
      /// Add a new phase to the phase
      template <typename A0, typename A1, typename A2>
      Geant4ActionPhase* addPhase(const std::string& name, bool throw_on_exist = true) {
        return addPhase(name, typeid(A0), typeid(A1), typeid(A2), throw_on_exist);
      }
      /// Remove an existing phase from the phase. If not existing returns false
      virtual bool removePhase(const std::string& name);
      /// Destroy all phases. To be called only at shutdown.
      virtual void destroyPhases();

      /// Access generator action sequence
      Geant4GeneratorActionSequence* generatorAction(bool create);
      /// Access generator action sequence
      Geant4GeneratorActionSequence& generatorAction() {
        return *generatorAction(true);
      }

      /// Access run action sequence
      Geant4RunActionSequence* runAction(bool create);
      /// Access run action sequence
      Geant4RunActionSequence& runAction() {
        return *runAction(true);
      }

      /// Access run action sequence
      Geant4EventActionSequence* eventAction(bool create);
      /// Access run action sequence
      Geant4EventActionSequence& eventAction() {
        return *eventAction(true);
      }

      /// Access stepping action sequence
      Geant4SteppingActionSequence* steppingAction(bool create);
      /// Access stepping action sequence
      Geant4SteppingActionSequence& steppingAction() {
        return *steppingAction(true);
      }

      /// Access tracking action sequence
      Geant4TrackingActionSequence* trackingAction(bool create);
      /// Access tracking action sequence
      Geant4TrackingActionSequence& trackingAction() {
        return *trackingAction(true);
      }

      /// Access stacking action sequence
      Geant4StackingActionSequence* stackingAction(bool create);
      /// Access stacking action sequence
      Geant4StackingActionSequence& stackingAction() {
        return *stackingAction(true);
      }

      /// Access to the sensitive detector sequences from the kernel object
      Geant4SensDetSequences& sensitiveActions() const;
      /// Access to the sensitive detector action from the kernel object
      Geant4SensDetActionSequence* sensitiveAction(const std::string& name) const;

      /// Access to the physics list
      Geant4PhysicsListActionSequence* physicsList(bool create);
      /// Access to the physics list
      Geant4PhysicsListActionSequence& physicsList() {
        return *physicsList(true);
      }
      /// Access to the Track Manager from the kernel object
      Geant4MonteCarloTruth& mcTruthMgr();
      /// Access to the MC record manager from the kernel object (if instantiated!)
      Geant4MonteCarloRecordManager& mcRecordMgr();

      /// Construct detector geometry using lcdd plugin
      void loadGeometry(const std::string& compact_file);
      /// Run the simulation
      void configure();
      void initialize();
      void run();
      void terminate();
      void loadXML(const char* fname);
    };
    /// Declare property
    template <typename T> Geant4Kernel& Geant4Kernel::declareProperty(const std::string& nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }

    /// Declare property
    template <typename T> Geant4Kernel& Geant4Kernel::declareProperty(const char* nam, T& val) {
      m_properties.add(nam, val);
      return *this;
    }
 
    struct Geant4Exec {
      static int configure(Geant4Kernel& kernel);
      static int initialize(Geant4Kernel& kernel);
      static int run(Geant4Kernel& kernel);
      static int terminate(Geant4Kernel& kernel);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4KERNEL_H
