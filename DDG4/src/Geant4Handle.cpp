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

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"

#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4GeneratorAction.h"
#include "DDG4/Geant4RunAction.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4TrackingAction.h"
#include "DDG4/Geant4SteppingAction.h"
#include "DDG4/Geant4StackingAction.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4PhysicsList.h"
#include "DDG4/Geant4ActionPhase.h"
#include "DDG4/Geant4UserInitialization.h"
#include "DDG4/Geant4DetectorConstruction.h"

// Geant4 include files
#include "G4Threading.hh"
#include "G4AutoLock.hh"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;
namespace {
  G4Mutex creation_mutex=G4MUTEX_INITIALIZER;
}

namespace DD4hep {
  namespace Simulation {


    template <typename TYPE> static inline TYPE* checked_value(TYPE* p) {
      if (p) {
        return p;
      }
      throw runtime_error(format("Geant4Handle", "Attempt to access an invalid object of type:%s!", typeName(typeid(TYPE)).c_str()));
    }

    template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle()
      : value(0) {
    }

    template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle(TYPE* typ)
      : value(typ) {
      if (value)
        value->addRef();
    }

    template <typename TYPE> Geant4Handle<TYPE>::Geant4Handle(const Geant4Handle<TYPE>& handle)
      : value(0) {
      value = handle.get();
      if (value)
        value->addRef();
    }

    template <typename TYPE> TYPE* _create_object(Geant4Kernel& kernel, const TypeName& typ)    {
      Geant4Context* ctxt = kernel.workerContext();
      Geant4Action* object = PluginService::Create<Geant4Action*>(typ.first, ctxt, typ.second);
      if (!object && typ.first == typ.second) {
        string _t = typeName(typeid(TYPE));
        printout(DEBUG, "Geant4Handle", "Object factory for %s not found. Try out %s",
                 typ.second.c_str(), _t.c_str());
        object = PluginService::Create<Geant4Action*>(_t, ctxt, typ.second);
        if (!object) {
          size_t idx = _t.rfind(':');
          if (idx != string::npos)
            _t = string(_t.substr(idx + 1));
          printout(DEBUG, "Geant4Handle", "Try out object factory for %s",_t.c_str());
          object = PluginService::Create<Geant4Action*>(_t, ctxt, typ.second);
        }
      }
      if (object)  {
        TYPE* ptr = dynamic_cast<TYPE*>(object);
        if (ptr)  {
          return ptr;
        }
        throw runtime_error(format("Geant4Handle", "Failed to convert object of type %s to handle of type %s!",
                                   typ.first.c_str(),typ.second.c_str()));
      }
      throw runtime_error(format("Geant4Handle", "Failed to create object of type %s!", typ.first.c_str()));
    }

    template <typename TYPE, typename CONT> 
    TYPE* _create_share(Geant4Kernel& kernel,
                        CONT& (Geant4Kernel::*pmf)(), 
                        const string& type_name, 
                        const string& shared_typ, 
                        bool shared, TYPE*)
    {
      TypeName typ = TypeName::split(type_name);
      Geant4Kernel& k = shared ? kernel.master() : kernel;
      if ( shared && k.isMultiThreaded() )   {
        typedef typename TYPE::shared_type _ST;
        TypeName s_type = TypeName::split(shared_typ+"/"+typ.second);
        _ST* object = (_ST*)_create_object<TYPE>(kernel,s_type);
        CONT& container = (k.*pmf)();
        TYPE* value = 0;
        // Need to protect the global action sequence!
        G4AutoLock protection_lock(&creation_mutex);    {
          value = container.get(typ.second);
          if ( !value ) value = _create_object<TYPE>(k,typ);
          container.adopt(value);
        }
        object->use(value);
        value->info("+++ Created shared object for %s of type %s.",
                    typ.second.c_str(),typeName(typeid(TYPE)).c_str());
        return object;
      }
      TYPE* value = _create_object<TYPE>(k,typ);
      return value;
    }

    template <typename TYPE> 
    Geant4Handle<TYPE>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool /* shared */)  {
      value = _create_object<TYPE>(kernel,TypeName::split(type_name));
    }

    template <typename TYPE> 
    Geant4Handle<TYPE>::Geant4Handle(Geant4Kernel& kernel, const char* type_name_char, bool /* shared */)  {
      value = _create_object<TYPE>(kernel,TypeName::split(type_name_char ? type_name_char : "????"));
    }

    template <typename TYPE> Geant4Handle<TYPE>::~Geant4Handle() {
      if (value)
        value->release();
      value = 0;
    }

    template <typename TYPE> TYPE* Geant4Handle<TYPE>::release() {
      TYPE* temp = value;
      value = 0;
      return temp;
    }

    template <typename TYPE> void Geant4Handle<TYPE>::checked_assign(TYPE* p) {
      if (value)
        value->release();
      value = checked_value(p);
      if (value)
        value->addRef();
    }

    template <typename TYPE> Property& Geant4Handle<TYPE>::operator[](const string& property_name) const {
      PropertyManager& pm = checked_value(value)->properties();
      return pm[property_name];
    }

    template <typename TYPE> Geant4Handle<TYPE>::operator TYPE*() const {
      return checked_value(value);
    }

    template <typename TYPE> bool Geant4Handle<TYPE>::operator!() const {
      return 0 == value;
    }

    template <typename TYPE> TYPE* Geant4Handle<TYPE>::get() const {
      return checked_value(value);
    }

    template <typename TYPE> TYPE* Geant4Handle<TYPE>::operator->() const {
      return checked_value(value);
    }

    template <typename TYPE> Geant4Action* Geant4Handle<TYPE>::action() const {
      return checked_value(value);
    }

    template <typename TYPE> Geant4Handle<TYPE>& Geant4Handle<TYPE>::operator=(const Geant4Handle& handle) {
      if ( &handle != this )  {
        if (value) value->release();
        value = handle.get();
        if (value) value->addRef();
      }
      return *this;
    }

    template <typename TYPE> Geant4Handle<TYPE>& Geant4Handle<TYPE>::operator=(TYPE* typ) {
      if ( typ != value )  {
        if (value)    value->release();
        value = typ;
        if (value)    value->addRef();
      }
      return *this;
    }

    //namespace DD4hep {
    //  namespace Simulation {

    KernelHandle::KernelHandle()  {
      value = &Geant4Kernel::instance(Geometry::LCDD::getInstance());
    }
    KernelHandle::KernelHandle(Geant4Kernel* k) : value(k)  {
    }
    KernelHandle KernelHandle::worker()  {
      Geant4Kernel* k = value ? &value->worker(Geant4Kernel::thread_self()) : 0;
      if ( k ) return KernelHandle(k);
      throw runtime_error(format("KernelHandle", "Cannot access worker context [Invalid Handle]"));
    }
    void KernelHandle::destroy()  {
      if ( value ) delete value;
      value = 0;
    }

    template <> 
    Geant4Handle<Geant4RunAction>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::runAction,type_name,"Geant4SharedRunAction",shared,(handled_type*)0);
    }
    template <> 
    Geant4Handle<Geant4RunAction>::Geant4Handle(Geant4Kernel& kernel, const char* type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::runAction,type_name,"Geant4SharedRunAction",shared,(handled_type*)0);
    }

    template <> 
    Geant4Handle<Geant4EventAction>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::eventAction,type_name,"Geant4SharedEventAction",shared,(handled_type*)0);
    }
    template <> 
    Geant4Handle<Geant4EventAction>::Geant4Handle(Geant4Kernel& kernel, const char* type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::eventAction,type_name,"Geant4SharedEventAction",shared,(handled_type*)0);
    }

    template <> 
    Geant4Handle<Geant4GeneratorAction>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::generatorAction,type_name,"Geant4SharedGeneratorAction",shared,(handled_type*)0);
    }
    template <> 
    Geant4Handle<Geant4GeneratorAction>::Geant4Handle(Geant4Kernel& kernel, const char* type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::generatorAction,type_name,"Geant4SharedGeneratorAction",shared,(handled_type*)0);
    }

    template <> 
    Geant4Handle<Geant4TrackingAction>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::trackingAction,type_name,"Geant4SharedTrackingAction",shared,(handled_type*)0);
    }
    template <> 
    Geant4Handle<Geant4TrackingAction>::Geant4Handle(Geant4Kernel& kernel, const char* type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::trackingAction,type_name,"Geant4SharedTrackingAction",shared,(handled_type*)0);
    }

    template <> 
    Geant4Handle<Geant4SteppingAction>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::steppingAction,type_name,"Geant4SharedSteppingAction",shared,(handled_type*)0);
    }
    template <> 
    Geant4Handle<Geant4SteppingAction>::Geant4Handle(Geant4Kernel& kernel, const char* type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::steppingAction,type_name,"Geant4SharedSteppingAction",shared,(handled_type*)0);
    }

    template <> 
    Geant4Handle<Geant4StackingAction>::Geant4Handle(Geant4Kernel& kernel, const string& type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::stackingAction,type_name,"Geant4SharedStackingAction",shared,(handled_type*)0);
    }
    template <> 
    Geant4Handle<Geant4StackingAction>::Geant4Handle(Geant4Kernel& kernel, const char* type_name, bool shared)  {
      value = _create_share(kernel,&Geant4Kernel::stackingAction,type_name,"Geant4SharedStackingAction",shared,(handled_type*)0);
    }

    template <> Geant4Handle<Geant4Sensitive>::Geant4Handle(Geant4Kernel& kernel, const string& type_name,
                                                            const string& detector, bool /* shared */) {
      try {
        Geant4Context* ctxt = kernel.workerContext();
        TypeName typ = TypeName::split(type_name);
        Geometry::LCDD& lcdd = kernel.lcdd();
        Geometry::DetElement det = lcdd.detector(detector);
        Geant4Sensitive* object = PluginService::Create<Geant4Sensitive*>(typ.first, ctxt, typ.second, &det, &lcdd);
        if (object) {
          value = object;
          return;
        }
      }
      catch (const exception& e) {
        printout(ERROR, "Geant4Handle<Geant4Sensitive>", "Exception: %s", e.what());
      }
      catch (...) {
        printout(ERROR, "Geant4Handle<Geant4Sensitive>", "Exception: Unknown exception");
      }
      throw runtime_error(format("Geant4Handle<Geant4Sensitive>", 
                                 "Failed to create sensitive object of type %s for detector %s!",
                                 type_name.c_str(), detector.c_str()));
    }
  

    template class Geant4Handle<Geant4Action>;
    template class Geant4Handle<Geant4Filter>;
    template class Geant4Handle<Geant4Sensitive>;
    template class Geant4Handle<Geant4ActionPhase>;
    template class Geant4Handle<Geant4PhaseAction>;
    template class Geant4Handle<Geant4GeneratorAction>;
    template class Geant4Handle<Geant4RunAction>;
    template class Geant4Handle<Geant4EventAction>;
    template class Geant4Handle<Geant4TrackingAction>;
    template class Geant4Handle<Geant4SteppingAction>;
    template class Geant4Handle<Geant4StackingAction>;
    template class Geant4Handle<Geant4DetectorConstruction>;
    template class Geant4Handle<Geant4PhysicsList>;
    template class Geant4Handle<Geant4UserInitialization>;

    template class Geant4Handle<Geant4GeneratorActionSequence>;
    template class Geant4Handle<Geant4PhysicsListActionSequence>;
    template class Geant4Handle<Geant4RunActionSequence>;
    template class Geant4Handle<Geant4EventActionSequence>;
    template class Geant4Handle<Geant4TrackingActionSequence>;
    template class Geant4Handle<Geant4SteppingActionSequence>;
    template class Geant4Handle<Geant4StackingActionSequence>;
    template class Geant4Handle<Geant4SensDetActionSequence>;
    template class Geant4Handle<Geant4UserInitializationSequence>;
    template class Geant4Handle<Geant4DetectorConstructionSequence>;
  }
}
