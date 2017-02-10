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
#include "DDEve/View.h"
#include "DDEve/Display.h"
#include "DDEve/ViewMenu.h"
#include "DDEve/DD4hepMenu.h"
#include "DDEve/ElementList.h"
#include "DDEve/GenericEventHandler.h"
#include "DDEve/EveShapeContextMenu.h"
#include "DDEve/EvePgonSetProjectedContextMenu.h"
#include "DDEve/Utilities.h"
#include "DDEve/DDEveEventData.h"
#include "DDEve/HitActors.h"
#include "DDEve/ParticleActors.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/LCDDData.h"
#include "DD4hep/Printout.h"

// ROOT include files
#include "TH2.h"
#include "TFile.h"
#include "TSystem.h"
#include "TGTab.h"
#include "TGMsgBox.h"
#include "TGClient.h"
#include "TGFileDialog.h"
#include "TEveScene.h"
#include "TEveBrowser.h"
#include "TEveManager.h"
#include "TEveCaloData.h"
#include "TEveCalo.h"
#include "TEveViewer.h"
#include "TEveCompound.h"
#include "TEveBoxSet.h"
#include "TEvePointSet.h"
#include "TEveTrackPropagator.h"
#include "TGeoManager.h"

// C/C++ include files
#include <stdexcept>
#include <climits>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


ClassImp(Display)

namespace DD4hep {
  void EveDisplay(const char* xmlConfig = 0, const char* eventFileName = 0)  {
    Display* display = new Display(TEveManager::Create(true,"VI"));
    if ( xmlConfig != 0 )   {
      char text[PATH_MAX];
      ::snprintf(text,sizeof(text),"%s%s",strncmp(xmlConfig,"file:",5)==0 ? "file:" : "",xmlConfig);
      display->LoadXML(text);
    }
    else   {
      display->MessageBox(INFO,"No DDEve setup given.\nYou need to choose now.....\n"
                          "If you need an example, open\n\n"
                          "examples/CLIDSid/eve/DDEve.xml\n"
                          "and the corresponding event data\n"
                          "examples/CLIDSid/eve/CLICSiD_Events.root\n\n\n",
                          "Need to choos setup file");
      display->ChooseGeometry();
      //display->LoadXML("file:../DD4hep/examples/CLICSiD/compact/DDEve.xml");
    }
    if (eventFileName != 0) {
      display->eventHandler().Open(display->getEventHandlerName(),eventFileName);
    }
  }
}


Display::CalodataContext::CalodataContext() 
  : slice(0), calo3D(0), caloViz(0), eveHist(0), config()
{
}

Display::CalodataContext::CalodataContext(const CalodataContext& c) 
  : slice(c.slice), calo3D(c.calo3D), caloViz(c.caloViz), eveHist(c.eveHist), config(c.config)
{
}

Display::CalodataContext& Display::CalodataContext::operator=(const CalodataContext& c)  
{
  if ( &c == this ) return *this;
  config = c.config;
  slice = c.slice;
  calo3D = c.calo3D;
  caloViz = c.caloViz;
  eveHist = c.eveHist;
  return *this;
}

/// Standard constructor
Display::Display(TEveManager* eve) 
  : m_eve(eve), m_lcdd(0), m_evtHandler(0), m_geoGlobal(0), m_eveGlobal(0),
    m_viewMenu(0), m_dd4Menu(0), m_visLevel(7), m_loadLevel(1)
{
  TEveBrowser* br = m_eve->GetBrowser();
  TGMenuBar* bar = br->GetMenuBar();
  EveShapeContextMenu::install(this);
  EvePgonSetProjectedContextMenu::install(this);
  ElementListContextMenu::install(this);
  m_lcdd = &Geometry::LCDD::getInstance();
  m_evtHandler = new GenericEventHandler();
  m_evtHandler->Subscribe(this);
  m_lcdd->addExtension<Display>(this);
  br->ShowCloseTab(kFALSE);
  m_eve->GetViewers()->SwitchColorSet();
  TFile::SetCacheFileDir(".");
  BuildMenus(bar);
  br->SetTabTitle("Global Scene",TRootBrowser::kRight,0);
}

/// Default destructor
Display::~Display()   {
  TRootBrowser* br = m_eve->GetBrowser();
  m_lcdd->removeExtension<Display>(false);
  m_viewConfigs.clear();
  deletePtr(m_evtHandler);
  deletePtr(m_eveGlobal);
  deletePtr(m_geoGlobal);
  br->CloseTabs();
  deletePtr(m_dd4Menu);
  deletePtr(m_viewMenu);
  deletePtr(m_eve);
  //br->ReallyDelete();
  LCDDData* data = dynamic_cast<LCDDData*>(m_lcdd);
  if ( data ) data->destroyData(false);
  deletePtr(m_lcdd);
  gGeoManager = 0;
  gEve = 0;
}

/// Load geometry from compact xml file
void Display::LoadXML(const char* xmlFile)     {
  TGeoManager& mgr = m_lcdd->manager();
  bool has_geo = !m_geoTopics.empty();
  m_lcdd->fromXML(xmlFile);
  if ( !has_geo )  {
    LoadGeoChildren(0,m_loadLevel,false);
    mgr.SetVisLevel(m_visLevel);
  }
  if ( m_dd4Menu && !m_geoTopics.empty() )   {
    m_dd4Menu->OnGeometryLoaded();
  }

  m_eve->FullRedraw3D(kTRUE); // Reset camera 
  BuildMenus(m_eve->GetBrowser()->GetMenuBar());
}

/// Load geometry from compact xml file
void Display::LoadGeometryRoot(const char* /* rootFile */)     {
  throw runtime_error("This call is not implemented !");
}

/// Load geometry with panel
void Display::ChooseGeometry()   {
  m_dd4Menu->OnLoadXML(0,0);
}

/// Access to geometry hub
Geometry::LCDD& Display::lcdd() const  {
  return *m_lcdd;
}

/// Access to X-client
TGClient& Display::client() const   {
  return *gClient;
}

/// Access to the event reader
GenericEventHandler& Display::eventHandler() const   {
  if ( m_evtHandler )  {
    return *m_evtHandler;
  }
  throw runtime_error("Invalid event handler");
}

/// Add new menu to the main menu bar
void Display::AddMenu(TGMenuBar* bar, PopupMenu* menu, int hints)  {
  m_menus.insert(menu);
  menu->Build(bar, hints);
  m_eve->FullRedraw3D(kTRUE); // Reset camera and redraw
}

/// Import configuration parameters
void Display::ImportConfiguration(const DisplayConfiguration& config)   {
  DisplayConfiguration::ViewConfigurations::const_iterator i;
  for(i=config.views.begin(); i!=config.views.end(); ++i)  
    m_viewConfigs[(*i).name] = *i;

  DisplayConfiguration::Configurations::const_iterator j;
  for(j=config.calodata.begin(); j!=config.calodata.end(); ++j)  
    m_calodataConfigs[(*j).name] = *j;
  for(j=config.collections.begin(); j!=config.collections.end(); ++j)  
    m_collectionsConfigs[(*j).name] = *j;
}

/// Access to calo data histograms by name as defined in the configuration
Display::CalodataContext& Display::GetCaloHistogram(const string& nam)   {
  Calodata::iterator i = m_calodata.find(nam);
  if ( i == m_calodata.end() )  {
    DataConfigurations::const_iterator j = m_calodataConfigs.find(nam);
    if ( j != m_calodataConfigs.end() )   {
      CalodataContext ctx;
      ctx.config = (*j).second;
      string use = ctx.config.use;
      string hits = ctx.config.hits;
      if ( use.empty() )  {
        const char* n = nam.c_str();
        const DisplayConfiguration::Calodata& cd = (*j).second.data.calodata;
        TH2F* h = new TH2F(n,n,cd.n_eta, cd.eta_min, cd.eta_max, cd.n_phi, cd.phi_min, cd.phi_max);
        h->SetTitle(hits.c_str());
        ctx.eveHist = new TEveCaloDataHist();
        ctx.slice = ctx.eveHist->GetNSlices();
        ctx.eveHist->AddHistogram(h);
        ctx.eveHist->RefSliceInfo(0).Setup(n,cd.threshold,cd.color,101);
        ctx.eveHist->GetEtaBins()->SetTitleFont(120);
        ctx.eveHist->GetEtaBins()->SetTitle("h");
        ctx.eveHist->GetPhiBins()->SetTitleFont(120);
        ctx.eveHist->GetPhiBins()->SetTitle("f");
        ctx.eveHist->IncDenyDestroy();

        ctx.calo3D = new TEveCalo3D(ctx.eveHist);
        ctx.calo3D->SetName(n);
        ctx.calo3D->SetBarrelRadius(cd.rmin);
        ctx.calo3D->SetEndCapPos(cd.dz);
        ctx.calo3D->SetAutoRange(kTRUE);
        ctx.calo3D->SetMaxTowerH(cd.towerH);
        ImportGeo(ctx.calo3D);
        EtaPhiHistogramActor actor(h);
        eventHandler().collectionLoop(hits,actor);
        ctx.eveHist->DataChanged();
      }
      else   {
        CalodataContext c = GetCaloHistogram(use);
        ctx = c;
        ctx.config.use = use;
        ctx.config.hits = hits;
        ctx.config.name = nam;
      }
      i = m_calodata.insert(make_pair(nam,ctx)).first;
      return (*i).second;      
    }
    throw runtime_error("Cannot access calodata configuration "+nam);
  }
  return (*i).second;
}

/// Access a data filter by name. Data filters are used to customize views
const Display::ViewConfig* Display::GetViewConfiguration(const string& nam)  const   {
  ViewConfigurations::const_iterator i = m_viewConfigs.find(nam);
  return (i == m_viewConfigs.end()) ? 0 : &((*i).second);
}

/// Access a data filter by name. Data filters are used to customize calodatas
const Display::DataConfig* Display::GetCalodataConfiguration(const string& nam)  const   {
  DataConfigurations::const_iterator i = m_calodataConfigs.find(nam);
  return (i == m_calodataConfigs.end()) ? 0 : &((*i).second);
}

/// Register to the main event scene on new events
void Display::RegisterEvents(View* view)   {
  m_eveViews.insert(view);
}

/// Unregister from the main event scene
void Display::UnregisterEvents(View* view)   {
  Views::iterator i = m_eveViews.find(view);
  if ( i != m_eveViews.end() )  {
    m_eveViews.erase(i);
  }
}

/// Open standard message box
void Display::MessageBox(PrintLevel level, const string& text, const string& title) const   {
  string path = TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")).Data();
  const TGPicture* pic = 0;
  if ( level == VERBOSE )
    pic = client().GetPicture((path+"mb_asterisk_s.xpm").c_str());
  else if ( level == DEBUG )
    pic = client().GetPicture((path+"mb_asterisk_s.xpm").c_str());
  else if ( level == INFO )
    pic = client().GetPicture((path+"mb_asterisk_s.xpm").c_str());
  else if ( level == WARNING )
    pic = client().GetPicture((path+"mb_excalamation_s.xpm").c_str());
  else if ( level == ERROR )
    pic = client().GetPicture((path+"mb_stop.xpm").c_str());
  else if ( level == FATAL )
    pic = client().GetPicture((path+"interrupt.xpm").c_str());
  new TGMsgBox(gClient->GetRoot(),0,title.c_str(),text.c_str(),pic,
               kMBDismiss,0,kVerticalFrame,kTextLeft|kTextCenterY);
}

/// Popup XML file chooser. returns chosen file name; empty on cancel
string Display::OpenXmlFileDialog(const string& default_dir)   const {
  static const char *evtFiletypes[] = { 
    "xml files",    "*.xml",
    "XML files",    "*.XML",
    "All files",     "*",
    0,               0 
  };
  TGFileInfo fi;
  fi.fFileTypes = evtFiletypes;
  fi.fIniDir    = StrDup(default_dir.c_str());
  fi.fFilename  = 0;
  new TGFileDialog(client().GetRoot(), 0, kFDOpen, &fi);
  if ( fi.fFilename ) {
    string ret = fi.fFilename;
    if ( ret.find("file:") != 0 ) return "file:"+ret;
    return ret;
  }
  return "";
}

/// Popup ROOT file chooser. returns chosen file name; empty on cancel
string Display::OpenEventFileDialog(const string& default_dir)   const {
  static const char *evtFiletypes[] = { 
    "ROOT files",    "*.root",
    "SLCIO files",   "*.slcio",
    "LCIO files",    "*.lcio",
    "All files",     "*",
    0,               0 
  };
  TGFileInfo fi;
  fi.fFileTypes = evtFiletypes;
  fi.fIniDir    = StrDup(default_dir.c_str());
  fi.fFilename  = 0;
  new TGFileDialog(client().GetRoot(), 0, kFDOpen, &fi);
  if ( fi.fFilename ) {
    return fi.fFilename;
  }
  return "";
}

/// Build the DDEve specific menues
void Display::BuildMenus(TGMenuBar* bar)   {
  if ( 0 == bar ) {
    bar = m_eve->GetBrowser()->GetMenuBar();
  }
  if ( 0 == m_dd4Menu )  {
    m_dd4Menu = new DD4hepMenu(this);
    AddMenu(bar, m_dd4Menu);
  }
  if ( 0 == m_viewMenu && !m_viewConfigs.empty() )  {
    m_viewMenu = new ViewMenu(this,"&Views");
    AddMenu(bar, m_viewMenu, kLHintsRight);
  }
}

/// Open ROOT file
TFile* Display::Open(const char* name) const   {
  TFile* f = TFile::Open(name);
  if ( f && !f->IsZombie() ) return f;
  throw runtime_error("+++ Failed to open ROOT file:"+string(name));
}

/// Consumer event data
void Display::OnFileOpen(EventHandler& /* handler */ )   {
}

/// Consumer event data
void Display::OnNewEvent(EventHandler& handler )   {
  typedef EventHandler::TypedEventCollections Types;
  typedef vector<EventHandler::Collection> Collections;
  const Types& types = handler.data();
  TEveElement* particles = 0;

  printout(ERROR,"EventHandler","+++ Display new event.....");
  manager().GetEventScene()->DestroyElements();
  for(Types::const_iterator ityp=types.begin(); ityp!=types.end(); ++ityp)  {
    const Collections& colls = (*ityp).second;
    for(Collections::const_iterator j=colls.begin(); j!=colls.end(); ++j)   {
      size_t len = (*j).second;
      const char* nam = (*j).first;
      if ( len > 0 )   {
        EventHandler::CollectionType typ = handler.collectionType(nam);
        if ( typ == EventHandler::CALO_HIT_COLLECTION ||
             typ == EventHandler::TRACKER_HIT_COLLECTION )  {
          const DataConfigurations::const_iterator i=m_collectionsConfigs.find(nam);
          if ( i != m_collectionsConfigs.end() )  {
            const DataConfig& cfg = (*i).second;
            if ( cfg.hits == "PointSet" )  {
              PointsetCreator cr(nam,len,cfg);
              handler.collectionLoop((*j).first, cr);
              ImportEvent(cr.element());
            }
            else if ( cfg.hits == "BoxSet" )  {
              BoxsetCreator cr(nam,len,cfg);
              handler.collectionLoop((*j).first, cr);
              ImportEvent(cr.element());
            }
            else if ( cfg.hits == "TowerSet" )  {
              TowersetCreator cr(nam,len,cfg);
              handler.collectionLoop((*j).first, cr);
              ImportEvent(cr.element());
            }
            else {  // Default is point set
              PointsetCreator cr(nam,len);
              handler.collectionLoop((*j).first, cr);
              ImportEvent(cr.element());
            }
          }
          else  {
            PointsetCreator cr(nam,len);
            handler.collectionLoop((*j).first, cr);
            ImportEvent(cr.element());
          }
        }
        else if ( typ == EventHandler::PARTICLE_COLLECTION )   {
          // We do not have to care about memory leaks here:
          // TEveTrackPropagator is reference counted and will be destroyed if the
          // last track is gone ie. when we re-initialize the event scene

          // $$$ Do not know exactly what the field parameters mean
          const DataConfigurations::const_iterator i=m_collectionsConfigs.find(nam);
          const DataConfig* cfg = (i==m_collectionsConfigs.end()) ? 0 : &((*i).second);
          MCParticleCreator cr(new TEveTrackPropagator("","",new TEveMagFieldDuo(350, -3.5, 2.0)),
                               new TEveCompound("MC_Particles","MC_Particles"),cfg);
          handler.collectionLoop((*j).first, cr);
          cr.close();
          particles = cr.particles;
        }
      }
    }
  }
  for(Calodata::iterator i = m_calodata.begin(); i != m_calodata.end(); ++i)
    (*i).second.eveHist->GetHist(0)->Reset();
  for(Calodata::iterator i = m_calodata.begin(); i != m_calodata.end(); ++i)  {
    CalodataContext& ctx = (*i).second;
    TH2F* h = ctx.eveHist->GetHist(0);
    EtaPhiHistogramActor actor(h);
    size_t n = eventHandler().collectionLoop(ctx.config.hits, actor);
    ctx.eveHist->DataChanged();
    printout(INFO,"FillEtaPhiHistogram","+++ %s: Filled %ld hits from %s....",
             ctx.calo3D->GetName(), n, ctx.config.hits.c_str());
  }
  /// We absolutely want to import the particles as the last elements, otherwise
  /// they end up under the hits and are close to invisible
  if ( particles )  {
    ImportEvent(particles);
  }
  for(Views::iterator i = m_eveViews.begin(); i != m_eveViews.end(); ++i)
    (*i)->ConfigureEventFromInfo();
  manager().Redraw3D();
}

/// Access / Create global geometry element
TEveElementList& Display::GetGeo()   {
  if ( 0 == m_geoGlobal )  {
    m_geoGlobal = new ElementList("Geo-Global","Geo-Global", true, true);
    manager().AddGlobalElement(m_geoGlobal);
  }
  return *m_geoGlobal;
}

/// Access/Create a topic by name
TEveElementList& Display::GetGeoTopic(const string& name)    {
  Topics::iterator i=m_geoTopics.find(name);
  if ( i == m_geoTopics.end() )  {
    TEveElementList* topic = new ElementList(name.c_str(), name.c_str(), true, true);
    m_geoTopics[name] = topic;
    GetGeo().AddElement(topic);
    return *topic;
  }
  return *((*i).second);
}

/// Access/Create a topic by name. Throws exception if the topic does not exist
TEveElementList& Display::GetGeoTopic(const string& name) const   {
  Topics::const_iterator i=m_geoTopics.find(name);
  if ( i == m_geoTopics.end() )  {
    throw runtime_error("Display: Attempt to access non-existing geometry topic:"+name);
  }
  return *((*i).second);
}

/// Access/Create a topic by name
TEveElementList& Display::GetEveTopic(const string& name)    {
  Topics::iterator i=m_eveTopics.find(name);
  if ( i == m_eveTopics.end() )  {
    TEveElementList* topic = new ElementList(name.c_str(), name.c_str(), true, true);
    m_eveTopics[name] = topic;
    manager().GetEventScene()->AddElement(topic);
    return *topic;
  }
  return *((*i).second);
}

/// Access/Create a topic by name. Throws exception if the topic does not exist
TEveElementList& Display::GetEveTopic(const string& name) const   {
  Topics::const_iterator i=m_eveTopics.find(name);
  if ( i == m_eveTopics.end() )  {
    throw runtime_error("Display: Attempt to access non-existing event topic:"+name);
  }
  return *((*i).second);
}

/// Call to import geometry elements 
void Display::ImportGeo(TEveElement* el)   {
  GetGeo().AddElement(el);
}

/// Call to import geometry elements by topic
void Display::ImportGeo(const string& topic, TEveElement* el)  { 
  GetGeoTopic(topic).AddElement(el);
}

/// Call to import event elements by topic
void Display::ImportEvent(const string& topic, TEveElement* el)  { 
  GetEveTopic(topic).AddElement(el);
}

/// Call to import top level event elements 
void Display::ImportEvent(TEveElement* el)  { 
  manager().GetEventScene()->AddElement(el);
}

/// Load 'levels' Children into the geometry scene
void Display::LoadGeoChildren(TEveElement* start, int levels, bool redraw)  {
  using namespace DD4hep::Geometry;
  DetElement world = m_lcdd->world();
  if ( world.children().size() == 0 )   {
    MessageBox(INFO,"It looks like there is no\nGeometry loaded.\nNo event display availible.\n");
  }
  else if ( levels > 0 )   {
    if ( 0 == start )     {
      TEveElementList& sens = GetGeoTopic("Sensitive");
      TEveElementList& struc = GetGeoTopic("Structure");
      const DetElement::Children& c = world.children();
      
      printout(INFO,"Display","+++ Load children of %s to %d levels", 
               world.placement().name(), levels);
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
        DetElement de = (*i).second;
        SensitiveDetector sd = m_lcdd->sensitiveDetector(de.name());
        TEveElementList& parent = sd.isValid() ? sens : struc;
        pair<bool,TEveElement*> e = Utilities::LoadDetElement(de,levels,&parent);
        if ( e.second && e.first )  {
          parent.AddElement(e.second);
        }
      }
    }
    else    {
      TGeoNode* n = (TGeoNode*)start->GetUserData();
      printout(INFO,"Display","+++ Load children of %s to %d levels",Utilities::GetName(start),levels);
      if ( 0 != n )   {
        TGeoHMatrix mat;
        const char* node_name = n->GetName();
        int level = Utilities::findNodeWithMatrix(lcdd().world().placement().ptr(),n,&mat);
        if ( level > 0 )   {
          pair<bool,TEveElement*> e(false,0);
          const DetElement::Children& c = world.children();
          for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
            DetElement de = (*i).second;
            if ( de.placement().ptr() == n )  {
              e = Utilities::createEveShape(0, levels, start, n, mat, de.name());
              break;
            }
          }
          if ( !e.first && !e.second )  {
            e = Utilities::createEveShape(0, levels, start, n, mat, node_name);
          }
          if ( e.first )  { // newly created
            start->AddElement(e.second);
          }
          printout(INFO,"Display","+++ Import geometry node %s with %d levels.",node_name, levels);
        }
        else   {
          printout(INFO,"Display","+++ FAILED to import geometry node %s with %d levels.",node_name, levels);
        }
      }
      else  {
        LoadGeoChildren(0,levels,false);
      }
    }
  }
  if ( redraw )   {
    manager().Redraw3D();
  }
}

/// Make a set of nodes starting from a top element (in-)visible with a given depth
void Display::MakeNodesVisible(TEveElement* e, bool visible, int level)   {
  printout(INFO,"Display","+++ %s element %s with a depth of %d.",
           visible ? "Show" : "Hide",Utilities::GetName(e),level);
  Utilities::MakeNodesVisible(e, visible, level);
  manager().Redraw3D();
}
