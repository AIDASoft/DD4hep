// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_VIEW_H
#define DD4HEP_DDEVE_VIEW_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDEve/DisplayConfiguration.h"

// Eve include files
#include <TEveScene.h>
#include <TEveViewer.h>

// C/C++ include files
#include <map>
#include <string>

// Forward declarations
class TEveManager;
class TEveElementList;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class Display;
  class EventHandler;

  /** @class View  View.h DDEve/View.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class View   {
  public:
    typedef Geometry::LCDD LCDD;
    typedef Geometry::DetElement DetElement;
    typedef Geometry::SensitiveDetector SensitiveDetector;
    typedef std::map<std::string, TEveElementList*> Topics;
  protected:
    Display               *m_eve;
    /// Reference to the view
    TEveViewer            *m_view;
    /// Reference to the geometry scene
    TEveScene             *m_geoScene;
    /// Reference to the event scene
    TEveScene             *m_eveScene;
    /// Reference to the global item (if added
    TEveElementList       *m_global;
    /// The name of the view
    std::string            m_name;

    Topics m_geoTopics;
    Topics m_eveTopics;
  public:
    /// Call an element to a event element list
    virtual TEveElement* ImportElement(TEveElement* element, TEveElementList* list);

  public:
    /// Initializing constructor
    View(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~View();
    /// Access to the view name/title
    const std::string& name()  const { return m_name;  }
    const char* c_name() const { return m_name.c_str(); }
    /// Access to the Eve viewer
    TEveViewer* viewer()  const {   return m_view;     }
    /// Build the view view and map it to the given slot
    virtual View& Build(TEveWindow* slot);
    /// Map the view view to the slot
    virtual View& Map(TEveWindow* slot);

    /// Create the geometry and the event scene
    virtual View& CreateScenes();
    /// Add the view to the global list of eve objects
    virtual TEveElementList* AddToGlobalItems(const std::string& nam);

    /** Manipulation of the geometry scene */

    /// Access to the Eve geometry scene
    TEveScene* geoScene() const {   return m_geoScene; }
    /// Create the geometry scene
    virtual View& CreateGeoScene();
    /// Configure a single geometry view
    virtual void ConfigureGeometry(const DisplayConfiguration::ViewConfig& config);
    /// Create a new instance of the geometry of a sub-detector
    virtual std::pair<bool,TEveElement*> 
      CreateGeometry(DetElement de, const DisplayConfiguration::Config& cfg);
    /// Access the global instance of the subdetector geometry
    virtual std::pair<bool,TEveElement*> 
      GetGlobalGeometry(DetElement de, const DisplayConfiguration::Config& cfg);


    /// Call to import geometry elements into topics
    virtual void ImportGeo(const std::string& topic, TEveElement* element);
    /// Call to import geometry elements into topics
    virtual void ImportGeo(TEveElementList& topic, TEveElement* element);
    /// Call to import geometry elements into the main geometry  scene menu
    virtual void ImportGeo(TEveElement* element);
    /// Call to import geometry topics. If title is empty, do not add to global item list
    virtual void ImportGeoTopics(const std::string& title);
    /// Call to destroy all geometry elements
    virtual void DestroyGeo();

    /** Manipulation of the event scene */

    /// Access to the Eve event scene
    TEveScene* eveScene() const {   return m_eveScene; }
    /// Create the event scene
    virtual View& CreateEventScene();
    /// Configure a single event scene view
    virtual void ConfigureEvent(const DisplayConfiguration::ViewConfig& config);
    /// Call to import event elements into the main event scene
    virtual void ImportEvent(TEveElement* element);
    /// Import event data from event handler for a given subdetector
    virtual void ImportEvent(EventHandler& hdlr, 
			     DetElement det, 
			     SensitiveDetector sd,
			     const DisplayConfiguration::ViewConfig& config);
    /// Import event typics after creation
    virtual void ImportEventTopics();
    /// Call to destroy all event elements
    virtual void DestroyEvent();
    /// Prepare the view before loading new event data
    virtual void PrepareEvent();

    /// Access/Create an geometry topic by name
    virtual TEveElementList& GetGeoTopic(const std::string& name);
#if 0
    /// Access/Create an event topic by name
    virtual TEveElementList& GetEveTopic(const std::string& name);
    /// Call to import event elements into topics
    virtual void ImportEvent(const std::string& topic, TEveElement* element);
    void Finalize();
#endif
    /// Root implementation macro
    ClassDef(View,0);
  };
} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_VIEW_H */
