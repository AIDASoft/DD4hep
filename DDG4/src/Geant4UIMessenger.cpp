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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4UIMessenger.h"

// Geant4 include files
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace {
  struct InstallProperties {
    Geant4UIMessenger::Commands& cmds;
    const std::string& path;
    G4UImessenger* msg;
    InstallProperties(Geant4UIMessenger::Commands& c, const std::string& p, G4UImessenger* m)
      : cmds(c), path(p), msg(m) {
    }
    void operator()(const pair<string, Property>& o) {
      string n = path + o.first;
      G4UIcmdWithAString* cmd = new G4UIcmdWithAString(n.c_str(), msg);
      cmd->SetParameterName(o.first.c_str(), true);
      cmd->SetGuidance(("Property item of type " + o.second.type()).c_str());
      cmds[cmd] = o.first;
    }
  };
}

Geant4UIMessenger::Geant4UIMessenger(const string& name, const string& path)
  : G4UImessenger(), m_directory(0), m_properties(0), m_name(name), m_path(path) {
  m_directory = new G4UIdirectory(path.c_str());
  printout(INFO, "Geant4UIMessenger", "+++ %s> Install Geant4 control directory:%s", name.c_str(), path.c_str());
  m_directory->SetGuidance(("Control hierarchy for Geant4 action:" + name).c_str());
}

/// Default destructor
Geant4UIMessenger::~Geant4UIMessenger() {
  destroyFirst(m_propertyCmd);
  destroyFirst(m_actionCmd);
}

/// Add a new callback structure
void Geant4UIMessenger::addCall(const std::string& name, const std::string& description, const Callback& cb) {
  G4UIcommand* cmd = new G4UIcmdWithoutParameter((m_path + name).c_str(), this);
  cmd->SetGuidance(description.c_str());
  m_actionCmd[cmd] = cb;
}

/// Export all properties to the Geant4 UI
void Geant4UIMessenger::exportProperties(PropertyManager& mgr) {
  InstallProperties installer(m_propertyCmd, m_path, this);
  m_properties = &mgr;
  addCall("show", "Show all properties of Geant4 component:" + m_name,
          Callback(m_properties).make(&PropertyManager::dump));
  m_properties->for_each(installer);
}

/// Pass current property value to Geant4 UI
G4String Geant4UIMessenger::GetCurrentValue(G4UIcommand * c) {
  Commands::iterator i = m_propertyCmd.find(c);
  if (m_properties && i != m_propertyCmd.end()) {
    const string& n = (*i).second;
    return (*m_properties)[n].str();
  }
  printout(INFO, "Geant4UIMessenger",
           "+++ %s> Failed to access property value.", m_name.c_str());
  return "";
}

/// Accept ne property value from Geant4 UI
void Geant4UIMessenger::SetNewValue(G4UIcommand *c, G4String v) {
  Commands::iterator i = m_propertyCmd.find(c);
  if (m_properties && i != m_propertyCmd.end()) {
    const string& n = (*i).second;
    try  {
      if (!v.empty()) {
        Property& p = (*m_properties)[n];
        p.str(v);
        printout(INFO, "Geant4UIMessenger",
                 "+++ %s> Setting property value %s = %s  native:%s.",
                 m_name.c_str(), n.c_str(), v.c_str(), p.str().c_str());
      }
      else {
        string value = (*m_properties)[n].str();
        printout(INFO, "Geant4UIMessenger", "+++ %s> Unchanged property value %s = %s.",
                 m_name.c_str(), n.c_str(), value.c_str());
      }
    }
    catch(const exception& e)   {
      printout(INFO, "Geant4UIMessenger", "+++ %s> Exception: Failed to change property %s = '%s'.",
               m_name.c_str(), n.c_str(), v.c_str());
      printout(INFO, "Geant4UIMessenger", "+++ %s> Exception: %s", m_name.c_str(), e.what());
    }
    catch(...)   {
      printout(INFO, "Geant4UIMessenger", "+++ %s> UNKNOWN Exception: Failed to change property %s = '%s'.",
               m_name.c_str(), n.c_str(), v.c_str());
    }
    return;
  }
  else  {
    Actions::iterator j = m_actionCmd.find(c);
    if (j != m_actionCmd.end()) {
      try  {
        (*j).second.execute(0);
      }
      catch(const exception& e)   {
        printout(INFO, "Geant4UIMessenger", "+++ %s> Exception: Failed to exec action '%s' [%s].",
                 m_name.c_str(), c->GetCommandName().c_str(), c->GetCommandPath().c_str());
        printout(INFO, "Geant4UIMessenger", "+++ %s> Exception: %s",e.what());
      }
      catch(...)   {
        printout(INFO, "Geant4UIMessenger", "+++ %s> UNKNOWN Exception: Failed to exec action '%s' [%s].",
                 m_name.c_str(), c->GetCommandName().c_str(), c->GetCommandPath().c_str());
      }
      return;
    }
  }
  printout(INFO, "Geant4UIMessenger", "+++ %s> Unknown command callback!", m_name.c_str());
}
