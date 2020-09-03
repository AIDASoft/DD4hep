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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "DDEve/View.h"
#include "DDEve/DD4hepMenu.h"
#include "DDEve/PopupMenu.h"
#include "DDEve/EventControl.h"

// ROOT include files
#include "TEveBrowser.h"
#include "TEveManager.h"
#include "TEveElement.h"
#include "TEveWindow.h"
#include "TEveViewer.h"
#include "TGLViewer.h"
#include "TGClient.h"
#include "TSystem.h"

// Forward declarations
class TEveWindowSlot;

using namespace std;
using namespace dd4hep;

ClassImp(DD4hepMenu)

/// Initializing constructor
DD4hepMenu::DD4hepMenu(Display* display) 
: PopupMenu(display->client().GetRoot()), m_display(display), m_evtCtrl(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
DD4hepMenu::~DD4hepMenu()  {
  detail::deletePtr(m_evtCtrl);
  InstanceCount::decrement(this);
}

/// Add the menu to the menu bar
void DD4hepMenu::Build(TGMenuBar* menubar, int hints)    {
  int id;
  PopupMenu& pm = *this;
  pm.AddEntry("&Load XML",          this, &DD4hepMenu::OnLoadXML);
  //Not for now: pm.AddEntry("&Load ROOT Geometry",this, &DD4hepMenu::OnLoadRootGeometry);
  id = pm.AddEntry("&Show Event I/O",    this, &DD4hepMenu::OnCreateEventIO);
  pm.menu().DisableEntry(id);
  id = pm.AddEntry("&Open Event Data",   this, &DD4hepMenu::OnOpenEventData);
  pm.menu().DisableEntry(id);
  id = pm.AddEntry("&Next Event",        this, &DD4hepMenu::OnNextEvent);
  pm.menu().DisableEntry(id);
  id = pm.AddEntry("&Previous Event",    this, &DD4hepMenu::OnPreviousEvent);
  pm.menu().DisableEntry(id);
  pm.AddEntry("&Exit",              this, &DD4hepMenu::OnExit);
  menubar->AddPopup("&dd4hep",*this, new TGLayoutHints(hints, 0, 4, 0, 0));
}

/// Callback when the geometry was loaded
void DD4hepMenu::OnGeometryLoaded()   {
  TGPopupMenu& pm = menu();
  pm.DisableEntry(pm.GetEntry("Load XML")->GetEntryId());
  pm.EnableEntry(pm.GetEntry("Show Event I/O")->GetEntryId());
  pm.EnableEntry(pm.GetEntry("Open Event Data")->GetEntryId());
  pm.DisableEntry(pm.GetEntry("Next Event")->GetEntryId());
  pm.DisableEntry(pm.GetEntry("Previous Event")->GetEntryId());
}

/// Callback when loading the configuration
void DD4hepMenu::OnLoadXML(TGMenuEntry* /* entry */, void* /* ptr */)  {
  std::string fname = m_display->OpenXmlFileDialog(".");
  if ( !fname.empty() )  {
    m_display->LoadXML(fname.c_str());
    OnGeometryLoaded();
  }
}

/// Callback when loading the configuration
void DD4hepMenu::OnLoadRootGeometry(TGMenuEntry* /* entry */, void* /* ptr */)  {
  std::string fname = m_display->OpenEventFileDialog(".");
  if ( !fname.empty() )  {
    m_display->LoadGeometryRoot(fname.c_str());
  }
}

/// Callback to show the event I/O panel
void DD4hepMenu::OnCreateEventIO(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if ( 0 == m_evtCtrl )  {
    TEveBrowser* browser = m_display->manager().GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);
    m_evtCtrl = new EventControl(m_display,600,450);
    m_evtCtrl->Build();
    browser->SetTabTitle("Evt I/O", 0);
    browser->StopEmbedding();
    menu().DisableEntry(menu().GetEntry("Show Event I/O")->GetEntryId());
  }
}

/// Callback when loading a new event data file
void DD4hepMenu::OnOpenEventData(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if ( 0 == m_evtCtrl )  {
    OnCreateEventIO(0,0);
  }
  if ( m_evtCtrl->Open() )   {
    TGPopupMenu& pm = menu();
    pm.EnableEntry(pm.GetEntry("Open Event Data")->GetEntryId());
    pm.EnableEntry(pm.GetEntry("Next Event")->GetEntryId());
    pm.EnableEntry(pm.GetEntry("Previous Event")->GetEntryId());
  }
}

/// Callback when loading the next event
void DD4hepMenu::OnNextEvent(TGMenuEntry* /* entry */, void* /* ptr */)  {
  m_display->eventHandler().NextEvent();
}

/// Callback when loading the previous event
void DD4hepMenu::OnPreviousEvent(TGMenuEntry* /* entry */, void* /* ptr */)  {
  m_display->eventHandler().PreviousEvent();
}

/// Callback when exiting the display
void DD4hepMenu::OnExit(TGMenuEntry* /* entry */, void* /* ptr */)    {
  delete m_display;
  printout(INFO,"DDEve","+++ The life of this display instance ended.... good bye!");
  gSystem->Exit(0,kTRUE);
}
