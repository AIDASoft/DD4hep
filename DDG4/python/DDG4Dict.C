// $Id: Geant4Data.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Define the ROOT dictionaries for all data classes to be saved 
//  which are created by the DDG4 examples.
//
//  Author     : M.Frank
//
//====================================================================
// FRamework include files
#include "DDG4/Geant4Primary.h"
#include "DDG4/DDG4Dict.h"

// CINT configuration
#if defined(__MAKECINT__)
#pragma link C++ namespace DD4hep::DDSegmentation;

/// Geant4 Vertex dictionaries
#pragma link C++ class DD4hep::Simulation::VertexExtension+;
#pragma link C++ class std::auto_ptr<DD4hep::Simulation::VertexExtension>+;
#pragma link C++ class std::auto_ptr<DD4hep::Simulation::PrimaryExtension>+;
#pragma link C++ class DD4hep::Simulation::Geant4Vertex+;
#pragma link C++ class std::vector<DD4hep::Simulation::Geant4Vertex*>+;
#pragma link C++ class std::map<int,DD4hep::Simulation::Geant4Vertex*>+;

#pragma link C++ class DD4hep::Simulation::Geant4ParticleMap+;
#pragma link C++ class DD4hep::Simulation::PrimaryExtension+;
#pragma link C++ class DD4hep::Simulation::Geant4PrimaryInteraction+;
#pragma link C++ class std::map<int,DD4hep::Simulation::Geant4PrimaryInteraction*>+;
#pragma link C++ class DD4hep::Simulation::Geant4PrimaryEvent+;

#endif

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

#include "DDG4/Geant4Config.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4InputAction.h"
#include <iostream>

namespace DD4hep {
  namespace Simulation  {

#define ACTIONHANDLE(x)    \
    struct x##Handle  {    \
      Geant4##x* action;   \
      x##Handle(Geant4##x* a)       : action(a)        { if ( action ) action->addRef();} \
      x##Handle(const x##Handle& h) : action(h.action) { if ( action ) action->addRef();} \
      ~x##Handle()                  { if ( action) action->release();                   } \
      Geant4##x* release()          { Geant4##x* tmp = action; action=0; return tmp;    } \
      operator Geant4##x* () const  { return action;     }  \
      Geant4##x* operator->() const { return action;     }  \
      Geant4##x* get() const        { return action;     }  \
      }

    ACTIONHANDLE(Filter);
    ACTIONHANDLE(Action);
    ACTIONHANDLE(RunAction);
    ACTIONHANDLE(EventAction);
    ACTIONHANDLE(GeneratorAction);
    ACTIONHANDLE(PhysicsList);
    ACTIONHANDLE(TrackingAction);
    ACTIONHANDLE(SteppingAction);
    ACTIONHANDLE(StackingAction);
    ACTIONHANDLE(Sensitive);
    ACTIONHANDLE(ParticleHandler);

    ACTIONHANDLE(GeneratorActionSequence);
    ACTIONHANDLE(RunActionSequence);
    ACTIONHANDLE(EventActionSequence);
    ACTIONHANDLE(TrackingActionSequence);
    ACTIONHANDLE(SteppingActionSequence);
    ACTIONHANDLE(StackingActionSequence);
    ACTIONHANDLE(PhysicsListActionSequence);
    ACTIONHANDLE(SensDetActionSequence);

    struct PropertyResult  {
      string data;
      int status;
      PropertyResult() : status(0) {}
      PropertyResult(const string& d, int s) : data(d), status(s) {}
      PropertyResult(const PropertyResult& c) : data(c.data), status(c.status) {}
      ~PropertyResult() {}
    };

    struct Geant4ActionCreation  {
      template <typename H,typename T> static H cr(KernelHandle& kernel, const string& name_type)  {
	T action(*kernel.get(),name_type);
	H handle(action.get());
	return handle;
      }
      static ActionHandle createAction(KernelHandle& kernel, const string& name_type)   
      { return cr<ActionHandle,Setup::Action>(kernel,name_type);                            }
      static FilterHandle createFilter(KernelHandle& kernel, const string& name_type)
      { return cr<FilterHandle,Setup::Filter>(kernel,name_type);                            }
      static PhysicsListHandle createPhysicsList(KernelHandle& kernel, const string& name_type)
      { return cr<PhysicsListHandle,Setup::PhysicsList>(kernel,name_type);                  }
      static RunActionHandle createRunAction(KernelHandle& kernel, const string& name_type)
      { return cr<RunActionHandle,Setup::RunAction>(kernel,name_type);                      }
      static EventActionHandle createEventAction(KernelHandle& kernel, const string& name_type)
      { return cr<EventActionHandle,Setup::EventAction>(kernel,name_type);                  }
      static TrackingActionHandle createTrackingAction(KernelHandle& kernel, const string& name_type)
      { return cr<TrackingActionHandle,Setup::TrackAction>(kernel,name_type);               }
      static SteppingActionHandle createSteppingAction(KernelHandle& kernel, const string& name_type)
      { return cr<SteppingActionHandle,Setup::StepAction>(kernel,name_type);                }
      static StackingActionHandle createStackingAction(KernelHandle& kernel, const string& name_type)
      { return cr<StackingActionHandle,Setup::StackAction>(kernel,name_type);               }
      static GeneratorActionHandle createGeneratorAction(KernelHandle& kernel, const string& name_type)
      { return cr<GeneratorActionHandle,Setup::GenAction>(kernel,name_type);                }
      static SensitiveHandle createSensitive(KernelHandle& kernel, const string& name_type, const string& detector)
      {	return SensitiveHandle(Setup::Sensitive(*kernel.get(),name_type,detector).get());   }
      static SensDetActionSequenceHandle createSensDetSequence(KernelHandle& kernel, const string& name_type)
      {	return cr<SensDetActionSequenceHandle,Setup::SensitiveSeq>(kernel,name_type);       }

      static Geant4Action* toAction(Geant4Filter* f)                   { return f;          }
      static Geant4Action* toAction(Geant4Action* f)                   { return f;          }
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

      static Geant4Action* toAction(FilterHandle f)                    { return f.action;   }
      static Geant4Action* toAction(ActionHandle f)                    { return f.action;   }
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
// CINT configuration for DDG4
#if defined(__MAKECINT__)
#pragma link C++ class PropertyResult;
#pragma link C++ class Geant4InputAction::Particles;
#pragma link C++ class auto_ptr<Geant4InputAction::Particles>;

#pragma link C++ class ActionHandle;
#pragma link C++ class FilterHandle;
#pragma link C++ class RunActionHandle;
#pragma link C++ class GeneratorActionHandle;
#pragma link C++ class EventActionHandle;
#pragma link C++ class PhysicsListHandle;
#pragma link C++ class TrackingActionHandle;
#pragma link C++ class SteppingActionHandle;
#pragma link C++ class StackingActionHandle;
#pragma link C++ class SensitiveHandle;
#pragma link C++ class GeneratorActionSequenceHandle;
#pragma link C++ class RunActionSequenceHandle;
#pragma link C++ class EventActionSequenceHandle;
#pragma link C++ class TrackingActionSequenceHandle;
#pragma link C++ class SteppingActionSequenceHandle;
#pragma link C++ class StackingActionSequenceHandle;
#pragma link C++ class PhysicsListActionSequenceHandle;
#pragma link C++ class SensDetActionSequenceHandle;

#pragma link C++ class Geant4DataDump;

#pragma link C++ class Geant4ActionCreation;
#pragma link C++ class Geant4Action;
#pragma link C++ class Geant4Kernel;
#pragma link C++ class Geant4Kernel::PhaseSelector;
#pragma link C++ class Geant4Context;
#pragma link C++ class KernelHandle;

#pragma link C++ class Geant4RunActionSequence;
#pragma link C++ class Geant4RunAction;

#pragma link C++ class Geant4EventActionSequence;
#pragma link C++ class Geant4EventAction;

#pragma link C++ class Geant4SteppingActionSequence;
#pragma link C++ class Geant4SteppingAction;

#pragma link C++ class Geant4StackingActionSequence;
#pragma link C++ class Geant4StackingAction;

#pragma link C++ class Geant4TrackingActionSequence;
#pragma link C++ class Geant4TrackingAction;

#pragma link C++ class Geant4GeneratorActionSequence;
#pragma link C++ class Geant4GeneratorAction;
#pragma link C++ class Geant4InputAction;
#pragma link C++ class Geant4EventReader;

#pragma link C++ class Geant4PhysicsListActionSequence;
#pragma link C++ class Geant4PhysicsList;

#pragma link C++ class Geant4ParticleHandler;
#pragma link C++ class Geant4UserParticleHandler;
#pragma link C++ class Geant4Filter;
#pragma link C++ class Geant4ActionSD;
#pragma link C++ class Geant4Sensitive;
#pragma link C++ class Geant4SensDetActionSequence;
#pragma link C++ class Geant4ActionPhase;

// Work around CINT bug: 
// somehow the symbol Geometry moved into global namespace. Redeclare it here
namespace Geometry {}
#pragma link C++ namespace Geometry;

#endif

int Geant4Dict()  {
  return 0;
}
