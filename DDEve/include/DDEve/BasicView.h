// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_BASICVIEW_H
#define DD4HEP_DDEVE_BASICVIEW_H

#include "DDEve/PopupMenu.h"
#include "DDEve/ToolTip.h"

// ROOT include files
#include "TGFrame.h"
#include "TEvePad.h"

#include <vector>

// Forward declarations
class TGClient;
class TGLEmbeddedViewer;
class TGMenuBar;
class TGPopupMenu;
class TGStatusBar;
class TGLPhysicalShape;
class TGLViewerBase;
class TGListTreeItem;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class ToolTip;

  /** @class BasicView  BasicView.h DDEve/BasicView.h
   *
   * Original author: Bertrand Bellenot CERN/SFT
   *
   *
   * @author  M.Frank
   * @version 1.0
   */
  class BasicView : public TGMainFrame  {
  public:
    struct Embedded {
      bool isTrue;
      TGStatusBar* status;
    };
  protected:
   std::string             m_config;
   std::vector<PopupMenu*> m_menus;
   TEvePad                 m_pad;           // pad used as geometry container
   TGMenuBar*              m_menuBar;       // main menu bar
   TGStatusBar*            m_statusBar;     // status bar
   ToolTip*                m_toolTip;       // shaped tooltip
   TGCompositeFrame*       m_frame;
   Embedded                m_embedded;
  public:
    /// Standard constructor
   BasicView(const TGWindow *parent, unsigned int width, unsigned int height, const Embedded& embed);
   /// Default destructor
   virtual ~BasicView();

   bool isEmbedded() const {  return m_embedded.isTrue; }

   /// Build the view. Will call subsequently menu, frame creators etc.
   virtual void Build();
   /// Map the view and load sub components
   void RealizeView();

   /// User overridable callback to create the main frame for placing objects
   virtual TGCompositeFrame* CreateFrame(TGCompositeFrame& parent, unsigned int width, unsigned int height);
   /// User overridable callback to customize the view's frame
   virtual void OnCustomFrame(TGCompositeFrame& frame);
   
   /// User overridable callback to fill/modify the status bar
   virtual std::pair<TGStatusBar*,TGLayoutHints*> CreateStatusBar(TGCompositeFrame& parent);
   /// User overridable callback to customize the view's frame
   virtual void OnCustomStatusBar(TGStatusBar& bar);

   /// User overridable callback to create a menu bar associated to this view
   virtual TGMenuBar* CreateMenuBar(TGCompositeFrame& parent);
   /// User overridable callback to customize the menu bar
   virtual void OnCustomMenuBar(TGMenuBar& bar);

   /// User overridable callback to create tooltip object
   virtual ToolTip*   CreateToolTip(TGCompositeFrame& parent);
   /// User overridable callback to customize the tool tip
   virtual void OnCustomToolTip(ToolTip& tip);
   /// User overridable callback to set information before showing the tool tip
   virtual void OnShowToolTip(ToolTip& tip);

   /// Create an empty menu and add register it to the view
   virtual PopupMenu* CreateMenu();
   /// Create 'File' menu 
   virtual PopupMenu* CreateFileMenu();
   /// Create 'File' menu and add it to the menu bar
   virtual PopupMenu* CreateFileMenu(TGMenuBar& menubar);
   /// Create 'Help' menu and add it to the menu bar
   virtual PopupMenu* CreateHelpMenu();
   /// Create 'Help' menu and add it to the menu bar
   virtual PopupMenu* CreateHelpMenu(TGMenuBar& menubar);

   /// Item has been clicked, based on mouse button do:
   void ItemClicked(TGListTreeItem *item, int btn, int x, int y);

   /// Handle when the view is activated
   void OnActivateView(bool activate);
   /// Handle click events in GL viewer
   void OnClicked(TObject *obj);
   /// Slot used to handle "OnMouseIdle" signal coming from any GL viewer.
   void OnMouseIdle(TGLPhysicalShape *shape, unsigned int posx, unsigned int posy);
   /// Slot used to handle "OnMouseOver" signal coming from any GL viewer.
   void OnMouseOver(TGLPhysicalShape *shape);
   /// Open a Root file to display a geometry in the GL viewers.
   void OpenFile(const char *fname);

   /// Undock frame embedded in a splitframe (slot method).
   void Undock(TGLViewerBase *viewer);
   /// Load configuration parameters from file
   virtual void LoadConfig(const char *fname);
   /// Save configuration parameters to file
   virtual void SaveConfig(const char *fname);

   /// Message handlers

   /// File menu: Callback when opening a new file
   virtual void OnFileOpen(TGMenuEntry* entry, void* ptr);
   /// File menu: Callback when loading the configuration
   virtual void OnFileLoadConfig(TGMenuEntry* entry, void* ptr);
   /// File menu: Callback when saving the configuration
   virtual void OnFileSaveConfig(TGMenuEntry* entry, void* ptr);
   /// File menu: Callback when calling Exit
   virtual void OnFileExit(TGMenuEntry* entry, void* ptr);
   /// Help menu: Callback when calling about
   virtual void OnHelpAbout(TGMenuEntry* entry, void* ptr);

   ClassDef(BasicView,0);
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_BASICVIEW_H */

