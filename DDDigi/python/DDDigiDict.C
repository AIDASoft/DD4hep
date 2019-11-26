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

#include "DDDigi/DigiData.h"
#include "DDDigi/DigiHandle.h"
#include "DDDigi/DigiKernel.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiSynchronize.h"
#include "DDDigi/DigiActionSequence.h"
#include "DDDigi/DigiSignalProcessor.h"

struct DDDigiDict  {};

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

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
    }

    ACTIONHANDLE(SignalProcessor);
    ACTIONHANDLE(Action);
    ACTIONHANDLE(EventAction);
    ACTIONHANDLE(ActionSequence);
    ACTIONHANDLE(Synchronize);

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
      static ActionHandle createAction(KernelHandle& kernel, const std::string& name_type)   
      { return cr<ActionHandle,DigiHandle<DigiAction> >(kernel,name_type);                           }
      static EventActionHandle createEventAction(KernelHandle& kernel, const std::string& name_type)   
      { return cr<EventActionHandle,DigiHandle<DigiEventAction> >(kernel,name_type);                 }
      static ActionSequenceHandle createSequence(KernelHandle& kernel, const std::string& name_type)   
      { return cr<ActionSequenceHandle,DigiHandle<DigiActionSequence> >(kernel,name_type);           }
      static SynchronizeHandle createSync(KernelHandle& kernel, const std::string& name_type)
      { return cr<SynchronizeHandle,DigiHandle<DigiSynchronize> >(kernel,name_type);                 }

      static DigiAction* toAction(DigiAction* f)                   { return f;                       }
      static DigiAction* toAction(DigiActionSequence* f)           { return f;                       }
      static DigiAction* toAction(DigiSynchronize* f)              { return f;                       }
      static DigiAction* toAction(DigiSignalProcessor* f)          { return f;                       }

      static DigiAction* toAction(ActionHandle f)                  { return f.action;                }
      static DigiAction* toAction(ActionSequenceHandle f)          { return f.action;                }
      static DigiAction* toAction(SynchronizeHandle f)             { return f.action;                }
      static DigiAction* toAction(SignalProcessorHandle f)         { return f.action;                }

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
    };
  }
}

// CINT configuration
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::digi;

#pragma link C++ class dd4hep::digi::DigiActionCreation;
#pragma link C++ class dd4hep::digi::DigiContext;

#pragma link C++ class dd4hep::digi::DigiKernel;
#pragma link C++ class dd4hep::digi::KernelHandle;

#pragma link C++ class dd4hep::digi::DigiAction;
#pragma link C++ class dd4hep::digi::ActionHandle;

#pragma link C++ class dd4hep::digi::DigiEventAction;
#pragma link C++ class dd4hep::digi::EventActionHandle;

#pragma link C++ class dd4hep::digi::DigiActionSequence;
#pragma link C++ class dd4hep::digi::ActionSequenceHandle;

#pragma link C++ class dd4hep::digi::DigiSynchronize;
#pragma link C++ class dd4hep::digi::SynchronizeHandle;

#pragma link C++ class dd4hep::digi::DigiSignalProcessor;
#pragma link C++ class dd4hep::digi::SignalProcessorHandle;

/// Digi data item wrappers
#pragma link C++ class dd4hep::digi::DigiEvent;
#pragma link C++ class dd4hep::digi::DigiEnergyDeposits+;
#pragma link C++ class dd4hep::digi::DigiCounts+;

#endif
