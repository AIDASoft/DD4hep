// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_SPLITVIEW_H
#define DD4HEP_DDEVE_SPLITVIEW_H

#include "DDEve/BasicView.h"

// ROOT include files
#include "TGFrame.h"
#include "TGLViewer.h"

#include <vector>

// Forward declarations
class TGSplitFrame;
class TEveViewer;
class TEveProjectionManager;
class TGListTreeItem;
class TGLViewerBase;
class TEveManager;
class TEveElement;
class TEveScene;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  class ToolTip;
  class EmbeddedView;

  /** @class SplitView  SplitView.h DDEve/SplitView.h
   *
   * Original author: Bertrand Bellenot CERN/SFT
   *
   *
   * @author  M.Frank
   * @version 1.0
   */
  class SplitView : public BasicView  {
  public:
    typedef std::vector<EmbeddedView*> Views;
  protected:
    TGLEmbeddedViewer     *m_activeViewer;  // actual (active) GL viewer
    PopupMenu* m_cameraMenu;
    PopupMenu* m_sceneMenu;
    Views                  m_views;
    TEveManager*           m_eve;
    TEveElement*           m_geom;
    int m_GLOrthoDolly, m_GLOrthoRotate;

  public:
    /// Standard constructor
    SplitView(TEveManager* m, TEveElement* geom, const TGWindow *p, unsigned int w, unsigned int h, const Embedded& embed);
    /// Default destructor
    virtual ~SplitView();

    Views& views() { return m_views; }
    TGSplitFrame* splitFrame() const { return (TGSplitFrame*)m_frame;} 

    /// Create a dockable viewer with "swap" and "undock" buttons
    EmbeddedView* CreateView(TGCompositeFrame* frame, TGLViewer::ECameraType camera_typ);
    /// Destroy view 
    void DestroyView(EmbeddedView*& view);

    /// User overridable callback to create the main frame for placing objects
    virtual TGCompositeFrame* CreateFrame(TGCompositeFrame& parent, unsigned int width, unsigned int height);
    /// User overridable callback to customize the view's frame
    virtual void OnCustomFrame(TGCompositeFrame& frame);

    /// User overridable callback to create a menu bar associated to this view
    virtual TGMenuBar* CreateMenuBar(TGCompositeFrame& parent);
    /// User overridable callback to customize the menu bar
    virtual void OnCustomMenuBar(TGMenuBar& bar);
    /// User overridable callback to create tooltip object
    virtual ToolTip*   CreateToolTip(TGCompositeFrame& parent);

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

    /// Message handlers

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

    ClassDef(SplitView,0);
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_SPLITVIEW_H */

