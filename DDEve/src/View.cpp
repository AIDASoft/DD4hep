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
// Original Author: Matevz Tadel 2009 (MultiView.C)
//
//==========================================================================

// Framework include files
#include "DDEve/View.h"
#include "DDEve/Display.h"
#include "DDEve/ElementList.h"
#include "DDEve/Utilities.h"
#include "DDEve/Annotation.h"
#include "DD4hep/InstanceCount.h"

// Eve include files
#include <TEveManager.h>
#include <TEveWindow.h>
#include <TEveCalo.h>
#include <TGLViewer.h>

using namespace std;
using namespace dd4hep;

template <typename T>
static inline typename T::const_iterator find(const T& cont,const string& str)  {
  for(typename T::const_iterator i=cont.begin(); i!=cont.end(); ++i)  
    if ( (*i).name == str ) return i;
  return cont.end();
}

/// Initializing constructor
View::View(Display* eve, const string& nam)
  : m_eve(eve), m_view(0), m_geoScene(0), m_eveScene(0), m_global(0), m_name(nam), m_showGlobal(false)
{
  m_config = m_eve->GetViewConfiguration(m_name);
  InstanceCount::increment(this);
}

/// Default destructor
View::~View() {
  m_eve->UnregisterEvents(this);
  for(Topics::iterator i=m_geoTopics.begin(); i!=m_geoTopics.end(); ++i)
    (*i).second->DestroyElements();
  if ( m_geoScene ) m_geoScene->DestroyElements();
  if ( m_eveScene ) m_eveScene->DestroyElements();
  if ( m_global ) m_global->DestroyElements();
  InstanceCount::decrement(this);
}

/// Build the projection view and map it to the given slot
View& View::Build(TEveWindow* /* slot */)   {
  return *this;
}

/// Initialize the view port
void View::Initialize()  {
  TEveScene *evt = this->eveScene();
  TEveScene *geo = this->geoScene();
  if ( evt ) evt->Repaint(kTRUE);
  if ( geo ) geo->Repaint(kTRUE);
  /// Update color set
  TGLViewer* glv = viewer()->GetGLViewer();
  glv->PostSceneBuildSetup(kTRUE);
  glv->SetSmartRefresh(kFALSE);
  if ( m_eve->manager().GetViewers()->UseLightColorSet() )
    glv->UseLightColorSet();
  else
    glv->UseDarkColorSet();
  glv->RequestDraw(TGLRnrCtx::kLODHigh);
  glv->SetSmartRefresh(kTRUE);
}

/// Add the view to the global list of eve objects
TEveElementList* View::AddToGlobalItems(const string& nam)   {
  if ( 0 == m_global )   {
    m_global = new ElementList(nam.c_str(), nam.c_str(), true, true);
    if ( m_geoScene ) m_global->AddElement(geoScene());
    if ( m_eveScene ) m_global->AddElement(eveScene());
    if ( m_showGlobal ) m_eve->manager().AddToListTree(m_global,kFALSE);
  }
  return m_global;
}

/// Call an element to a event element list
TEveElement* View::ImportGeoElement(TEveElement* el, TEveElementList* list)  { 
  TEveScene* scene = dynamic_cast<TEveScene*>(el);
  if ( scene )   {
    printf("ERROR: Adding a Scene [%s] to a list. This is BAD and causes crashes!\n",scene->GetName());
  }
  if ( el ) list->AddElement(el);
  return el;
}

/// Call an element to a geometry element list
TEveElement* View::ImportGeoTopic(TEveElement* element, TEveElementList* list)   {
  return ImportGeoElement(element, list);
}

/// Call an element to a event element list
TEveElement* View::ImportEventElement(TEveElement* el, TEveElementList* list)  { 
  TEveScene* scene = dynamic_cast<TEveScene*>(el);
  if ( scene )   {
    printf("ERROR: Adding a Scene [%s] to a list. This is BAD and causes crashes!\n",scene->GetName());
  }
  if ( el )   {
    printout(INFO,"View","ImportElement %s [%s] into list: %s",
             Utilities::GetName(el), el->IsA()->GetName(), list->GetName());
    list->AddElement(el);
  }
  return el;
}

/// Access the global instance of the subdetector geometry
pair<bool,TEveElement*> 
View::GetGlobalGeometry(DetElement de, const DisplayConfiguration::Config& /* cfg */)   {
  SensitiveDetector sd = m_eve->detectorDescription().sensitiveDetector(de.name());
  TEveElementList& global = m_eve->GetGeoTopic(sd.isValid() ? "Sensitive" : "Structure");
  TEveElement* elt = global.FindChild(de.name());
  return pair<bool,TEveElement*>(true,elt);
}

/// Create a new instance of the geometry of a sub-detector
pair<bool,TEveElement*> 
View::CreateGeometry(DetElement de, const DisplayConfiguration::Config& cfg)   {
  SensitiveDetector sd = m_eve->detectorDescription().sensitiveDetector(de.name());
  TEveElementList& topic = GetGeoTopic(sd.isValid() ? "Sensitive" : "Structure");
  return Utilities::LoadDetElement(de,cfg.data.defaults.load_geo,&topic);
}

/// Configure a view using the view's name and a proper ViewConfiguration if present
void View::ConfigureGeometryFromInfo()     {
  printout(INFO,"View","+++ Configure Geometry for view %s Config=%p.",c_name(),m_config);
  (m_config) ? ConfigureGeometry(*m_config) : ConfigureGeometryFromGlobal();
  ImportGeoTopics(name());
}

/// Configure a single geometry view by default from the global geometry scene
void View::ConfigureGeometryFromGlobal()    {
  TEveElementList* l = &m_eve->GetGeoTopic("Sensitive");
  TEveElementList* t = &GetGeoTopic("Sensitive");
  for(TEveElementList::List_i i=l->BeginChildren(); i!=l->EndChildren(); ++i)
    ImportGeo(*t,*i);
    
  l = &m_eve->GetGeoTopic("Structure");
  t = &GetGeoTopic("Structure");
  for(TEveElementList::List_i i=l->BeginChildren(); i!=l->EndChildren(); ++i) 
    ImportGeo(*t,*i);
}

/// Configure a single geometry view
void View::ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)    {
  string dets;
  DisplayConfiguration::Configurations::const_iterator ic;
  float legend_y = Annotation::DefaultTextSize()+Annotation::DefaultMargin();
  const DetElement::Children& c = m_eve->detectorDescription().world().children();
  for( ic=config.subdetectors.begin(); ic != config.subdetectors.end(); ++ic)   {
    const DisplayConfiguration::Config& cfg = *ic;
    string nam = cfg.name;
    if ( nam == "global" ) {
      m_view->AddScene(m_eve->manager().GetGlobalScene());
      m_view->AddScene(m_eve->manager().GetEventScene());
      dets += nam;
    }
    else if ( cfg.type == DisplayConfiguration::CALODATA )   {
      // Note: The histogram grid must be handled like a geometry item.
      const Display::CalodataContext& ctx = m_eve->GetCaloHistogram(nam);
      if ( ctx.config.use.empty() ) ImportGeo(ctx.calo3D);
      printout(INFO,"View","+++ %s: add detector %s  %s",name().c_str(),nam.c_str(),ctx.config.use.c_str());
      Color_t col = ctx.calo3D->GetDataSliceColor(ctx.slice);
      Annotation* a = new Annotation(viewer(),nam.c_str(),Annotation::DefaultMargin(),legend_y,col);
      legend_y += a->GetTextSize();
      dets += nam + "(Calo3D)  ";
    }
    else if ( cfg.type == DisplayConfiguration::DETELEMENT )    {
      DetElement::Children::const_iterator i = c.find(nam);
      if ( i != c.end() )   {
        DetElement de = (*i).second;
        SensitiveDetector sd = m_eve->detectorDescription().sensitiveDetector(nam);
        TEveElementList& topic = GetGeoTopic(sd.isValid() ? "Sensitive" : "Structure");
        pair<bool,TEveElement*> e(false,0);
        if ( cfg.data.defaults.load_geo > 0 )       // Create a new instance
          e = CreateGeometry(de,cfg);               // with the given number of levels
        else if ( cfg.data.defaults.load_geo < 0 )  // Use the global geometry instance
          e = GetGlobalGeometry(de,cfg);            // with the given number of levels
        if ( e.first && e.second )   {
          ImportGeo(topic,e.second);
        }
        dets += nam + "(Geo)  ";
      }
    }
  }
  printout(INFO,"ViewConfiguration","+++ Configured geometry for view %s.",c_name());
  printout(INFO,"ViewConfiguration","+++ Detectors:%s", dets.c_str());
}

/// Call to import geometry topics
void View::ImportGeoTopics(const string& title)   {
  printout(INFO,"View","+++ %s: Import geometry topics.",c_name());
  for(Topics::iterator i=m_geoTopics.begin(); i!=m_geoTopics.end(); ++i)  {
    printout(INFO,"ViewConfiguration","+++     Add topic %s",(*i).second->GetName());
    ImportGeoTopic((*i).second,m_geoScene);
  }
  if ( !title.empty() ) AddToGlobalItems(title);
}

/// Call to import geometry elements by topic
void View::ImportGeo(const string& topic, TEveElement* element)  { 
  ImportGeoElement(element,&GetGeoTopic(topic));
}

/// Call to import geometry elements into topics
void View::ImportGeo(TEveElementList& topic, TEveElement* element)   {
  ImportGeoElement(element,&topic);
}

/// Call to import geometry elements
void View::ImportGeo(TEveElement* el)  { 
  ImportGeoElement(el, m_geoScene);
}

/// Configure the adding of event data 
void View::ConfigureEventFromInfo()    {
  printout(INFO,"View","+++ Import event data into view %s Config=%p.",c_name(),m_config);
  (m_config) ? ConfigureEvent(*m_config) : ConfigureEventFromGlobal();
  ImportEventTopics();
}

/// Configure an event view by default from the global event scene
void View::ConfigureEventFromGlobal()    {
  TEveElementList* l = m_eve->manager().GetEventScene();
  for(TEveElementList::List_i i=l->BeginChildren(); i!=l->EndChildren(); ++i) 
    ImportEvent(*i);
}

/// Configure a single view
void View::ConfigureEvent(const DisplayConfiguration::ViewConfig& config)  {
  DetElement world = m_eve->detectorDescription().world();
  const DetElement::Children& c = world.children();
  DisplayConfiguration::Configurations::const_iterator ic;
  for( ic=config.subdetectors.begin(); ic != config.subdetectors.end(); ++ic)  {
    const DisplayConfiguration::Config& cfg = *ic;
    string nam = cfg.name;
    if ( nam == "global" )  {
      continue;
    }
    else if ( cfg.type == DisplayConfiguration::CALODATA )  {
      continue;
    }
    else if ( cfg.type == DisplayConfiguration::COLLECTION )  {
      // Not using the global scene!
      if ( cfg.data.defaults.show_evt>0 )   {
        TEveElement* child = m_eve->manager().GetEventScene()->FindChild(nam);
        printout(INFO,"View","+++     Add collection:%s data:%p scene:%p",nam.c_str(),child,m_eveScene);
        if ( child ) ImportEvent(child);
      }
    }
    else if ( cfg.type == DisplayConfiguration::DETELEMENT )  {
      // Not using the global scene!
      DetElement::Children::const_iterator i = c.find(nam);
      if ( i != c.end() && cfg.data.defaults.show_evt>0 )  {
        SensitiveDetector sd = m_eve->detectorDescription().sensitiveDetector(nam);
        if ( sd.isValid() )  {
          // This should be configurable!
          const char* coll = sd.readout().name();
          TEveElement* child = m_eve->manager().GetEventScene()->FindChild(coll);
          printout(INFO,"View","+++     Add detector event %s collection:%s data:%p scene:%p",
                   nam.c_str(),coll,child,m_eveScene);
          if ( child ) ImportEvent(child);
        }
      }
    }
  }
}

/// Import event typics after creation
void View::ImportEventTopics()  {
}

/// Call to import event elements into the main event scene
void View::ImportEvent(TEveElement* el)  { 
  if ( m_eveScene )   {
    ImportEventElement(el, m_eveScene);
  }
}

/// Access/Create a topic by name
TEveElementList& View::GetGeoTopic(const string& nam)    {
  Topics::iterator i=m_geoTopics.find(nam);
  if ( i == m_geoTopics.end() )  {
    TEveElementList* topic = new ElementList(nam.c_str(), nam.c_str(), true, true);
    m_geoTopics[nam] = topic;
    return *topic;
  }
  return *((*i).second);
}

/// Create the geometry and the event scene
View& View::CreateScenes()  {
  // Scenes
  CreateGeoScene();
  CreateEventScene();
  return *this;
}

/// Create the event scene
View& View::CreateEventScene()   {
  if ( 0 == m_eveScene ) {
    string nam  = m_name+" - Event Data";
    string tool = m_name+" - Scene holding projected event-data for the view.";
    m_eveScene = m_eve->manager().SpawnNewScene(nam.c_str(), tool.c_str());
  }
  return *this;
}

/// Create the geometry scene
View& View::CreateGeoScene()  {
  if ( 0 == m_geoScene )   {
    string nam  = m_name+" - Geometry";
    string tool = m_name+" - Scene holding projected geometry for the view.";
    m_geoScene = m_eve->manager().SpawnNewScene(nam.c_str(), tool.c_str());
  }
  return *this;
}
 
/// Map the projection view to the slot
View& View::Map(TEveWindow* slot)  {
  slot->MakeCurrent();
  m_view = m_eve->manager().SpawnNewViewer(m_name.c_str(), "");
  if ( m_geoScene ) m_view->AddScene(m_geoScene);
  if ( m_eveScene ) m_view->AddScene(m_eveScene);
  return *this;
}
