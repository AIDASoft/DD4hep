// $Id: Readout.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/LCDD.h"
#include "DD4hep/Handle.h"
#include "DD4hep/Plugins.inl"
#include "XML/XMLElements.h"

using namespace std;
using namespace DD4hep;

namespace {
  inline int* s_debug_value()   {
    static int s_debug_value = ::getenv("DD4HEP_TRACE") == 0 ? 0 : 1;
    return &s_debug_value;
  }
}

bool PluginService::debug()  {
  return *s_debug_value() ? true : false;
}

bool PluginService::setDebug(bool new_value)   {
  int *ptr = s_debug_value();
  bool old_value = *ptr;
  *ptr = new_value ? 1 : 0;
  return old_value;
}

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)

/// Default constructor
PluginDebug::PluginDebug(int dbg)
  : m_debug(0) {
  m_debug = ROOT::Reflex::PluginService::Debug();
  ROOT::Reflex::PluginService::SetDebug(dbg);
}

/// Default destructor
PluginDebug::~PluginDebug() {
  ROOT::Reflex::PluginService::SetDebug (m_debug);
}

/// Helper to check factory existence
string PluginDebug::missingFactory(const string& name) const {
  ROOT::Reflex::Scope factories = ROOT::Reflex::Scope::ByName(PLUGINSVC_FACTORY_NS);
  string factoryname = ROOT::Reflex::PluginService::FactoryName(name);
  string msg = "\n\t\tNo factory with name " + factoryname + " for type " + name + " found.\n\t\tPlease check library load path.";
  return msg;
}

#else   // ROOT 6

// Do not know yet what code to really put in there.....at least it presevers the interfaces and links

/// Default constructor
PluginDebug::PluginDebug(int)
  : m_debug(0) {
}

/// Default destructor
PluginDebug::~PluginDebug() {
}

/// Helper to check factory existence
string PluginDebug::missingFactory(const string& name) const {
  string factoryname = "??? Create("+name+")";
  string msg = "\n\t\tNo factory with name " + factoryname + " for type " + name + " found.\n\t\tPlease check library load path.";
  return msg;
}

#endif

DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Geometry::LCDD*,XML::Handle_t*,Geometry::Ref_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Geometry::LCDD*,XML::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(NamedObject*, (Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*,XML::Handle_t*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*,XML::Handle_t const*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*, int, char**))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, (Geometry::LCDD*))
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(long, ())
DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(void*, (const char*))


