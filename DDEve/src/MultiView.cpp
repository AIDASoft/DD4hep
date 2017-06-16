// $Id: $
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
#include "DDEve/MultiView.h"
#include "DDEve/Factories.h"
#include "DDEve/DisplayConfiguration.h"
#include "DD4hep/Plugins.h"

#include <iostream>

using namespace std;
using namespace dd4hep;

ClassImp(MultiView)
DECLARE_VIEW_FACTORY(MultiView)

/// Import Geometry data
static void _build(Display* display, View* v, TEveWindowSlot* slot)  {
  v->Build(slot);
  display->RegisterEvents(v);
  v->ConfigureGeometryFromInfo();
  v->ConfigureEventFromInfo();
  v->Initialize();
}

/// Initializing constructor
MultiView::MultiView(Display* eve, const string& nam) : View(eve, nam)
{
}

/// Default destructor
MultiView::~MultiView()  {
}

/// Build the projection view and map it to the given slot
View& MultiView::Build(TEveWindow* slot)   {
  typedef DisplayConfiguration::Configurations _C;
  typedef Display::ViewConfigurations _V;
  TEveWindowSlot* new_slot;
  View* v;

  // Viewers
  TEveWindowPack *pack = ((TEveWindowSlot*)slot)->MakePack();
  pack->SetElementName(m_name.c_str());
  pack->SetHorizontal();
  pack->SetShowTitleBar(kFALSE);

  CreateScenes().Map(new_slot = pack->NewSlot());
  pack = pack->NewSlot()->MakePack();
  pack->SetShowTitleBar(kFALSE);

  _C panels;
  const _V& view_cfg = m_eve->viewConfigurations();
  _V::const_iterator icfg = view_cfg.find(m_name);
  if ( icfg != view_cfg.end() )   {
    const _C& c = (*icfg).second.subdetectors;
    for(_C::const_iterator i = c.begin(); i!=c.end();++i)  {
      if ( (*i).type == DisplayConfiguration::PANEL ) panels.push_back(*i);
    }
  }
  /// First panel
  if ( panels.size()>0)   {
    const DisplayConfiguration::Config& cfg = panels[0];
    string typ = "dd4hep__"+cfg.use;
    v = PluginService::Create<View*>(typ.c_str(),m_eve,cfg.name.c_str());
  }
  else  {
    v = PluginService::Create<View*>("dd4hep_DDEve_RhoZProjection",m_eve,(m_name+" - RhoZ View").c_str());
  }
  (new_slot = pack->NewSlot())->MakeCurrent();
  _build(m_eve,v,new_slot);

  /// Second panel
  if ( panels.size()>1)   {
    const DisplayConfiguration::Config& cfg = panels[1];
    string typ = "dd4hep__"+cfg.use;
    v = PluginService::Create<View*>(typ.c_str(),m_eve,cfg.name.c_str());
  }
  else  {
    v = PluginService::Create<View*>("dd4hep_DDEve_RhoPhiProjection",m_eve,(m_name+" - RPhi View").c_str());
  }
  (new_slot = pack->NewSlot())->MakeCurrent();
  _build(m_eve,v,new_slot);
  return *this;
}
