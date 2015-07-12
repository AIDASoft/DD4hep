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
#include "DDEve/PopupMenu.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"

// C/C++ include files

// Forward declarations

using namespace DD4hep;

ClassImp(PopupMenu)

/// Standard constructor
PopupMenu::PopupMenu(const TGWindow *parent) : m_popup(parent), m_cmd(0)  {
  m_popup.Connect("Activated(int)", "DD4hep::PopupMenu", this, "HandleMenu(int)");
  InstanceCount::increment(this);
}

/// Default destructor
PopupMenu::~PopupMenu()  {
  m_popup.Disconnect("Activated(int)", this, "HandleMenu(int)");
  m_calls.clear();
  InstanceCount::decrement(this);
}

/// Add the menu to the menu bar
void PopupMenu::Build(TGMenuBar* /* bar */, int /* hints */)    {
}

/// Add a menu separator to the menu.
void PopupMenu::AddSeparator(TGMenuEntry* before)  {
  m_popup.AddSeparator(before);
}

/// Add a menu label to the menu.
void PopupMenu::AddLabel(const char* s, const TGPicture* p, TGMenuEntry* before)  {
  m_popup.AddLabel(s,p,before);
}

/// Add a (cascading) popup menu to a popup menu.
void PopupMenu::AddPopup(const char* s, TGPopupMenu* popup, TGMenuEntry* before, const TGPicture* p)  {
  m_popup.AddPopup(s,popup,before,p);
}

/// Add a new popup menu entry with a callback
int PopupMenu::AddEntry(const char* name, Callback cb, void* ud, const TGPicture* p, TGMenuEntry* before)   {
  m_calls[++m_cmd] = cb;
  m_popup.AddEntry(name, m_cmd, ud, p, before);
  return m_cmd;
}

/// Handle menu items.
void PopupMenu::HandleMenu(int id)   {
  Callbacks::const_iterator i = m_calls.find(id);
  if ( i != m_calls.end() )  {
    TGMenuEntry* e = m_popup.GetEntry(id);
    void* ud = e->GetUserData();
    const void* args[2] = {&e, ud}; 
    printout(INFO,"PopupMenu","+++ HandleMenu: executing callback with ID=%d Arg=%p [%p]",id,ud,&ud);
    (*i).second.execute(args);
    return;
  }
  printout(INFO,"PopupMenu","+++ HandleMenu: unhandled callback with ID=%d",id);
}

/// Check menu entry
void PopupMenu::CheckEntry(int id)  {
  m_popup.CheckEntry(id);
}

/// Uncheck menu entry
void PopupMenu::UnCheckEntry(int id)   {
  m_popup.UnCheckEntry(id);
}

/// Get check-value
bool PopupMenu::IsEntryChecked(int id)  {
  return m_popup.IsEntryChecked(id);
}


