// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_VIEWMENU_H
#define DD4HEP_DDEVE_VIEWMENU_H

// Framework include files
#include "DDEve/Display.h"
#include "DDEve/PopupMenu.h"

// Forward declarations
class TGMenuBar;
class TGMenuEntry;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class View;

  /** @class ViewMenu  ViewMenu.h DDEve/ViewMenu.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class ViewMenu : public PopupMenu {
  public:

  protected:
    /// Reference to display manager
    Display* m_display;
    /// Menu title
    std::string m_title;
  public:
    /// Standard constructor
    ViewMenu(Display* eve, const std::string& title);
    /// Default destructor
    virtual ~ViewMenu();

    /// Add the menu to the menu bar
    virtual void Build(TGMenuBar* bar, int hints=kLHintsNormal);

    /// Import Geometry data and build the view
    void BuildView(View *view)  const;
    /// Menu callback: Create a generic view from an XML configuration
    void CreateGenericView(TGMenuEntry* e, void* ud);
    /// Menu callback: Create a new 3D view
    void CreateView(TGMenuEntry* e, void* ud);

    /// Create a new 3D view
    View* CreateView3D(const std::string& title);
    /// Create a new R-Z view
    View* CreateRhoZProjection(const std::string& title);
    /// Create a new R-Phi view
    View* CreateRhoPhiProjection(const std::string& title);
    /// ROOT implementation macro
    ClassDef(ViewMenu,0);
  };

} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_VIEWMENU_H */

