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

  int init_grammar_types();
  
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <typename VAL>
    int add_action_property(DigiAction* action, const std::string& name, VAL value);

    struct ActionHandle  {
      DigiAction* action;
      explicit ActionHandle(DigiAction* a) : action(a)  {
	if (action) action->addRef();
      }
      ActionHandle(const ActionHandle& h) : action(h.action) {
	if (action) action->addRef();
      }
      ~ActionHandle()   {
	if (action) action->release();
      }
      ActionHandle& operator=(const ActionHandle& h) { 
	if ( h.action ) h.action->addRef();
	if ( action ) action->release();
	action = h.action;
	return *this;
      }
      DigiAction* release()  {
	DigiAction* tmp = action;
	action=0;
	return tmp;
      }
      operator dd4hep::digi::DigiAction* () const         { return action; }
      DigiAction* operator->() const                      { return action; }
      DigiAction* get() const                             { return action; }
      DigiAction* I_am_a_ROOT_interface_handle() const    { return action; }
      KernelHandle kernel()  const  {
	auto* k = const_cast<DigiKernel*>(action->kernel());
	return KernelHandle(k);
      }
    };

    struct PropertyResult  {
    public:
      std::string data   {   };
      int status         { 0 };
      PropertyResult() = default;
      PropertyResult(const std::string& d, int s);
      PropertyResult(const PropertyResult& c) = default;
      ~PropertyResult() = default;
    };
    
    inline PropertyResult::PropertyResult(const std::string& d, int s)
      : data(d), status(s)
    {
    }

    struct DigiActionCreation  {
      template <typename H> static H* cst(DigiAction* in)  {
	auto* out = dynamic_cast<H*>(in);
	if ( out ) return out;
	if ( in )
	  except("DigiAction","Invalid cast of action '%s' [type:%s] to type %s!",
		 in->c_name(), typeName(typeid(DigiAction)).c_str(), typeName(typeid(H)).c_str());
	except("DigiAction","Invalid cast of NULL [type:%s] to type %s!",
	       typeName(typeid(DigiAction)).c_str(), typeName(typeid(H)).c_str());
	return nullptr;
      }

      static KernelHandle createKernel(DigiAction* action)   {
	auto* k = const_cast<DigiKernel*>(action->kernel());
	return KernelHandle(k);
      }

      static ActionHandle createAction(KernelHandle& kernel, const std::string& name_type)   {
        DigiHandle<DigiAction> action(*kernel.get(),name_type);
        return ActionHandle(action.get());
      }
      static DigiAction* toAction(DigiKernel* f)    { return f;        }
      static DigiAction* toAction(DigiAction* f)    { return f;        }
      static DigiAction* toAction(KernelHandle f)   { return f.value;  }
      static DigiAction* toAction(ActionHandle f)   { return f.action; }

      static DigiEventAction*        toEventAction(DigiAction* a) { return cst<DigiEventAction>(a); }
      static DigiDepositMonitor*     toDepositMonitor(DigiAction* a) { return cst<DigiDepositMonitor>(a); }
      static DigiContainerProcessor* toContainerProcessor(DigiAction* a) { return cst<DigiContainerProcessor>(a); }

      /// Access DigiAction property
      static PropertyResult getProperty(DigiAction* action, const std::string& name)  {
        if ( action->hasProperty(name) )  {
          return PropertyResult(action->property(name).str(),1);
        }
        return PropertyResult("",0);
      }

      /// Set DigiAction property
      static int setProperty(DigiAction* action, const std::string& name, const std::string& value)  {
	init_grammar_types();
	printout(DEBUG,"setProperty","Setting property: %s.%s = %s", action->name().c_str(), name.c_str(), value.c_str());
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
    };
  }
}

#include <DDDigi/DigiSynchronize.h>
#include <DDDigi/DigiInputAction.h>
#include <DDDigi/DigiSegmentSplitter.h>
#include <DDDigi/DigiActionSequence.h>
#include <DDDigi/DigiSignalProcessor.h>
#include <DDDigi/DigiDepositMonitor.h>

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

#pragma link C++ class dd4hep::digi::DigiSegmentSplitter;

#pragma link C++ class dd4hep::digi::DigiDepositMonitor;

/// Digi data item wrappers
#pragma link C++ class dd4hep::digi::Particle+;
#pragma link C++ class dd4hep::digi::EnergyDeposit+;
#pragma link C++ class dd4hep::digi::ParticleMapping+;
#pragma link C++ class dd4hep::digi::DepositMapping+;
#pragma link C++ class dd4hep::digi::DepositVector+;

#pragma link C++ class dd4hep::digi::DigiEvent;

#endif
