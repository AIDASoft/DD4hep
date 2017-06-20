//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//====================================================================
//
// Define the ROOT dictionaries for all data classes to be saved 
// which are created by the DDG4 examples.
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4Config.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Random.h"
#include "DDG4/DDG4Dict.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4InputAction.h"
#include "DDG4/Geant4GeneratorWrapper.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

#define ACTIONHANDLE(x)                                                                   \
    struct x##Handle  {                                                                   \
      Geant4##x* action;                                                                  \
      explicit x##Handle(Geant4##x* a) : action(a)     { if ( action ) action->addRef();} \
      x##Handle(const x##Handle& h) : action(h.action) { if ( action ) action->addRef();} \
      ~x##Handle()                  { if ( action) action->release();                   } \
      Geant4##x* release()          { Geant4##x* tmp = action; action=0; return tmp;    } \
      operator dd4hep::sim::Geant4##x* () const  { return action;     }            \
      Geant4##x* operator->() const                     { return action;     }            \
      Geant4##x* get() const                            { return action;     }            \
    }

    ACTIONHANDLE(Filter);
    ACTIONHANDLE(Action);
    ACTIONHANDLE(PhaseAction);
    ACTIONHANDLE(RunAction);
    ACTIONHANDLE(EventAction);
    ACTIONHANDLE(GeneratorAction);
    ACTIONHANDLE(PhysicsList);
    ACTIONHANDLE(TrackingAction);
    ACTIONHANDLE(SteppingAction);
    ACTIONHANDLE(StackingAction);
    ACTIONHANDLE(DetectorConstruction);
    ACTIONHANDLE(Sensitive);
    ACTIONHANDLE(ParticleHandler);
    ACTIONHANDLE(UserInitialization);

    ACTIONHANDLE(GeneratorActionSequence);
    ACTIONHANDLE(RunActionSequence);
    ACTIONHANDLE(EventActionSequence);
    ACTIONHANDLE(TrackingActionSequence);
    ACTIONHANDLE(SteppingActionSequence);
    ACTIONHANDLE(StackingActionSequence);
    ACTIONHANDLE(DetectorConstructionSequence);
    ACTIONHANDLE(PhysicsListActionSequence);
    ACTIONHANDLE(SensDetActionSequence);
    ACTIONHANDLE(UserInitializationSequence);

    struct PropertyResult  {
      std::string data;
      int status;
      PropertyResult() : status(0) {}
      PropertyResult(const std::string& d, int s) : data(d), status(s) {}
      PropertyResult(const PropertyResult& c) : data(c.data), status(c.status) {}
      ~PropertyResult() {}
    };

    struct Geant4ActionCreation  {
      template <typename H,typename T> static H cr(KernelHandle& kernel, const std::string& name_type, bool shared)  {
        T action(*kernel.get(),name_type,shared);
        H handle(action.get());
        return handle;
      }
      static ActionHandle createAction(KernelHandle& kernel, const std::string& name_type, bool shared)   
      { return cr<ActionHandle,Setup::Action>(kernel,name_type,shared);                            }
      static FilterHandle createFilter(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<FilterHandle,Setup::Filter>(kernel,name_type,shared);                            }
      static PhaseActionHandle createPhaseAction(KernelHandle& kernel, const std::string& name_type, bool shared)   
      { return cr<PhaseActionHandle,Setup::PhaseAction>(kernel,name_type,shared);                  }
      static PhysicsListHandle createPhysicsList(KernelHandle& kernel, const std::string& name_type)
      { return cr<PhysicsListHandle,Setup::PhysicsList>(kernel,name_type,false);                  }
      static RunActionHandle createRunAction(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<RunActionHandle,Setup::RunAction>(kernel,name_type,shared);                      }
      static EventActionHandle createEventAction(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<EventActionHandle,Setup::EventAction>(kernel,name_type,shared);         }
      static TrackingActionHandle createTrackingAction(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<TrackingActionHandle,Setup::TrackAction>(kernel,name_type,shared);               }
      static SteppingActionHandle createSteppingAction(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<SteppingActionHandle,Setup::StepAction>(kernel,name_type,shared);                }
      static StackingActionHandle createStackingAction(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<StackingActionHandle,Setup::StackAction>(kernel,name_type,shared);               }
      
      static GeneratorActionHandle createGeneratorAction(KernelHandle& kernel, const std::string& name_type, bool shared)
      { return cr<GeneratorActionHandle,Setup::GenAction>(kernel,name_type,shared);                }
      
      static DetectorConstructionHandle createDetectorConstruction(KernelHandle& kernel, const std::string& name_type)
      { return cr<DetectorConstructionHandle,Setup::DetectorConstruction>(kernel,name_type,false); }
      
      static UserInitializationHandle createUserInitialization(KernelHandle& kernel, const std::string& name_type)
      {	return UserInitializationHandle(Setup::Initialization(*kernel.get(),name_type,false).get());}
      
      static SensitiveHandle createSensitive(KernelHandle& kernel, const std::string& name_type, const std::string& detector, bool shared)
      {	return SensitiveHandle(Setup::Sensitive(*kernel.get(),name_type,detector,shared).get());   }
      
      static SensDetActionSequenceHandle createSensDetSequence(KernelHandle& kernel, const std::string& name_type)
      {	return cr<SensDetActionSequenceHandle,Setup::SensitiveSeq>(kernel,name_type,false);       }
      
      static Geant4Action* toAction(Geant4Filter* f)                   { return f;          }
      static Geant4Action* toAction(Geant4Action* f)                   { return f;          }
      static Geant4Action* toAction(Geant4PhaseAction* f)              { return f;          }
      static Geant4Action* toAction(Geant4Sensitive* f)                { return f;          }
      static Geant4Action* toAction(Geant4PhysicsList* f)              { return f;          }
      static Geant4Action* toAction(Geant4RunAction* f)                { return f;          }
      static Geant4Action* toAction(Geant4EventAction* f)              { return f;          }
      static Geant4Action* toAction(Geant4TrackingAction* f)           { return f;          }
      static Geant4Action* toAction(Geant4SteppingAction* f)           { return f;          }
      static Geant4Action* toAction(Geant4StackingAction* f)           { return f;          }
      static Geant4Action* toAction(Geant4GeneratorAction* f)          { return f;          }
      static Geant4Action* toAction(Geant4GeneratorActionSequence* f)  { return f;          }
      static Geant4Action* toAction(Geant4RunActionSequence* f)        { return f;          }
      static Geant4Action* toAction(Geant4EventActionSequence* f)      { return f;          }
      static Geant4Action* toAction(Geant4TrackingActionSequence* f)   { return f;          }
      static Geant4Action* toAction(Geant4SteppingActionSequence* f)   { return f;          }
      static Geant4Action* toAction(Geant4StackingActionSequence* f)   { return f;          }
      static Geant4Action* toAction(Geant4PhysicsListActionSequence* f){ return f;          }
      static Geant4Action* toAction(Geant4SensDetActionSequence* f)    { return f;          }
      static Geant4Action* toAction(Geant4UserInitialization* f)       { return f;          }
      static Geant4Action* toAction(Geant4UserInitializationSequence* f){ return f;         }
      static Geant4Action* toAction(Geant4DetectorConstruction* f)     { return f;          }
      static Geant4Action* toAction(Geant4DetectorConstructionSequence* f){ return f;       }

      static Geant4Action* toAction(FilterHandle f)                    { return f.action;   }
      static Geant4Action* toAction(ActionHandle f)                    { return f.action;   }
      static Geant4Action* toAction(PhaseActionHandle f)               { return f.action;   }
      static Geant4Action* toAction(SensitiveHandle f)                 { return f.action;   }
      static Geant4Action* toAction(PhysicsListHandle f)               { return f.action;   }
      static Geant4Action* toAction(RunActionHandle f)                 { return f.action;   }
      static Geant4Action* toAction(EventActionHandle f)               { return f.action;   }
      static Geant4Action* toAction(TrackingActionHandle f)            { return f.action;   }
      static Geant4Action* toAction(SteppingActionHandle f)            { return f.action;   }
      static Geant4Action* toAction(StackingActionHandle f)            { return f.action;   }
      static Geant4Action* toAction(GeneratorActionHandle f)           { return f.action;   }
      static Geant4Action* toAction(GeneratorActionSequenceHandle f)   { return f.action;   }
      static Geant4Action* toAction(RunActionSequenceHandle f)         { return f.action;   }
      static Geant4Action* toAction(EventActionSequenceHandle f)       { return f.action;   }
      static Geant4Action* toAction(TrackingActionSequenceHandle f)    { return f.action;   }
      static Geant4Action* toAction(SteppingActionSequenceHandle f)    { return f.action;   }
      static Geant4Action* toAction(StackingActionSequenceHandle f)    { return f.action;   }
      static Geant4Action* toAction(PhysicsListActionSequenceHandle f) { return f.action;   }
      static Geant4Action* toAction(SensDetActionSequenceHandle f)     { return f.action;   }
      static Geant4Action* toAction(UserInitializationHandle f)        { return f.action;   }
      static Geant4Action* toAction(UserInitializationSequenceHandle f){ return f.action;   }
      static Geant4Action* toAction(DetectorConstructionHandle f)      { return f.action;   }
      static Geant4Action* toAction(DetectorConstructionSequenceHandle f){ return f.action;   }
      static PropertyResult getProperty(Geant4Action* action, const std::string& name)  {
        if ( action->hasProperty(name) )  {
          return PropertyResult(action->property(name).str(),1);
        }
        return PropertyResult("",0);
      }
      static int setProperty(Geant4Action* action, const std::string& name, const std::string& value)  {
        if ( action->hasProperty(name) )  {
          action->property(name).str(value);
          return 1;
        }
        return 0;
      }
      static PropertyResult getPropertyKernel(Geant4Kernel* kernel, const std::string& name)  {
        if ( kernel->hasProperty(name) )  {
          return PropertyResult(kernel->property(name).str(),1);
        }
        return PropertyResult("",0);
      }
      static int setPropertyKernel(Geant4Kernel* kernel, const std::string& name, const std::string& value)  {
        if ( kernel->hasProperty(name) )  {
          kernel->property(name).str(value);
          return 1;
        }
        return 0;
      }
    };

    /// Container definitions for Geant4Vertex
    typedef vector<Geant4Vertex*>  Geant4VertexVector;
    typedef map<int,Geant4Vertex*> Geant4VertexIntMap;

    /// Container definitions for Geant4Particle
    typedef vector<Geant4Particle*>  Geant4ParticleVector;
    typedef map<int,Geant4Particle*> Geant4ParticleIntMap;
  }
}

typedef dd4hep::sim::Geant4ActionCreation Geant4ActionCreation;

#include "DD4hep/detail/DetectorInterna.h"

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

//#pragma link C++ namespace DDSegmentation;

/// Geant4 Vertex dictionaries
#pragma link C++ class dd4hep::sim::VertexExtension+;
#pragma link C++ class dd4hep::sim::Geant4Vertex+;
#pragma link C++ class vector<dd4hep::sim::Geant4Vertex*>+;
#pragma link C++ class map<int,dd4hep::sim::Geant4Vertex*>+;

#pragma link C++ class dd4hep::sim::Geant4ParticleMap+;
#pragma link C++ class dd4hep::sim::PrimaryExtension+;
#pragma link C++ class dd4hep::sim::Geant4PrimaryInteraction+;
#pragma link C++ class std::vector<dd4hep::sim::Geant4PrimaryInteraction*>+;
#pragma link C++ class std::map<int,dd4hep::sim::Geant4PrimaryInteraction*>+;
#pragma link C++ class dd4hep::sim::Geant4PrimaryEvent+;

#pragma link C++ typedef dd4hep::sim::Geant4VertexVector;
#pragma link C++ typedef dd4hep::sim::Geant4VertexIntMap;

#pragma link C++ typedef dd4hep::sim::Geant4ParticleVector;
#pragma link C++ typedef dd4hep::sim::Geant4ParticleIntMap;

#pragma link C++ class dd4hep::sim::PropertyResult;
#pragma link C++ class dd4hep::sim::Geant4InputAction::Particles;

#pragma link C++ class dd4hep::sim::Geant4ParticleHandle;
#pragma link C++ class dd4hep::sim::Geant4ParticleMap;
#pragma link C++ class std::map<int,dd4hep::sim::Geant4Particle*>;

#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::VertexExtension>+;
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::PrimaryExtension>+;
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::Geant4InputAction::Particles>;
#ifdef DD4HEP_DD4HEP_PTR_AUTO
#pragma link C++ class dd4hep::dd4hep_ptr<dd4hep::sim::Geant4InputAction::Particles>::base_t;
#endif

// Basic stuff
#pragma link C++ class dd4hep::sim::Geant4ActionCreation;
#pragma link C++ class dd4hep::sim::Geant4Kernel;
#pragma link C++ class dd4hep::sim::Geant4Kernel::PhaseSelector;
#pragma link C++ class dd4hep::sim::Geant4Context;
#pragma link C++ class dd4hep::sim::KernelHandle;
#pragma link C++ class dd4hep::sim::Geant4ActionContainer;

#pragma link C++ class dd4hep::sim::Geant4Action;
#pragma link C++ class dd4hep::sim::ActionHandle;

#pragma link C++ class dd4hep::sim::RunActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4RunActionSequence;
#pragma link C++ class dd4hep::sim::RunActionHandle;
#pragma link C++ class dd4hep::sim::Geant4SharedRunAction;
#pragma link C++ class dd4hep::sim::Geant4RunAction;

#pragma link C++ class dd4hep::sim::EventActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4EventActionSequence;
#pragma link C++ class dd4hep::sim::EventActionHandle;
#pragma link C++ class dd4hep::sim::Geant4SharedEventAction;
#pragma link C++ class dd4hep::sim::Geant4EventAction;

#pragma link C++ class dd4hep::sim::SteppingActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4SteppingActionSequence;
#pragma link C++ class dd4hep::sim::SteppingActionHandle;
#pragma link C++ class dd4hep::sim::Geant4SharedSteppingAction;
#pragma link C++ class dd4hep::sim::Geant4SteppingAction;

#pragma link C++ class dd4hep::sim::StackingActionHandle;
#pragma link C++ class dd4hep::sim::Geant4StackingAction;
#pragma link C++ class dd4hep::sim::StackingActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4StackingActionSequence;

#pragma link C++ class dd4hep::sim::TrackingActionHandle;
#pragma link C++ class dd4hep::sim::Geant4TrackingAction;
#pragma link C++ class dd4hep::sim::TrackingActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4TrackingActionSequence;

#pragma link C++ class dd4hep::sim::GeneratorActionHandle;
#pragma link C++ class dd4hep::sim::GeneratorActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4GeneratorActionSequence;
#pragma link C++ class dd4hep::sim::Geant4GeneratorAction;

#pragma link C++ class dd4hep::sim::Geant4InputAction;
#pragma link C++ class dd4hep::sim::Geant4EventReader;

#pragma link C++ class dd4hep::sim::PhysicsListHandle;
#pragma link C++ class dd4hep::sim::Geant4PhysicsList;
#pragma link C++ class dd4hep::sim::PhysicsListActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4PhysicsListActionSequence;

#pragma link C++ class dd4hep::sim::UserInitializationHandle;
#pragma link C++ class dd4hep::sim::UserInitializationSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4UserInitializationSequence;
#pragma link C++ class dd4hep::sim::Geant4UserInitialization;

#pragma link C++ class dd4hep::sim::DetectorConstructionHandle;
#pragma link C++ class dd4hep::sim::DetectorConstructionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4DetectorConstructionSequence;
#pragma link C++ class dd4hep::sim::Geant4DetectorConstruction;

#pragma link C++ class dd4hep::sim::SensitiveHandle;
#pragma link C++ class dd4hep::sim::SensDetActionSequenceHandle;
#pragma link C++ class dd4hep::sim::Geant4ActionSD;
#pragma link C++ class dd4hep::sim::Geant4Sensitive;
#pragma link C++ class dd4hep::sim::Geant4SensDetActionSequence;

#pragma link C++ class dd4hep::sim::FilterHandle;
#pragma link C++ class dd4hep::sim::Geant4Filter;
#pragma link C++ class dd4hep::sim::Geant4GeneratorWrapper;

#pragma link C++ class dd4hep::sim::PhaseActionHandle;
#pragma link C++ class dd4hep::sim::Geant4ActionPhase;
#pragma link C++ class dd4hep::sim::Geant4PhaseAction;

#pragma link C++ class dd4hep::Callback;
#pragma link C++ class dd4hep::Callback::mfunc_t;

// Other (specialized) components
#pragma link C++ class dd4hep::sim::Geant4DataDump;
#pragma link C++ class dd4hep::sim::Geant4Random;
#pragma link C++ class dd4hep::sim::Geant4ParticleHandler;
#pragma link C++ class dd4hep::sim::Geant4UserParticleHandler;


// somehow the symbol Geometry moved into global namespace. Redeclare it here
//namespace detail {}
//#pragma link C++ namespace detail;

#endif

// CLHEP stuff
#include "CLHEP/Random/Random.h"
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

#pragma link C++ namespace CLHEP;
#pragma link C++ class CLHEP::HepRandom;
#pragma link C++ class CLHEP::HepRandomEngine;
#endif
