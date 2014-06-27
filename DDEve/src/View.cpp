// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
// Framework include files
#include "DDEve/View.h"
#include "DDEve/Display.h"
#include "DDEve/ElementList.h"
#include "DDEve/EventHandler.h"
#include "DDEve/Utilities.h"
#include "DD4hep/InstanceCount.h"

// Eve include files
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveWindow.h>
#include <TGLViewer.h>

using namespace std;
using namespace DD4hep;

template <typename T>
static inline typename T::const_iterator find(const T& c,const string& s)  {
  for(typename T::const_iterator i=c.begin(); i!=c.end(); ++i)  
    if ( (*i).name == s ) return i;
  return c.end();
}


/// Initializing constructor
View::View(Display* eve, const string& name)
  : m_eve(eve), m_view(0), m_geoScene(0), m_eveScene(0), m_global(0), m_name(name)
{
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

/// Add the view to the global list of eve objects
TEveElementList* View::AddToGlobalItems(const string& nam)   {
  if ( 0 == m_global )   {
    m_global = new ElementList(nam.c_str(), nam.c_str(), true, true);
    if ( m_geoScene ) m_global->AddElement(geoScene());
    if ( m_eveScene ) m_global->AddElement(eveScene());
    m_eve->manager().AddToListTree(m_global,kFALSE);
  }
  return m_global;
}

/// Call an element to a event element list
TEveElement* View::ImportElement(TEveElement* el, TEveElementList* list)  { 
  list->AddElement(el);
  return el;
}

/// Access the global instance of the subdetector geometry
pair<bool,TEveElement*> 
View::GetGlobalGeometry(DetElement de, const DisplayConfiguration::Config& /* cfg */)   {
  SensitiveDetector sd = m_eve->lcdd().sensitiveDetector(de.name());
  TEveElementList& global = m_eve->GetGeoTopic(sd.isValid() ? "Sensitive" : "Structure");
  TEveElement* elt = global.FindChild(de.name());
  return pair<bool,TEveElement*>(true,elt);
}

/// Create a new instance of the geometry of a sub-detector
pair<bool,TEveElement*> 
View::CreateGeometry(DetElement de, const DisplayConfiguration::Config& cfg)   {
  SensitiveDetector sd = m_eve->lcdd().sensitiveDetector(de.name());
  TEveElementList& topic = GetGeoTopic(sd.isValid() ? "Sensitive" : "Structure");
  return Utilities::LoadDetElement(de,cfg.data.defaults.load_geo,&topic);
}

/// Configure a single geometry view
void View::ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)    {
  string dets;
  const DetElement::Children& c = m_eve->lcdd().world().children();
  for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
    DetElement de = (*i).second;
    SensitiveDetector sd = m_eve->lcdd().sensitiveDetector(de.name());
    if ( (sd.isValid() && config.show_sensitive) || (!sd.isValid() && config.show_structure) )   {
      DisplayConfiguration::Configurations::const_iterator i = find(config.subdetectors,de.name());
      if ( i != config.subdetectors.end() )  {
	TEveElementList& topic = GetGeoTopic(sd.isValid() ? "Sensitive" : "Structure");
	const DisplayConfiguration::Config& cfg = *i;
	pair<bool,TEveElement*> e(false,0);
	if ( cfg.data.defaults.load_geo > 0 )       // Create a new instance
	  e = CreateGeometry(de,cfg);               // with the given number of levels
	else if ( cfg.data.defaults.load_geo < 0 )  // Use the global geometry instance
	  e = GetGlobalGeometry(de,cfg);            // with the given number of levels
	if ( e.first && e.second )   {
	  dets += de.name();
	  dets += "  ";
	  ImportGeo(topic,e.second);
	}
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
    ImportElement((*i).second,m_geoScene);
  }
  if ( !title.empty() ) AddToGlobalItems(title);
}

/// Call to import geometry elements by topic
void View::ImportGeo(const string& topic, TEveElement* element)  { 
  ImportElement(element,&GetGeoTopic(topic));
}

/// Call to import geometry elements into topics
void View::ImportGeo(TEveElementList& topic, TEveElement* element)   {
  ImportElement(element,&topic);
}

/// Call to import geometry elements
void View::ImportGeo(TEveElement* el)  { 
  ImportElement(el, m_geoScene);
}

/// Call to destroy all event elements
void View::DestroyGeo()  {
  m_geoScene->DestroyElements();
}


/// Configure a single view
void View::ConfigureEvent(const DisplayConfiguration::ViewConfig& config)    {
  LCDD& lcdd = m_eve->lcdd();
  DetElement world = lcdd.world();
  EventHandler& handler = m_eve->eventHandler();
  const DetElement::Children& c = world.children();

  printout(INFO,"View","+++ Configure event for view %s.",c_name());
  for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
    DetElement de = (*i).second;
    SensitiveDetector sd = lcdd.sensitiveDetector(de.name());
    if ( sd.isValid() )  {
      DisplayConfiguration::Configurations::const_iterator i=find(config.subdetectors,de.name());
      if ( i != config.subdetectors.end() )  {
	ImportEvent(handler, de, sd, config);
      }
    }
  }
}

/// Import event data from event handler for a given subdetector
void View::ImportEvent(EventHandler& /* hdlr */, 
		       DetElement de, 
		       SensitiveDetector sd,
		       const DisplayConfiguration::ViewConfig& /* config */)
{
  ///
  /// By default we reuse the "global" event. We do not want to recreate the data
  /// unless really necessary....
  ///
  const char* coll = sd.readout().name();
  TEveElement* child = m_eve->GetEve().FindChild(coll);
  printout(INFO,"View","+++     Add detector event %s collection:%s data:%p",
	   de.name(), coll, child);
  if ( child )   {
    ImportEvent(child);
  }
}

/// Import event typics after creation
void View::ImportEventTopics()  {
}

/// Call to import event elements into the main event scene
void View::ImportEvent(TEveElement* el)  { 
  ImportElement(el, m_eveScene);
}

/// Call to destroy all event elements
void View::DestroyEvent()  {
  m_eveScene->DestroyElements();
}

/// Prepare the view before loading new event data
void View::PrepareEvent()  {
}

/// Access/Create a topic by name
TEveElementList& View::GetGeoTopic(const string& name)    {
  Topics::iterator i=m_geoTopics.find(name);
  if ( i == m_geoTopics.end() )  {
    TEveElementList* topic = new ElementList(name.c_str(), name.c_str(), true, true);
    m_geoTopics[name] = topic;
    return *topic;
  }
  return *((*i).second);
}

#if 0
/// Access/Create a topic by name
TEveElementList& View::GetEveTopic(const string& name)    {
  Topics::iterator i=m_eveTopics.find(name);
  if ( i == m_eveTopics.end() )  {
    TEveElementList* topic = new TEveElementList(name.c_str(), name.c_str(), true, true);
    m_eveTopics[name] = topic;
    return *topic;
  }
  return *((*i).second);
}

/// Call to import event elements by topic
void View::ImportEvent(const string& topic, TEveElement* el)  { 
  GetEveTopic(topic).AddElement(el);
}
#endif

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
    string name = m_name+" - Event Data";
    string tool = m_name+" - Scene holding projected event-data for the view.";
    m_eveScene = m_eve->manager().SpawnNewScene(name.c_str(), tool.c_str());
  }
  return *this;
}

/// Create the geometry scene
View& View::CreateGeoScene()  {
  if ( 0 == m_geoScene )   {
    string name = m_name+" - Geometry";
    string tool = m_name+" - Scene holding projected geometry for the view.";
    m_geoScene = m_eve->manager().SpawnNewScene(name.c_str(), tool.c_str());
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
