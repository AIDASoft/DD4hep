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

ClassImp(dd4hepMenu)

/// Initializing constructor
dd4hepMenu::dd4hepMenu(Display* display) 
: PopupMenu(display->client().GetRoot()), m_display(display), m_evtCtrl(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
dd4hepMenu::~dd4hepMenu()  {
  detail::deletePtr(m_evtCtrl);
  InstanceCount::decrement(this);
}

/// Add the menu to the menu bar
void dd4hepMenu::Build(TGMenuBar* bar, int hints)    {
  int id;
  PopupMenu& m = *this;
  m.AddEntry("&Load XML",          this, &dd4hepMenu::OnLoadXML);
  //Not for now: m.AddEntry("&Load ROOT Geometry",this, &dd4hepMenu::OnLoadRootGeometry);
  id = m.AddEntry("&Show Event I/O",    this, &dd4hepMenu::OnCreateEventIO);
  m.menu().DisableEntry(id);
  id = m.AddEntry("&Open Event Data",   this, &dd4hepMenu::OnOpenEventData);
  m.menu().DisableEntry(id);
  id = m.AddEntry("&Next Event",        this, &dd4hepMenu::OnNextEvent);
  m.menu().DisableEntry(id);
  id = m.AddEntry("&Previous Event",    this, &dd4hepMenu::OnPreviousEvent);
  m.menu().DisableEntry(id);
  m.AddEntry("&Exit",              this, &dd4hepMenu::OnExit);
  bar->AddPopup("&dd4hep",*this, new TGLayoutHints(hints, 0, 4, 0, 0));
}

/// Callback when the geometry was loaded
void dd4hepMenu::OnGeometryLoaded()   {
  TGPopupMenu& m = menu();
  m.DisableEntry(m.GetEntry("Load XML")->GetEntryId());
  m.EnableEntry(m.GetEntry("Show Event I/O")->GetEntryId());
  m.EnableEntry(m.GetEntry("Open Event Data")->GetEntryId());
  m.DisableEntry(m.GetEntry("Next Event")->GetEntryId());
  m.DisableEntry(m.GetEntry("Previous Event")->GetEntryId());
}

/// Callback when loading the configuration
void dd4hepMenu::OnLoadXML(TGMenuEntry* /* entry */, void* /* ptr */)  {
  std::string fname = m_display->OpenXmlFileDialog(".");
  if ( !fname.empty() )  {
    m_display->LoadXML(fname.c_str());
    OnGeometryLoaded();
  }
}

/// Callback when loading the configuration
void dd4hepMenu::OnLoadRootGeometry(TGMenuEntry* /* entry */, void* /* ptr */)  {
  std::string fname = m_display->OpenEventFileDialog(".");
  if ( !fname.empty() )  {
    m_display->LoadGeometryRoot(fname.c_str());
  }
}

/// Callback to show the event I/O panel
void dd4hepMenu::OnCreateEventIO(TGMenuEntry* /* entry */, void* /* ptr */)  {
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
void dd4hepMenu::OnOpenEventData(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if ( 0 == m_evtCtrl )  {
    OnCreateEventIO(0,0);
  }
  if ( m_evtCtrl->Open() )   {
    TGPopupMenu& m = menu();
    m.EnableEntry(m.GetEntry("Open Event Data")->GetEntryId());
    m.EnableEntry(m.GetEntry("Next Event")->GetEntryId());
    m.EnableEntry(m.GetEntry("Previous Event")->GetEntryId());
  }
}

/// Callback when loading the next event
void dd4hepMenu::OnNextEvent(TGMenuEntry* /* entry */, void* /* ptr */)  {
  m_display->eventHandler().NextEvent();
}

/// Callback when loading the previous event
void dd4hepMenu::OnPreviousEvent(TGMenuEntry* /* entry */, void* /* ptr */)  {
  m_display->eventHandler().PreviousEvent();
}

/// Callback when exiting the display
void dd4hepMenu::OnExit(TGMenuEntry* /* entry */, void* /* ptr */)    {
  delete m_display;
  printout(INFO,"DDEve","+++ The life of this display instance ended.... good bye!");
  gSystem->Exit(0,kTRUE);
}
