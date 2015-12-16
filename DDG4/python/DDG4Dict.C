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
//====================================================================
//
// Define the ROOT dictionaries for all data classes to be saved 
// which are created by the DDG4 examples.
//
//  Author     : M.Frank
//
//====================================================================
// FRamework include files
#include "DDG4/Geant4Config.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Random.h"
#include "DDG4/DDG4Dict.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4InputAction.h"

namespace DD4hep {
  namespace Simulation  {

    using std::string;

#define ACTIONHANDLE(x)                                                                   \
    struct x##Handle  {                                                                   \
      Geant4##x* action;                                                                  \
      explicit x##Handle(Geant4##x* a) : action(a)     { if ( action ) action->addRef();} \
      x##Handle(const x##Handle& h) : action(h.action) { if ( action ) action->addRef();} \
      ~x##Handle()                  { if ( action) action->release();                   } \
      Geant4##x* release()          { Geant4##x* tmp = action; action=0; return tmp;    } \
      operator DD4hep::Simulation::Geant4##x* () const  { return action;     }            \
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
      string data;
      int status;
      PropertyResult() : status(0) {}
      PropertyResult(const string& d, int s) : data(d), status(s) {}
      PropertyResult(const PropertyResult& c) : data(c.data), status(c.status) {}
      ~PropertyResult() {}
    };

    struct Geant4ActionCreation  {
      template <typename H,typename T> static H cr(KernelHandle& kernel, const string& name_type, bool shared)  {
        T action(*kernel.get(),name_type,shared);
        H handle(action.get());
        return handle;
      }
      static ActionHandle createAction(KernelHandle& kernel, const string& name_type, bool shared)   
      { return cr<ActionHandle,Setup::Action>(kernel,name_type,shared);                            }
      static FilterHandle createFilter(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<FilterHandle,Setup::Filter>(kernel,name_type,shared);                            }
      static PhaseActionHandle createPhaseAction(KernelHandle& kernel, const string& name_type, bool shared)   
      { return cr<PhaseActionHandle,Setup::PhaseAction>(kernel,name_type,shared);                  }
      static PhysicsListHandle createPhysicsList(KernelHandle& kernel, const string& name_type)
      { return cr<PhysicsListHandle,Setup::PhysicsList>(kernel,name_type,false);                  }
      static RunActionHandle createRunAction(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<RunActionHandle,Setup::RunAction>(kernel,name_type,shared);                      }
      static EventActionHandle createEventAction(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<EventActionHandle,Setup::EventAction>(kernel,name_type,shared);         }
      static TrackingActionHandle createTrackingAction(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<TrackingActionHandle,Setup::TrackAction>(kernel,name_type,shared);               }
      static SteppingActionHandle createSteppingAction(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<SteppingActionHandle,Setup::StepAction>(kernel,name_type,shared);                }
      static StackingActionHandle createStackingAction(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<StackingActionHandle,Setup::StackAction>(kernel,name_type,shared);               }
      
      static GeneratorActionHandle createGeneratorAction(KernelHandle& kernel, const string& name_type, bool shared)
      { return cr<GeneratorActionHandle,Setup::GenAction>(kernel,name_type,shared);                }
      
      static DetectorConstructionHandle createDetectorConstruction(KernelHandle& kernel, const string& name_type)
      { return cr<DetectorConstructionHandle,Setup::DetectorConstruction>(kernel,name_type,false); }
      
      static UserInitializationHandle createUserInitialization(KernelHandle& kernel, const string& name_type)
      {	return UserInitializationHandle(Setup::Initialization(*kernel.get(),name_type,false).get());}
      
      static SensitiveHandle createSensitive(KernelHandle& kernel, const string& name_type, const string& detector, bool shared)
      {	return SensitiveHandle(Setup::Sensitive(*kernel.get(),name_type,detector,shared).get());   }
      
      static SensDetActionSequenceHandle createSensDetSequence(KernelHandle& kernel, const string& name_type)
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
      static PropertyResult getProperty(Geant4Action* action, const string& name)  {
        if ( action->hasProperty(name) )  {
          return PropertyResult(action->property(name).str(),1);
        }
        return PropertyResult("",0);
      }
      static int setProperty(Geant4Action* action, const string& name, const string& value)  {
        if ( action->hasProperty(name) )  {
          action->property(name).str(value);
          return 1;
        }
        return 0;
      }
      static PropertyResult getPropertyKernel(Geant4Kernel* kernel, const string& name)  {
        if ( kernel->hasProperty(name) )  {
          return PropertyResult(kernel->property(name).str(),1);
        }
        return PropertyResult("",0);
      }
      static int setPropertyKernel(Geant4Kernel* kernel, const string& name, const string& value)  {
        if ( kernel->hasProperty(name) )  {
          kernel->property(name).str(value);
          return 1;
        }
        return 0;
      }
    };
  }
}

typedef DD4hep::Simulation::Geant4ActionCreation Geant4ActionCreation;

#include "DD4hep/objects/DetectorInterna.h"
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

//#pragma link C++ namespace DDSegmentation;

/// Geant4 Vertex dictionaries
#pragma link C++ class VertexExtension+;
#pragma link C++ class Geant4Vertex+;
#pragma link C++ class vector<Geant4Vertex*>+;
#pragma link C++ class map<int,Geant4Vertex*>+;

#pragma link C++ class Geant4ParticleMap+;
#pragma link C++ class PrimaryExtension+;
#pragma link C++ class Geant4PrimaryInteraction+;
#pragma link C++ class vector<Geant4PrimaryInteraction*>+;
#pragma link C++ class map<int,Geant4PrimaryInteraction*>+;
#pragma link C++ class Geant4PrimaryEvent+;

#pragma link C++ class PropertyResult;
#pragma link C++ class Geant4InputAction::Particles;

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
namespace {
  struct bla {
    dd4hep_ptr<Geant4InputAction::Particles>  __m1;
    dd4hep_ptr<ParticleExtension>  __m2;
    dd4hep_ptr<DataExtension>  __m3;
  };
}
#else
#pragma link C++ class Geant4ParticleHandle;
#pragma link C++ class Geant4ParticleMap;
#pragma link C++ class map<int,Geant4Particle*>;
#endif

#pragma link C++ class dd4hep_ptr<VertexExtension>+;
#pragma link C++ class dd4hep_ptr<PrimaryExtension>+;
#pragma link C++ class dd4hep_ptr<Geant4InputAction::Particles>;
#pragma link C++ class dd4hep_ptr<Geant4InputAction::Particles>::base_t;

// Basic stuff
#pragma link C++ class Geant4ActionCreation;
#pragma link C++ class Geant4Kernel;
#pragma link C++ class Geant4Kernel::PhaseSelector;
#pragma link C++ class Geant4Context;
#pragma link C++ class KernelHandle;
#pragma link C++ class Geant4ActionContainer;

#pragma link C++ class Geant4Action;
#pragma link C++ class ActionHandle;

#pragma link C++ class RunActionSequenceHandle;
#pragma link C++ class Geant4RunActionSequence;
#pragma link C++ class RunActionHandle;
#pragma link C++ class Geant4SharedRunAction;
#pragma link C++ class Geant4RunAction;

#pragma link C++ class EventActionSequenceHandle;
#pragma link C++ class Geant4EventActionSequence;
#pragma link C++ class EventActionHandle;
#pragma link C++ class Geant4SharedEventAction;
#pragma link C++ class Geant4EventAction;

#pragma link C++ class SteppingActionSequenceHandle;
#pragma link C++ class Geant4SteppingActionSequence;
#pragma link C++ class SteppingActionHandle;
#pragma link C++ class Geant4SharedSteppingAction;
#pragma link C++ class Geant4SteppingAction;

#pragma link C++ class StackingActionHandle;
#pragma link C++ class Geant4StackingAction;
#pragma link C++ class StackingActionSequenceHandle;
#pragma link C++ class Geant4StackingActionSequence;

#pragma link C++ class TrackingActionHandle;
#pragma link C++ class Geant4TrackingAction;
#pragma link C++ class TrackingActionSequenceHandle;
#pragma link C++ class Geant4TrackingActionSequence;

#pragma link C++ class GeneratorActionHandle;
#pragma link C++ class GeneratorActionSequenceHandle;
#pragma link C++ class Geant4GeneratorActionSequence;
#pragma link C++ class Geant4GeneratorAction;

#pragma link C++ class Geant4InputAction;
#pragma link C++ class Geant4EventReader;

#pragma link C++ class PhysicsListHandle;
#pragma link C++ class Geant4PhysicsList;
#pragma link C++ class PhysicsListActionSequenceHandle;
#pragma link C++ class Geant4PhysicsListActionSequence;

#pragma link C++ class UserInitializationHandle;
#pragma link C++ class UserInitializationSequenceHandle;
#pragma link C++ class Geant4UserInitializationSequence;
#pragma link C++ class Geant4UserInitialization;

#pragma link C++ class DetectorConstructionHandle;
#pragma link C++ class DetectorConstructionSequenceHandle;
#pragma link C++ class Geant4DetectorConstructionSequence;
#pragma link C++ class Geant4DetectorConstruction;

#pragma link C++ class SensitiveHandle;
#pragma link C++ class SensDetActionSequenceHandle;
#pragma link C++ class Geant4ActionSD;
#pragma link C++ class Geant4Sensitive;
#pragma link C++ class Geant4SensDetActionSequence;

#pragma link C++ class FilterHandle;
#pragma link C++ class Geant4Filter;

#pragma link C++ class PhaseActionHandle;
#pragma link C++ class Geant4ActionPhase;
#pragma link C++ class Geant4PhaseAction;

#pragma link C++ class Callback;
#pragma link C++ class Callback::mfunc_t;

// Other (specialized) components
#pragma link C++ class Geant4DataDump;
#pragma link C++ class Geant4Random;
#pragma link C++ class Geant4ParticleHandler;
#pragma link C++ class Geant4UserParticleHandler;


// somehow the symbol Geometry moved into global namespace. Redeclare it here
//namespace Geometry {}
//#pragma link C++ namespace Geometry;

#endif

int Geant4Dict()  {
  return 0;
}
