// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_PROJECTIONVIEW_H
#define DD4HEP_DDEVE_PROJECTIONVIEW_H

#include "DDEve/PopupMenu.h"
#include "DDEve/ToolTip.h"

// ROOT include files
#include "TGFrame.h"

// Forward declarations
class TGClient;
class TEvePad;
class TGSplitFrame;
class TGLEmbeddedViewer;
class TGMenuBar;
class TGPopupMenu;
class TGStatusBar;
class TEveViewer;
class TEveProjectionManager;
class TGListTreeItem;
class TGLPhysicalShape;
class TGLViewerBase;
class TEveManager;
class TEveElement;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class ToolTip;

  /** @class ProjectionView  ProjectionView.h DDEve/ProjectionView.h
   *
   * Original author: Bertrand Bellenot CERN/SFT
   *
   *
   * @author  M.Frank
   * @version 1.0
   */
  class ProjectionView : public TGMainFrame  {
  public:
  protected:
   TEvePad               *m_pad;           // pad used as geometry container
   TGSplitFrame          *m_splitFrame;    // main (first) split frame
   TGLEmbeddedViewer     *m_viewer0;       // main GL viewer
   TGLEmbeddedViewer     *m_viewer1;       // first GL viewer
   TGLEmbeddedViewer     *m_viewer2;       // second GL viewer
   TGLEmbeddedViewer     *m_activeViewer;  // actual (active) GL viewer
   TGMenuBar             *m_menuBar;       // main menu bar

   PopupMenu              m_fileMenu;      // 'File' popup menu
   PopupMenu              m_helpmenu;      // 'Help' popup menu

   PopupMenu              m_cameraMenu;    // 'Camera' popup menu
   PopupMenu              m_sceneMenu;     // 'Scene' popup menu
   TGStatusBar           *m_statusBar;     // status bar
   ToolTip                m_toolTip;       // shaped tooltip
   Bool_t                 m_isEmbedded;

   TEveViewer            *m_viewer[3];
   TEveProjectionManager *m_rphiMgr;
   TEveProjectionManager *m_rhozMgr;
   TEveManager*           m_eve;
   TEveElement*           m_geom;
   int m_GLOrthoDolly, m_GLOrthoRotate;
  public:
    /// Standard constructor
   ProjectionView(TEveManager* m, TEveElement* geom, const TGWindow *p, unsigned int w=800, unsigned int h=600, bool embed=false);
   /// Default destructor
   virtual ~ProjectionView();

   /// Item has been clicked, based on mouse button do:
   void           ItemClicked(TGListTreeItem *item, int btn, int x, int y);
   /// Handle click events in GL viewer
   void           OnClicked(TObject *obj);
   /// Slot used to handle "OnMouseIdle" signal coming from any GL viewer.
   void           OnMouseIdle(TGLPhysicalShape *shape, unsigned int posx, unsigned int posy);
   /// Slot used to handle "OnMouseOver" signal coming from any GL viewer.
   void           OnMouseOver(TGLPhysicalShape *shape);
   /// Slot used to handle "Activated" signal coming from any GL viewer.
   void           OnViewerActivated();
   /// Open a Root file to display a geometry in the GL viewers.
   void           OpenFile(const char *fname);

   void           SwapToMainView(TGLViewerBase *viewer);
   /// Toggle state of the 'Ortho allow rotate' menu entry.
   void           ToggleOrthoRotate();
   /// Toggle state of the 'Ortho allow dolly' menu entry.
   void           ToggleOrthoDolly();
   /// Undock frame embedded in a splitframe (slot method).
   void           Undock(TGLViewerBase *viewer);
   /// Load configuration parameters from file
   void           LoadConfig(const char *fname);
   /// Save configuration parameters to file
   void           SaveConfig(const char *fname);

   /// Update displays and projections
   virtual void update();
   /// Add axis to the projections
   virtual void defineAxis(TEveProjectionManager* projection,const std::string& title,int num_divisions);

   TEveProjectionManager *rphiMgr() const { return m_rphiMgr; }
   TEveProjectionManager *rhozMgr() const { return m_rhozMgr; }


   /// Message handlers

   /// File menu: Callback when opening a new file
   void OnFileOpen(TGMenuEntry* entry, void* ptr);
   /// File menu: Callback when loading the configuration
   void OnFileLoadConfig(TGMenuEntry* entry, void* ptr);
   /// File menu: Callback when saving the configuration
   void OnFileSaveConfig(TGMenuEntry* entry, void* ptr);
   /// File menu: Callback when calling Exit
   void OnFileExit(TGMenuEntry* entry, void* ptr);

   /// Help menu: Callback when calling about
   void OnHelpAbout(TGMenuEntry* entry, void* ptr);

   /// Scene menu: Callback when calling update
   void OnSceneUpdate(TGMenuEntry* entry, void* ptr);
   /// Scene menu: Callback when calling update all
   void OnSceneUpdateAll(TGMenuEntry* entry, void* ptr);

   /// Camera menu: Changing to requested perspective
   void OnCameraGLPerspXOZ(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLPerspYOZ(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLPerspXOY(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLXOY(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLXOZ(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLZOY(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLOrthoRotate(TGMenuEntry* entry, void* ptr);
   /// Camera menu: Changing to requested perspective
   void OnCameraGLOrthoDolly(TGMenuEntry* entry, void* ptr);

   ClassDef(ProjectionView,0);
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_PROJECTIONVIEW_H */

