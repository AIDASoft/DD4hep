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
#include "DDG4/Geant4Data.h"
#include <vector>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

// CINT configuration
#if defined(__MAKECINT__)
//#pragma link C++ class Position+;
//#pragma link C++ class Direction+;
#pragma link C++ class SimpleRun+;
#pragma link C++ class SimpleEvent+;
//#pragma link C++ class SimpleEvent::Seeds+;
#pragma link C++ class SimpleHit+;
#pragma link C++ class std::vector<SimpleHit*>+;
#pragma link C++ class SimpleHit::Contribution+;
#pragma link C++ class SimpleHit::Contributions+;
#pragma link C++ class SimpleTracker+;
#pragma link C++ class SimpleTracker::Hit+;
#pragma link C++ class std::vector<SimpleTracker::Hit*>+;
#pragma link C++ class SimpleCalorimeter+;
#pragma link C++ class SimpleCalorimeter::Hit+;
#pragma link C++ class std::vector<SimpleCalorimeter::Hit*>+;
//#pragma link C++ class ;
#endif

#include "DDG4/Geant4Config.h"
#include <iostream>

namespace DD4hep {
  namespace Simulation  {

#define ACTIONHANDLE(x)    \
    struct x##Handle  {    \
      Geant4##x* action;   \
      x##Handle(Geant4##x* a)       : action(a)        { if ( action ) action->addRef();} \
      x##Handle(const x##Handle& h) : action(h.action) { if ( action ) action->addRef();} \
      ~x##Handle()                  { if ( action) action->release(); }  \
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
    };
  }
}

typedef DD4hep::Simulation::Geant4ActionCreation Geant4ActionCreation;

// CINT configuration for DDG4
#if defined(__MAKECINT__)
#pragma link C++ class PropertyResult;

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


#pragma link C++ class Geant4ActionCreation;
#pragma link C++ class Geant4Action;
#pragma link C++ class Geant4Kernel;
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

#pragma link C++ class Geant4PhysicsListActionSequence;
#pragma link C++ class Geant4PhysicsList;

#pragma link C++ class Geant4Filter;
#pragma link C++ class Geant4ActionSD;
#pragma link C++ class Geant4Sensitive;
#pragma link C++ class Geant4SensDetActionSequence;

#pragma link C++ class Geant4ActionPhase;

#endif


using namespace DD4hep::Geometry;

// CINT configuration for DD4hep
#if defined(__MAKECINT__)

//#pragma link C++ class Handle<TObject>;
#pragma link C++ class Handle<TNamed>;
#pragma link C++ class LCDD::HandleMap;
#pragma link C++ class LCDD::HandleMap::iterator;
#pragma link C++ class LCDD::HandleMap::const_iterator;
#pragma link C++ class LCDD::HandleMap::key_type;
#pragma link C++ class LCDD::HandleMap::value_type;
#pragma link C++ class LCDD::PropertyValues;
#pragma link C++ class LCDD::Properties;
#pragma link C++ class LCDD;
#pragma link C++ class VolumeManager;
#pragma link C++ class OverlayedField;

// Objects.h
#pragma link C++ class Author;
#pragma link C++ class Header;
#pragma link C++ class Constant;
#pragma link C++ class Atom;
#pragma link C++ class Material;
#pragma link C++ class VisAttr;
#pragma link C++ class AlignmentEntry;
#pragma link C++ class Limit;
#pragma link C++ class LimitSet;
#pragma link C++ class Region;

// Readout.h
#pragma link C++ class Readout;
#pragma link C++ class Alignment;
#pragma link C++ class Conditions;

// DetElement.h
#pragma link C++ class DetElement;
#pragma link C++ class SensitiveDetector;

// Volume.h
#pragma link C++ class Volume;
#pragma link C++ class PlacedVolume;

// Shapes.h
#pragma link C++ class Polycone;
#pragma link C++ class Solid_type<TGeoPcon>;
#pragma link C++ class Handle<TGeoPcon>;

#pragma link C++ class ConeSegment;
#pragma link C++ class Solid_type<TGeoConeSeg>;
#pragma link C++ class Handle<TGeoConeSeg>;

#pragma link C++ class Box;
#pragma link C++ class Solid_type<TGeoBBox>;
#pragma link C++ class Handle<TGeoBBox>;

#pragma link C++ class Torus;
#pragma link C++ class Solid_type<TGeoTorus>;
#pragma link C++ class Handle<TGeoTorus>;

#pragma link C++ class Cone;
#pragma link C++ class Solid_type<TGeoCone>;
#pragma link C++ class Handle<TGeoCone>;

#pragma link C++ class Tube;
#pragma link C++ class Solid_type<TGeoTubeSeg>;
#pragma link C++ class Handle<TGeoTubeSeg>;

#pragma link C++ class Trap;
#pragma link C++ class Solid_type<TGeoTrap>;
#pragma link C++ class Handle<TGeoTrap>;

#pragma link C++ class Trapezoid;
#pragma link C++ class Solid_type<TGeoTrd2>;
#pragma link C++ class Handle<TGeoTrd2>;

#pragma link C++ class Sphere;
#pragma link C++ class Solid_type<TGeoSphere>;
#pragma link C++ class Handle<TGeoSphere>;

#pragma link C++ class Paraboloid;
#pragma link C++ class Solid_type<TGeoParaboloid>;
#pragma link C++ class Handle<TGeoParaboloid>;

#pragma link C++ class PolyhedraRegular;
#pragma link C++ class Solid_type<TGeoPgon>;
#pragma link C++ class Handle<TGeoPgon>;

#pragma link C++ class BooleanSolid;
#pragma link C++ class Solid_type<TGeoCompositeShape>;
#pragma link C++ class Handle<TGeoCompositeShape>;

#pragma link C++ class SubtractionSolid;
#pragma link C++ class UnionSolid;
#pragma link C++ class IntersectionSolid;


//#pragma link C++ class Solid_type<>;
//#pragma link C++ class Handle<>;
#endif



int Geant4Dict()  {
  return 0;
}
