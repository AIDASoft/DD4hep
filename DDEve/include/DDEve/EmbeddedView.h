// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_EMBEDDEDVIEW_H
#define DD4HEP_DDEVE_EMBEDDEDVIEW_H

// Framework include files
#include "DD4hep/Callback.h"
#include "TGLViewer.h"
#include "TEveViewer.h"
#include "TEveProjections.h"
#include <map>

// Forward declarations
class TEveProjectionManager;
class TGLEmbeddedViewer;
class TGCompositeFrame;
class TEveViewerList;
class TEveManager;
class TEveViewer;
class TEveScene;
class TEvePad;


/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class EmbeddedView  EmbeddedView.h DDEve/EmbeddedView.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class EmbeddedView   {
  public:
    typedef std::map<std::string,Callback> Slots;
  protected:
    //Slots m_slots;
    Callback m_swap;
    Callback m_undock;
    TEveManager* m_eve;
    TGLEmbeddedViewer* m_view;
    TGCompositeFrame* m_frame;
    TEveViewer* m_eveView;
    TEveProjectionManager* m_projMgr;
  public:
    /// Standard constructor
    EmbeddedView(TEveManager* mgr);
    /// Default destructor
    virtual ~EmbeddedView();
    /// Access to the created view
    TGLEmbeddedViewer* viewer() const {  return m_view; }
    /// Access to the TEve viewer (if present)
    TEveViewer* eveViewer() const { return m_eveView; }
    /// Access to the projection manager 
    TEveProjectionManager* projection() const { return m_projMgr; }
    /// Access frame object
    TGCompositeFrame* GetFrame() const;
    

    /// Exception callback if the view is invalid
    EmbeddedView& invalidView(const std::string& tag) const;

    /// Create the embedded viewer
    virtual EmbeddedView& Create(TEvePad& pad, TGCompositeFrame* frm, TGLViewer::ECameraType camera_typ);

    /// Create (and add) projection scene to embedded viewer
    TEveProjectionManager* Project(const std::string& projection_title, TEveProjection::EPType_e projection_type);

    /// Set the view dockable and add buttons to undock/swap
    virtual EmbeddedView& SetDockable(Callback swap, Callback undock);

    /// Add the embedded viewer to Eve
    EmbeddedView& Attach(TEveViewerList* l=0);

    /// Set the view dockable and add buttons to undock/swap
    template <typename T> EmbeddedView& 
      SetDockable(T* p, void (T::*swp)(TGLViewerBase*), void (T::*undock)(TGLViewerBase*))  {
      Callback cb(p);
      return SetDockable(cb.make(swp),cb.make(undock));
    }

    /// Add scene to the embedded viewer
    virtual EmbeddedView& AddScene(TEveScene* scene);

    /// Connect to a QT event slot
    //EmbeddedView& Connect(const std::string& slot, Callback cb);

    /// Update scene
    virtual void UpdateScene();

    /// Swap frame embedded in a splitframe to the main view (slot method).
    void Swap(TGLViewerBase *viewer);

    /// Undock frame embedded in a splitframe (slot method).
    void Undock(TGLViewerBase *viewer);

    ClassDef(EmbeddedView,0);
  };

}      /* End namespace DD4hep        */
#endif /* DD4HEP_DDEVE_EMBEDDEDVIEW_H */

