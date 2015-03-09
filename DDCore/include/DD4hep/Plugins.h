// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_PLUGINS_H
#define DD4HEP_PLUGINS_H

// ROOT include files
#ifndef __CINT__
#include <string>
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

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
    PluginDebug(int dbg = 2);
    /// Default destructor
    ~PluginDebug();
    /// Helper to check factory existence
    std::string missingFactory(const std::string& name) const;
  };

  template <typename SIGNATURE> class PluginRegistry {
  public:
    typedef void (*stub_t)(void *retaddr, void*, const std::vector<void*>& arg, void*);
    typedef SIGNATURE signature_t;
    static void add(const char* name, stub_t stub);
  };

  class PluginService  {
  private:
  public:
    typedef void (*stub_t)(void *retaddr, void*, const std::vector<void*>& arg, void*);

    static bool debug();
    static bool setDebug(bool new_value);

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
  };

} /* End namespace DD4hep      */

#define DD4HEP_PLUGINSVC_CNAME(name, serial)  name##_dict_##serial

#define DD4HEP_FACTORY_CALL(type,name,signature)   DD4hep::PluginRegistry<signature>::add(name,Factory<type,signature>::Func);

#define DD4HEP_PLUGINSVC_FACTORY(type,name,signature,serial)	\
  namespace {\
  struct DD4HEP_PLUGINSVC_CNAME(__typeFactory__,serial)  {		\
    DD4HEP_PLUGINSVC_CNAME(__typeFactory__,serial)()  { DD4HEP_FACTORY_CALL(type,#name,signature); } \
  } DD4HEP_PLUGINSVC_CNAME(s____typeFactory__,serial);\
}


#endif    /* __CINT__          */
#endif    /* DD4HEP_PLUGINS_H  */
