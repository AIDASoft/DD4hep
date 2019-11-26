//==========================================================================
//  AIDA Detector description implementation 
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
#include "DD4hep/Detector.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"

#include "DDDigi/DigiHandle.h"
#include "DDDigi/DigiKernel.h"
#include "DDDigi/DigiEventAction.h"
#include "DDDigi/DigiSignalProcessor.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
 
    template <typename TYPE> static inline TYPE* checked_value(TYPE* p) {
      if (p) {
        return p;
      }
      except("DigiHandle","Attempt to access an invalid object of type:%s!",
             typeName(typeid(TYPE)).c_str());
      return 0;
    }

    template <typename TYPE> DigiHandle<TYPE>::DigiHandle(TYPE* typ) : value(typ)  {
      if (value)
        value->addRef();
    }

    template <typename TYPE> DigiHandle<TYPE>::DigiHandle(const DigiHandle<TYPE>& handle) : value(handle.get())
    {
      if (value)
        value->addRef();
    }

    template <typename TYPE> DigiHandle<TYPE>::DigiHandle(DigiHandle<TYPE>&& handle) : value(handle.get())
    {
      handle.value = 0;
    }

    template <typename T> T* _raw_create(const std::string& t, const DigiKernel& kernel, const std::string& n)    {
      DigiEventAction* object = PluginService::Create<DigiEventAction*>(t, &kernel, n);
      return object ? dynamic_cast<T*>(object) : nullptr;
    }
    template <> DigiAction* _raw_create<DigiAction>(const std::string& t, const DigiKernel& kernel, const std::string& n)    {
      return PluginService::Create<DigiAction*>(t, &kernel, n);
    }
    template <> DigiSignalProcessor* _raw_create<DigiSignalProcessor>(const std::string& t, const DigiKernel& kernel, const std::string& n)    {
      return PluginService::Create<DigiSignalProcessor*>(t, &kernel, n);
    }
    template <typename TYPE> TYPE* _create_object(const DigiKernel& kernel, const TypeName& typ)    {
      TYPE* object = _raw_create<TYPE>(typ.first, kernel, typ.second);
      if (!object && typ.first == typ.second) {
        string _t = typeName(typeid(TYPE));
        printout(DEBUG, "DigiHandle", "Object factory for %s not found. Try out %s",
                 typ.second.c_str(), _t.c_str());
        object = _raw_create<TYPE>(_t, kernel, typ.second);
        if (!object) {
          size_t idx = _t.rfind(':');
          if (idx != string::npos)
            _t = string(_t.substr(idx + 1));
          printout(DEBUG, "DigiHandle", "Try out object factory for %s",_t.c_str());
          object = _raw_create<TYPE>(_t, kernel, typ.second);
        }
      }
      if (object)  {
        return object;
      }
      except("DigiHandle", "Failed to create object of type %s!", typ.first.c_str());
      return nullptr;
    }

    template <typename TYPE> 
    DigiHandle<TYPE>::DigiHandle(const DigiKernel& kernel, const string& type_name)  {
      value = _create_object<TYPE>(kernel,TypeName::split(type_name));
    }

    template <typename TYPE> 
    DigiHandle<TYPE>::DigiHandle(const DigiKernel& kernel, const char* type_name_char)  {
      value = _create_object<TYPE>(kernel,TypeName::split(type_name_char ? type_name_char : "????"));
    }

    template <typename TYPE> DigiHandle<TYPE>::~DigiHandle() {
      if (value)
        value->release();
      value = 0;
    }

    template <typename TYPE> TYPE* DigiHandle<TYPE>::release() {
      TYPE* temp = value;
      value = 0;
      return temp;
    }

    template <typename TYPE> void DigiHandle<TYPE>::checked_assign(TYPE* p) {
      if (value)
        value->release();
      value = checked_value(p);
      if (value)
        value->addRef();
    }

    template <typename TYPE> Property& DigiHandle<TYPE>::operator[](const string& property_name) const {
      PropertyManager& pm = checked_value(value)->properties();
      return pm[property_name];
    }

    template <typename TYPE> DigiHandle<TYPE>::operator TYPE*() const {
      return checked_value(value);
    }

    template <typename TYPE> bool DigiHandle<TYPE>::operator!() const {
      return 0 == value;
    }

    template <typename TYPE> TYPE* DigiHandle<TYPE>::get() const {
      return checked_value(value);
    }

    template <typename TYPE> TYPE* DigiHandle<TYPE>::operator->() const {
      return checked_value(value);
    }

    template <typename TYPE> DigiAction* DigiHandle<TYPE>::action() const {
      return checked_value(value);
    }

    /// Assignment operator
    template <typename TYPE> DigiHandle<TYPE>& DigiHandle<TYPE>::operator=(const DigiHandle& handle) {
      if ( &handle != this )  {
        TYPE* point = value;
        value = handle.get();
        if ( value ) value->addRef();
        if ( point ) point->release();
      }
      return *this;
    }

    /// Assignment move operator
    template <typename TYPE> DigiHandle<TYPE>& DigiHandle<TYPE>::operator=(DigiHandle&& handle) {
      if ( value ) value->release();
      value = handle.get();
      handle.value = 0;
      return *this;
    }

    template <typename TYPE> DigiHandle<TYPE>& DigiHandle<TYPE>::operator=(TYPE* pointer) {
      if ( pointer != value )  {
        TYPE* point = value;
        value = pointer;
        if ( value ) value->addRef();
        if ( point ) point->release();
      }
      return *this;
    }

    KernelHandle::KernelHandle()  {
      value = &DigiKernel::instance(Detector::getInstance());
    }
    KernelHandle::KernelHandle(DigiKernel* k) : value(k)  {
    }
  }
}

#include "DDDigi/DigiSynchronize.h"
#include "DDDigi/DigiActionSequence.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
    template class DigiHandle<DigiAction>;
    template class DigiHandle<DigiEventAction>;
    template class DigiHandle<DigiSynchronize>;
    template class DigiHandle<DigiActionSequence>;
    template class DigiHandle<DigiSignalProcessor>;
  }
}
