// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_VIEWCONFIGURATION_H
#define DD4HEP_DDEVE_VIEWCONFIGURATION_H

// Framework include files
#include "DDEve/DisplayConfiguration.h"

// Forward declarations

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class View;
  class Display;

  /** @class ViewConfiguration  ViewConfiguration.h DDEve/ViewConfiguration.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class ViewConfiguration  {
  protected:
    /// The view configuration data
    DisplayConfiguration::ViewConfig config;
    /// Reference to hosting display
    Display* m_display;
  public:
    /// Standard constructor
    ViewConfiguration();
    /// Initializing constructor
    ViewConfiguration(Display* display, const DisplayConfiguration::ViewConfig& cfg);
    /// Default destructor
    virtual ~ViewConfiguration();
    /// Access to the name
    const std::string& name() const {  return config.name; }
    /// Access to the type
    const std::string& type() const {  return config.type; }
    /// Prepare the view for adding event data 
    void PrepareEvent(View* view)  const;
    /// Configure the event data display of a single view
    void ConfigureEvent(View* view) const;
    /// Configure the geometry of a single view
    void ConfigureGeometry(View* view) const;
    /// ROOT implementation macro
    ClassDef(ViewConfiguration,0);
  };

} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_VIEWCONFIGURATION_H */

