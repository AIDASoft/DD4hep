// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class Display;
  class EventHandler;

  /// class View  View.h DDEve/View.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
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
    const DisplayConfiguration::ViewConfig* m_config;

    /// The name of the view
    std::string            m_name;

    Topics m_geoTopics;
    Topics m_eveTopics;
    bool m_showGlobal;

  public:
    /// Call an element to a geometry element list
    virtual TEveElement* ImportGeoElement(TEveElement* element, TEveElementList* list);
    /// Call an element to a geometry element list
    virtual TEveElement* ImportGeoTopic(TEveElement* element, TEveElementList* list);
    /// Call an element to a event element list
    virtual TEveElement* ImportEventElement(TEveElement* element, TEveElementList* list);

  public:
    /// Initializing constructor
    View(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~View();
    /// Access to the view name/title
    const std::string& name()  const { return m_name;         }
    const char* c_name() const       { return m_name.c_str(); }
    /// Access to the Eve viewer
    TEveViewer* viewer()  const      { return m_view;         }
    /// Show global directory
    bool showGlobal() const          { return m_showGlobal;   }
    /// Set show globals
    void setShowGlobal(bool value)   { m_showGlobal = value;  }
    /// Build the view view and map it to the given slot
    virtual View& Build(TEveWindow* slot);
    /// Initialize the view port
    virtual void Initialize();
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

    /// Configure a view with geo info. Used configuration if present.
    virtual void ConfigureGeometry();
    /// Configure a single geometry view by default from the global geometry scene with all subdetectors
    virtual void ConfigureGeometryFromGlobal();
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
    /// Access/Create an geometry topic by name
    virtual TEveElementList& GetGeoTopic(const std::string& name);


    /** Manipulation of the event scene */

    /// Access to the Eve event scene
    TEveScene* eveScene() const {   return m_eveScene; }
    /// Create the event scene
    virtual View& CreateEventScene();
    /// Configure a view with event info. Used configuration if present.
    virtual void ConfigureEvent();
    /// Configure an event view by default from the global event scene
    virtual void ConfigureEventFromGlobal();
    /// Configure a single event scene view
    virtual void ConfigureEvent(const DisplayConfiguration::ViewConfig& config);
    /// Call to import event elements into the main event scene
    virtual void ImportEvent(TEveElement* element);
    /// Import event typics after creation
    virtual void ImportEventTopics();

    /// Root implementation macro
    ClassDef(View,0);
  };
} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_VIEW_H */
