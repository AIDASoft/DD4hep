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
#ifndef DD4HEP_PLUGINS_H
#define DD4HEP_PLUGINS_H

// ROOT include files
#ifndef __CINT__
#include <string>
#include <vector>
#include <typeinfo>

#ifndef DD4HEP_PARSERS_NO_ROOT
#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#define DD4HEP_ROOT_VERSION_5 1
#endif
#endif

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  class NamedObject;
  template <typename T> class Handle;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry  {
    class LCDD;
  }
  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML  {
    class Handle_t;
    class Collection_t;
    class Document;
    class Element;
  }

  /// Factory base class implementing some utilities
  struct PluginFactoryBase  {
    typedef Geometry::LCDD lcdd_t;
    typedef Handle<NamedObject> ref_t;
    typedef XML::Handle_t xml_h;
    typedef XML::Element  xml_e;
    typedef std::string   str_t;

    template <typename T> static T* ptr(const T* p)     { return (T*)p;  }
    template <typename T> static T& ref(const T* p)     { return *(T*)p; }
    template <typename T> static T  val(const T* p)     { return T(*p);  }
    template <typename T> static T value(const void* p) { return (T)p;   }
    static const char*  value(const void* p) { return (const char*)(p);  }
    template <typename T> static T make_return(const T& p) { return p;      }
  };
  template <> inline int PluginFactoryBase::value<int>(const void* p) { return *(int*)(p); }
  template <> inline long PluginFactoryBase::value<long>(const void* p) { return *(long*)(p); }
  template <> inline std::string PluginFactoryBase::value<std::string>(const void* p) { return *(std::string*)(p); }
  template <> inline const std::string& PluginFactoryBase::value<const std::string&>(const void* p) { return *(std::string*)(p); }

  /// Helper to debug plugin manager calls
  /**
   *  Small helper class to adjust the plugin service debug level
   *  for a limited code scope. Automatically back-adjusts the debug
   *  level at object destruction.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct PluginDebug {
    int m_debug;
    /// Default constructor
    PluginDebug(int dbg = 2)  noexcept(false);
    /// Default destructor
    ~PluginDebug();
    /// Helper to check factory existence
    std::string missingFactory(const std::string& name) const;
  };

  /// Factory template for the plugin mechanism
  class PluginService  {
  private:
  public:
    typedef void* stub_t;

    template <typename FUNCTION> struct FuncPointer {
      union { void* ptr; FUNCTION fcn; } fptr;
      FuncPointer()                      {  fptr.ptr = 0;           }
      FuncPointer(const FuncPointer& c)  {  fptr.ptr = c.fptr.ptr;  }
      FuncPointer(FUNCTION func)         {  fptr.fcn = func;        }
      FuncPointer(void* p)               {  fptr.ptr = p;           }
      void* ptr() const                  {  return fptr.ptr;        }
      FUNCTION function() const          {  return fptr.func;       }
      FuncPointer& operator=(const FuncPointer& c)  { 
        fptr.ptr = c.fptr.ptr; return *this;
      }
    };
    template <typename FUNCTION> static FuncPointer<FUNCTION> function(FUNCTION func)  {
      return FuncPointer<FUNCTION>(func);
    }

    static bool debug();
    static bool setDebug(bool new_value);

    static void* getCreator(const std::string& id, const std::type_info& info);
    static void  addFactory(const std::string& id, stub_t func,
                            const std::type_info& signature_type,
                            const std::type_info& return_type);

#if defined(DD4HEP_ROOT_VERSION_5)
    template <typename R> static R Create(const std::string& name);

    template <typename R, typename A0>
    static R Create(const std::string& name,A0 a0);

    template <typename R, typename A0, typename A1>
    static R Create(const std::string& name, A0 a0, A1 a1);

    template <typename R, typename A0, typename A1, typename A2>
    static R Create(const std::string& name, A0 a0, A1 a1, A2 a2);

    template <typename R, typename A0, typename A1, typename A2, typename A3>
    static R Create(const std::string& name, A0 a0, A1 a1, A2 a2, A3 a3);

    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
    static R Create(const std::string& name, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4);

    template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    static R Create(const std::string& name, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);
#else
    template <typename R, typename... Args> static R Create(const std::string& id, Args... args)  {
      FuncPointer<R(*)(Args...)> f(getCreator(id,typeid(R(Args...))));
      return f.fptr.ptr ? (*f.fptr.fcn)(std::forward<Args>(args)...) : 0;
    }
#endif
  };

  /// Factory template for the plugin mechanism
  template <typename SIGNATURE> class PluginRegistry {
  public:
    typedef PluginService svc_t;
    typedef SIGNATURE signature_t;
#if defined(DD4HEP_ROOT_VERSION_5)
    typedef void (*stub_t)(void *retaddr, void*, const std::vector<void*>& arg, void*);
    static void add(const char* name, stub_t stub);
#else
    template <typename R, typename... Args>  static void add(const std::string& id, R(*func)(Args...))  {
      svc_t::addFactory(id,svc_t::function(func).ptr(),typeid(R(Args...)),typeid(R));
    }
#endif
  };
} /* End namespace DD4hep      */

namespace {
  /// Base factory template
  template <typename P, typename S> class Factory {};
}

#if defined(DD4HEP_ROOT_VERSION_5)
#define DD4HEP_FACTORY_CALL(type,name,signature) DD4hep::PluginRegistry<signature>::add(name,Factory<type,signature>::wrapper);
#else
namespace DD4hep {
  template <> inline long PluginFactoryBase::make_return(const long& p) { static long s=p; return (long)&s; }  
}
#define DD4HEP_FACTORY_CALL(type,name,signature) DD4hep::PluginRegistry<signature>::add(name,Factory<type,signature>::call)
#define DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(X,Y)
#endif

#define DD4HEP_OPEN_PLUGIN(ns,name)  namespace ns { namespace { struct name {}; } } namespace DD4hep
#define DD4HEP_PLUGINSVC_CNAME(name, serial)  name##_dict_##serial
#define DD4HEP_PLUGINSVC_FACTORY(type,name,signature,serial)            \
  namespace {                                                           \
    struct DD4HEP_PLUGINSVC_CNAME(__typeFactory__,serial)  {            \
      DD4HEP_PLUGINSVC_CNAME(__typeFactory__,serial)()  { DD4HEP_FACTORY_CALL(type,#name,signature); } \
    } DD4HEP_PLUGINSVC_CNAME(s____typeFactory__,serial);                \
  }


#define DD4HEP_PLUGIN_FACTORY_ARGS_0(R)                                 \
  template <typename P> class Factory<P, R()>                           \
    : public DD4hep::PluginFactoryBase {                                \
  public:                                                               \
    static void wrapper(void *ret,void*,const std::vector<void*>& ,void*) { \
      *(void**)ret = (void*)call();                                     \
    }                                                                   \
    static R call();                                                    \
  };                                                                    \
  template <typename P> inline R Factory<P,R()>::call()
  
#define DD4HEP_PLUGIN_FACTORY_ARGS_1(R,A0)                              \
  template <typename P> class Factory<P, R(A0)>                         \
    : public DD4hep::PluginFactoryBase  {                               \
  public:                                                               \
    static void wrapper(void *ret,void*,const std::vector<void*>& a,void*) { \
      *(void**)ret = (void*)call(value<A0>(a[0]));                      \
    }                                                                   \
    static R call(A0 a0);                                               \
  };                                                                    \
  template <typename P> inline R Factory<P,R(A0)>::call(A0 a0)

#define DD4HEP_PLUGIN_FACTORY_ARGS_2(R,A0,A1)                           \
  template <typename P> class Factory<P, R(A0,A1)>                      \
    : public DD4hep::PluginFactoryBase  {                               \
  public:                                                               \
    static void wrapper(void *ret,void*,const std::vector<void*>& a,void*) { \
      *(void**)ret = (void*)call(value<A0>(a[0]),value<A1>(a[1]));      \
    }                                                                   \
    static R call(A0 a0,A1 a1);                                         \
  };                                                                    \
  template <typename P> inline R Factory<P,R(A0,A1)>::call(A0 a0, A1 a1)
  
#define DD4HEP_PLUGIN_FACTORY_ARGS_3(R,A0,A1,A2)                        \
    template <typename P> class Factory<P, R(A0,A1,A2)>                 \
      : public DD4hep::PluginFactoryBase  {                             \
    public:                                                             \
      static void wrapper(void *ret,void*,const std::vector<void*>& a,void*) { \
        *(void**)ret = (void*)call(value<A0>(a[0]),value<A1>(a[1]),value<A2>(a[2])); \
      }                                                                 \
      static R call(A0 a0,A1 a1,A2 a2);                                 \
    };                                                                  \
    template <typename P> inline R Factory<P,R(A0,A1,A2)>::call(A0 a0, A1 a1, A2 a2)

#define DD4HEP_PLUGIN_FACTORY_ARGS_4(R,A0,A1,A2,A3)                     \
    template <typename P> class Factory<P, R(A0,A1,A2,A3)>              \
      : public DD4hep::PluginFactoryBase  {                             \
    public:                                                             \
      static void wrapper(void *ret,void*,const std::vector<void*>& a,void*) { \
        *(void**)ret = (void*)call(value<A0>(a[0]),value<A1>(a[1]),value<A2>(a[2]),value<A3>(a[3])); \
      }                                                                 \
      static R call(A0 a0,A1 a1,A2 a2, A3 a3);                          \
    };                                                                  \
    template <typename P> inline R Factory<P,R(A0,A1,A2,A3)>::call(A0 a0, A1 a1, A2 a2, A3 a3)
  
#define DD4HEP_PLUGIN_FACTORY_ARGS_5(R,A0,A1,A2,A3,A4)                  \
    template <typename P> class Factory<P, R(A0,A1,A2,A3,A4)>           \
      : public DD4hep::PluginFactoryBase  {                             \
    public:                                                             \
      static void wrapper(void *ret,void*,const std::vector<void*>& a,void*) { \
        *(void**)ret = (void*)call(value<A0>(a[0]),value<A1>(a[1]),value<A2>(a[2]),value<A3>(a[3]),value<A4>(a[4])); \
      }                                                                 \
      static R call(A0 a0,A1 a1,A2 a2, A3 a3, A4 a4);                   \
    };                                                                  \
    template <typename P> inline R Factory<P,R(A0,A1,A2,A3,A4)>::call(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
  



#endif    /* __CINT__          */
#endif    /* DD4HEP_PLUGINS_H  */
