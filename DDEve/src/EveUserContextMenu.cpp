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
#include "DD4hep/Printout.h"
#include "DDEve/Display.h"
#include "DDEve/ContextMenu.h"
#include "DDEve/EveUserContextMenu.h"

// ROOT include files
#include "TEveGeoShape.h"
#include "TEveManager.h"
#include "TEveElement.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

ClassImp(EveUserContextMenu)

/// Initializing constructor
EveUserContextMenu::EveUserContextMenu(Display* m) : m_display(m)  {
}

/// Default destructor
EveUserContextMenu::~EveUserContextMenu()  {
}

/// EVE manager
TEveManager& EveUserContextMenu::manager() const   {
  return m_display->manager();
}

/// Set everything (in-) visible
void EveUserContextMenu::VisibleAll(bool visible, TObject* target, void* /* user_param */)   {
  TEveElement* element = dynamic_cast<TEveElement*>(target);
  if ( element ) m_display->MakeNodesVisible(element,visible,9999);
}

/// Set self (in-) visible
void EveUserContextMenu::VisibleSelf(bool visible, TObject* target, void* /* user_param */)  {
  TEveElement* element = dynamic_cast<TEveElement*>(target);
  if ( element ) m_display->MakeNodesVisible(element,visible,0);
}

/// Set the children (in-) visible
void EveUserContextMenu::VisibleChildren(bool visible, TObject* target, void* /* user_param */)  {
  TEveElement* element = dynamic_cast<TEveElement*>(target);
  if ( element ) m_display->MakeNodesVisible(element,visible,1);
}

/// Show all
void EveUserContextMenu::ShowAll(TObject* target, void* user_param)   {
  VisibleAll(true,target,user_param);
}

/// Show shape self
void EveUserContextMenu::ShowSelf(TObject* target, void* user_param)   {
  VisibleSelf(true,target,user_param);
}

/// Show shape children
void EveUserContextMenu::ShowChildren(TObject* target, void* user_param)   {
  VisibleChildren(true,target,user_param);
}

/// Hide all
void EveUserContextMenu::HideAll(TObject* target, void* user_param)   {
  VisibleAll(false,target,user_param);
}

/// Hide shape self
void EveUserContextMenu::HideSelf(TObject* target, void* user_param)   {
  VisibleSelf(false,target,user_param);
}

/// Hide shape children
void EveUserContextMenu::HideChildren(TObject* target, void* user_param)   {
  VisibleChildren(false,target,user_param);
}

/// Load next level children and add them to all scenes
void EveUserContextMenu::LoadChildren(TObject* target, void* /* user_param */)   {
  TEveElement* element = dynamic_cast<TEveElement*>(target);
  if ( element ) m_display->LoadGeoChildren(element, 1, true);
}

/// Load all children and add them to all scenes
void EveUserContextMenu::DeepLoadChildren(TObject* target, void* /* user_param */)   {
  TEveElement* element = dynamic_cast<TEveElement*>(target);
  if ( element ) m_display->LoadGeoChildren(element, 9999, true);
}

/// Install the geometry context menu
void EveUserContextMenu::InstallGeometryContextMenu(TClass* cl)   {
  ContextMenu& m = ContextMenu::instance(cl);
  m.Clear();
  m.Add("Show Children Recursive",this,&EveUserContextMenu::ShowAll);
  m.Add("Hide Children Recursive",this,&EveUserContextMenu::HideAll);
  m.AddSeparator();
  m.Add("Show Children",this,&EveUserContextMenu::ShowChildren);
  m.Add("Hide Children",this,&EveUserContextMenu::HideChildren);
  m.Add("Show Self",this,&EveUserContextMenu::ShowSelf);
  m.Add("Hide Self",this,&EveUserContextMenu::HideSelf);
  m.AddSeparator();
  m.Add("Load Children",this,&EveUserContextMenu::LoadChildren);
  m.Add("Deep Load",this,&EveUserContextMenu::DeepLoadChildren);
}
