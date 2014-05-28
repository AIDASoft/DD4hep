// $Id: Readout.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Plugins.h"
#include "Reflex/Reflex.h"

using namespace std;
using namespace DD4hep;
using namespace ROOT::Reflex;

/// Default constructor
PluginDebug::PluginDebug(int dbg)
    : m_debug(0) {
  m_debug = PluginService::Debug();
  PluginService::SetDebug(dbg);
}

/// Default destructor
PluginDebug::~PluginDebug() {
  PluginService::SetDebug (m_debug);
}

/// Helper to check factory existence
string PluginDebug::missingFactory(const string& name) const {
  string msg = "";
  Scope factories = Scope::ByName(PLUGINSVC_FACTORY_NS);
  string factoryname = PluginService::FactoryName(name);
  msg = "\n\t\tNo factory with name " + factoryname + " for type " + name + " found.\n\t\tPlease check library load path.";
  return msg;
}

