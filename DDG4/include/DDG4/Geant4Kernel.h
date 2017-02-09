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
#ifndef DD4HEP_DDG4_GEANT4KERNEL_H
#define DD4HEP_DDG4_GEANT4KERNEL_H

// Framework include files
#include "DDG4/Geant4ActionContainer.h"

// C/C++ include files
#include <map>
#include <typeinfo>

// Forward declarations
class G4RunManager;
class G4UIdirectory;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4ActionPhase;

    /// Class, which allows all Geant4Action derivatives to access the DDG4 kernel structures.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Kernel : public Geant4ActionContainer  {
    public:
      typedef DD4hep::Geometry::LCDD LCDD;
      typedef std::map<unsigned long, Geant4Kernel*> Workers;
      typedef std::map<std::string, Geant4ActionPhase*> Phases;
      typedef std::map<std::string, Geant4Action*> GlobalActions;
      typedef std::map<std::string,int> ClientOutputLevels;

    protected:
      /// Reference to the run manager
      G4RunManager*      m_runManager;
      /// Top level control directory
      G4UIdirectory*     m_control;
      /// Reference to Geant4 track manager
      G4TrackingManager* m_trackMgr;
      /// Detector description object
      LCDD*              m_lcdd;
      /// Property pool
      PropertyManager    m_properties;


      /// Action phases
      Phases        m_phases;
      /// Worker threads
      Workers       m_workers;
      /// Globally registered actions
      GlobalActions m_globalActions;
      /// Globally registered filters of sensitive detectors
      GlobalActions m_globalFilters;
      /// Property: Name of the G4UI command tree
      std::string m_controlName;
      /// Property: Name of the UI action. Must be member of the global actions
      std::string m_uiName;
      /// Property: Number of events to be executed in batch mode
      long        m_numEvent;
      /// Property: Output level
      int         m_outputLevel;
      /// Property: Client output levels
      ClientOutputLevels m_clientLevels;

      /// Property: Running in multi threaded context
      //bool        m_multiThreaded;
      /// Master property: Number of execution threads in multi threaded mode.
      int         m_numThreads;
      /// Flag: Master instance (id<0) or worker (id >= 0)
      unsigned long      m_id, m_ident;
      /// Parent reference
      Geant4Kernel*      m_master;
      Geant4Kernel*      m_shared;
      Geant4Context*     m_threadContext;

      bool isMaster() const  { return this == m_master; }
      bool isWorker() const  { return this != m_master; }

#ifndef __CINT__
      /// Standard constructor for workers
      Geant4Kernel(Geant4Kernel* m, unsigned long identifier);
#endif

    public:
      /// Standard constructor for the master instance
      Geant4Kernel(LCDD& lcdd);

      /// Thread's master context
      Geant4Kernel& master()  const  { return *m_master; }

      /// Shared action context
      Geant4Kernel& shared()  const  { return *m_shared; }

      //bool isMultiThreaded() const { return m_multiThreaded; }
      bool isMultiThreaded() const { return m_numThreads > 0; }

      /// Access thread identifier
      static unsigned long int thread_self();

    public:

      /// Embedded helper class to facilitate map access to the phases.
      /** @class PhaseSelector Geant4Kernel.h DDG4/Geant4Kernel.h
       *
       * @author  M.Frank
       * @version 1.0
       */
      class PhaseSelector {
      public:
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
#endif
      /// Access phase phases
      const Phases& phases() const              {        return m_phases;          }
      /// Access to detector description
      LCDD& lcdd() const                        {        return *m_lcdd;           }
      /// Access the tracking manager
      G4TrackingManager* trackMgr() const       {        return m_trackMgr;        }
      /// Access the tracking manager
      void setTrackMgr(G4TrackingManager* mgr)  {        m_trackMgr = mgr;         }
      /// Access the command directory
      const std::string& directoryName() const  {        return m_controlName;     }
      /// Access worker identifier
      unsigned long id()  const                 {        return m_ident;           }
      /// Access to the Geant4 run manager
      G4RunManager& runManager();

      /** Property access                            */
      /// Access to the properties of the object
      PropertyManager& properties()             {        return m_properties;      }
      /// Print the property values
      void printProperties() const;
      /// Declare property
      template <typename T> Geant4Kernel& declareProperty(const std::string& nam, T& val);
      /// Declare property
      template <typename T> Geant4Kernel& declareProperty(const char* nam, T& val);
      /// Check property for existence
      bool hasProperty(const std::string& name) const;
      /// Access single property
      Property& property(const std::string& name);

      /** Output level settings                       */
      /// Access the output level
      PrintLevel outputLevel() const  {        return (PrintLevel)m_outputLevel;    }
      /// Set the global output level of the kernel object; returns previous value
      PrintLevel setOutputLevel(PrintLevel new_level);
      /// Fill cache with the global output level of a named object. Must be set before instantiation
      void setOutputLevel(const std::string object, PrintLevel new_level);
      /// Retrieve the global output level of a named object.
      PrintLevel getOutputLevel(const std::string object) const;

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
      virtual Geant4ActionPhase* addSimplePhase(const std::string& name, bool throw_on_exist);

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

      /// Execute phase action if it exists
      virtual bool executePhase(const std::string& name, const void** args)  const;

      /// Construct detector geometry using lcdd plugin
      virtual void loadGeometry(const std::string& compact_file);
      /// Load XML file 
      virtual void loadXML(const char* fname);

      /** Geant4 Multi threading support */
      /// Create identified worker instance
      virtual Geant4Kernel& createWorker();
      /// Access worker instance by it's identifier
      Geant4Kernel& worker(unsigned long thread_identifier, bool create_if=false);
      /// Access number of workers
      int numWorkers() const;

      /// Run the simulation: Configure Geant4
      virtual int configure();
      /// Run the simulation: Initialize Geant4
      virtual int initialize();
      /// Run the simulation: Simulate the number of events given by the property "NumEvents"
      virtual int run();
      /// Run the simulation: Simulate the number of events "num_events" and modify the property "NumEvents"
      virtual int runEvents(int num_events);
      /// Run the simulation: Terminate Geant4
      virtual int terminate();
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

    /// Main executor steering the Geant4 execution
    class Geant4Exec {
    public:
      /// Configure the application
      static int configure(Geant4Kernel& kernel);
      /// Initialize the application
      static int initialize(Geant4Kernel& kernel);
      /// Run the application and simulate events
      static int run(Geant4Kernel& kernel);
      /// Terminate the application
      static int terminate(Geant4Kernel& kernel);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4KERNEL_H
