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

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Woverlength-strings"

#elif defined(__llvm__) || defined(__APPLE__)

#pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wunused"
#pragma clang diagnostic ignored "-Woverlength-strings"
#endif

#include <DDDigi/DigiData.h>
#include <DDDigi/DigiHandle.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiAction.h>
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiContainerProcessor.h>

struct DDDigiDict  {};

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <typename VAL>
    int add_action_property(DigiAction* action, const std::string& name, VAL value);

#define ACTIONHANDLE(x)                                                                   \
    struct x##Handle  {                                                                   \
      Digi##x* action;                                                                    \
      explicit x##Handle(Digi##x* a) : action(a)       { if ( action ) action->addRef();} \
      x##Handle(const x##Handle& h) : action(h.action) { if ( action ) action->addRef();} \
      ~x##Handle()                                     { if ( action) action->release();} \
      Digi##x* release()          { Digi##x* tmp = action; action=0; return tmp;        } \
      operator dd4hep::digi::Digi##x* () const         { return action;                 } \
      Digi##x* operator->() const                      { return action;                 } \
      Digi##x* get() const                             { return action;                 } \
      Digi##x* I_am_a_ROOT_interface_handle() const    { return action;                 } \
      KernelHandle kernel()  const                     {		\
	auto* k = const_cast<DigiKernel*>(action->kernel());		\
	return KernelHandle(k);						\
      }									\
    }

    ACTIONHANDLE(Action);

    struct PropertyResult  {
      std::string data;
      int status;
      PropertyResult() : status(0) {}
      PropertyResult(const std::string& d, int s) : data(d), status(s) {}
      PropertyResult(const PropertyResult& c) : data(c.data), status(c.status) {}
      ~PropertyResult() {}
    };

    struct DigiActionCreation  {
      template <typename H,typename T> static
      H cr(KernelHandle& kernel, const std::string& name_type)  {
        T action(*kernel.get(),name_type);
        H handle(action.get());
        return handle;
      }
      template <typename T,typename H> static H* cst(T* in)  {
	auto* out = dynamic_cast<H*>(in);
	if ( out ) return out;
	if ( in )
	  except("DigiAction","Invalid cast of action '%s' [type:%s] to type %s!",
		 in->c_name(), typeName(typeid(T)).c_str(), typeName(typeid(H)).c_str());
	except("DigiAction","Invalid cast of NULL [type:%s] to type %s!",
	       typeName(typeid(T)).c_str(), typeName(typeid(H)).c_str());
	return nullptr;
      }

      static PropertyResult getProperty(DigiAction* action, const std::string& name)  {
        if ( action->hasProperty(name) )  {
          return PropertyResult(action->property(name).str(),1);
        }
        return PropertyResult("",0);
      }
      static int setProperty(DigiAction* action, const std::string& name, const std::string& value)  {
        if ( action->hasProperty(name) )  {
          action->property(name).str(value);
          return 1;
        }
        return 0;
      }

#define MKVAL auto val = value

#define ADD_PROPERTY(n,X)						\
      static int add##n       (DigiAction* action, const std::string& name, const X value) \
      {	return add_action_property(action, name, value); }		\
      static int addVector##n (DigiAction* action, const std::string& name, std::vector<X> value) \
      {	MKVAL; return add_action_property(action, name, val); }	\
      static int addList##n   (DigiAction* action, const std::string& name, std::list<X> value) \
      {	MKVAL; return add_action_property(action, name, val); }		\
      static int addSet##n    (DigiAction* action, const std::string& name, std::set<X> value) \
      {	MKVAL; return add_action_property(action, name, val); }		\
      static int addMapped##n (DigiAction* action, const std::string& name, std::map<std::string,X> value) \
      {	MKVAL; return add_action_property(action, name, val); }
      ADD_PROPERTY(Property,int)
      ADD_PROPERTY(Property,short)
      ADD_PROPERTY(Property,size_t)
      ADD_PROPERTY(Property,double)

      ADD_PROPERTY(Property,std::string)

      static int addPositionProperty(DigiAction* action, const std::string& name, const std::string value)     {
	Position pos;
	Property pr(pos);
	pr.str(value);
	return add_action_property(action, name, pos);
      }
      //ADD_PROPERTY(PositionProperty,dd4hep::Position)

      static PropertyResult getPropertyKernel(DigiKernel* kernel, const std::string& name)  {
        if ( kernel->hasProperty(name) )  {
          return PropertyResult(kernel->property(name).str(),1);
        }
        return PropertyResult("",0);
      }
      static int setPropertyKernel(DigiKernel* kernel, const std::string& name, const std::string& value)  {
        if ( kernel->hasProperty(name) )  {
          kernel->property(name).str(value);
          return 1;
        }
        return 0;
      }

      static KernelHandle createKernel(DigiAction* action)   {
	auto* k = const_cast<DigiKernel*>(action->kernel());
	return KernelHandle(k);
      }
      static ActionHandle createAction(KernelHandle& kernel, const std::string& name_type)   
      { return cr<ActionHandle,DigiHandle<DigiAction> >(kernel,name_type);                           }
      static DigiAction* toAction(DigiAction* f)                   { return f;                       }
      static DigiAction* toAction(ActionHandle f)                  { return f.action;                }

      static DigiEventAction* toEventAction(DigiAction* a)        { return cst<DigiAction,DigiEventAction>(a); }
      static DigiContainerProcessor* toContainerProcessor(DigiAction* a) { return cst<DigiAction,DigiContainerProcessor>(a); }

#if 0
      static DigiEventAction* toEventAction(DigiEventAction* a)   { return a; }
      ACTIONHANDLE(SignalProcessor);
      ACTIONHANDLE(EventAction);
      ACTIONHANDLE(InputAction);
      ACTIONHANDLE(SegmentProcessor);
      ACTIONHANDLE(ContainerProcessor);
      ACTIONHANDLE(ActionSequence);
      ACTIONHANDLE(Synchronize);

      static EventActionHandle createEventAction(KernelHandle& kernel, const std::string& name_type)   
      { return cr<EventActionHandle,DigiHandle<DigiEventAction> >(kernel,name_type);                 }

      static InputActionHandle createInputAction(KernelHandle& kernel, const std::string& name_type)   
      { return cr<InputActionHandle,DigiHandle<DigiInputAction> >(kernel,name_type);                 }

      static SegmentProcessorHandle createSegmentProcessor(KernelHandle& kernel, const std::string& name_type)   
      { return cr<SegmentProcessorHandle,DigiHandle<DigiSegmentProcessor> >(kernel,name_type);              }

      static ActionSequenceHandle createSequence(KernelHandle& kernel, const std::string& name_type)   
      { return cr<ActionSequenceHandle,DigiHandle<DigiActionSequence> >(kernel,name_type);           }

      static SynchronizeHandle createSync(KernelHandle& kernel, const std::string& name_type)
      { return cr<SynchronizeHandle,DigiHandle<DigiSynchronize> >(kernel,name_type);                 }

      static DigiAction* toAction(DigiEventAction* f)              { return f;                       }
      static DigiAction* toAction(DigiInputAction* f)              { return f;                       }
      static DigiAction* toAction(DigiSegmentProcessor* f)            { return f;                       }
      static DigiAction* toAction(DigiContainerProcessor* f)       { return f;                       }
      static DigiAction* toAction(DigiActionSequence* f)           { return f;                       }
      static DigiAction* toAction(DigiSynchronize* f)              { return f;                       }
      static DigiAction* toAction(DigiSignalProcessor* f)          { return f;                       }

      static DigiAction* toAction(EventActionHandle f)             { return f.action;                }
      static DigiAction* toAction(InputActionHandle f)             { return f.action;                }
      static DigiAction* toAction(SegmentProcessorHandle f)           { return f.action;                }
      static DigiAction* toAction(ActionSequenceHandle f)          { return f.action;                }
      static DigiAction* toAction(SynchronizeHandle f)             { return f.action;                }
      static DigiAction* toAction(SignalProcessorHandle f)         { return f.action;                }
#endif
    };
  }
}

#include <DDDigi/DigiSynchronize.h>
#include <DDDigi/DigiInputAction.h>
#include <DDDigi/DigiSegmentSplitter.h>
#include <DDDigi/DigiSegmentProcessor.h>
#include <DDDigi/DigiActionSequence.h>
#include <DDDigi/DigiSignalProcessor.h>

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::digi;

#pragma link C++ class dd4hep::digi::KernelHandle;
#pragma link C++ class dd4hep::digi::ActionHandle;

#pragma link C++ class dd4hep::digi::DigiActionCreation;
#pragma link C++ class dd4hep::digi::DigiContext;
#pragma link C++ class dd4hep::digi::DigiKernel;
#pragma link C++ class dd4hep::digi::DigiAction;
#pragma link C++ class dd4hep::digi::DigiEventAction;
#pragma link C++ class dd4hep::digi::DigiInputAction;
#pragma link C++ class dd4hep::digi::DigiActionSequence;
#pragma link C++ class dd4hep::digi::DigiSynchronize;
#pragma link C++ class dd4hep::digi::DigiSignalProcessor;

#pragma link C++ class dd4hep::digi::DigiContainerProcessor;
#pragma link C++ class dd4hep::digi::DigiContainerSequence;
#pragma link C++ class dd4hep::digi::DigiContainerSequenceAction;
#pragma link C++ class dd4hep::digi::DigiMultiContainerProcessor;

#pragma link C++ class dd4hep::digi::DigiSegmentProcessor;
#pragma link C++ class dd4hep::digi::DigiSegmentSequence;
#pragma link C++ class dd4hep::digi::DigiSegmentSplitter;

/// Digi data item wrappers
#pragma link C++ class dd4hep::digi::Particle+;
#pragma link C++ class dd4hep::digi::EnergyDeposit+;
#pragma link C++ class dd4hep::digi::ParticleMapping+;
#pragma link C++ class dd4hep::digi::DepositMapping+;
#pragma link C++ class dd4hep::digi::DepositVector+;

#pragma link C++ class dd4hep::digi::DigiEvent;

//#pragma link C++ class dd4hep::digi::EventActionHandle;
//#pragma link C++ class dd4hep::digi::InputActionHandle;
//#pragma link C++ class dd4hep::digi::SegmentProcessorHandle;
//#pragma link C++ class dd4hep::digi::ActionSequenceHandle;
//#pragma link C++ class dd4hep::digi::SignalProcessorHandle;
//#pragma link C++ class dd4hep::digi::SynchronizeHandle;

#endif
