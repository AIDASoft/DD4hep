// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Primitives.h"
#include "DDEve/EmbeddedView.h"
#include "TGLOverlayButton.h"
#include "TGLEmbeddedViewer.h"

#include "TEveProjectionManager.h"
#include "TEveProjectionAxes.h"
#include "TEveManager.h"
#include "TEveScene.h"
#include "TEvePad.h"

#include <stdexcept>
#include <iostream>

using namespace std;
using namespace DD4hep;

ClassImp(EmbeddedView)

/// Standard constructor
EmbeddedView::EmbeddedView(TEveManager* mgr) : m_eve(mgr), m_view(0), m_frame(0), m_eveView(0), m_projMgr(0)
{
}

/// Default destructor
EmbeddedView::~EmbeddedView()   {
  deletePtr(m_eveView);
  deletePtr(m_view);
}

/// Access frame
TGCompositeFrame* EmbeddedView::GetFrame() const { 
  if ( m_view )  {
    return m_view->GetFrame();
  }
  invalidView("GetFrame"); 
  return 0;
}

/// Exception callback if the view is invalid
EmbeddedView& EmbeddedView::invalidView(const std::string& tag)  const {
  throw runtime_error("EmbeddedView:"+tag+": Attempt to add invalid view!");
}

/// Create the embedded viewer
EmbeddedView& EmbeddedView::Create(TEvePad& pad, TGCompositeFrame* frm, TGLViewer::ECameraType camera_typ)   {
  m_frame = frm;
  m_view = new TGLEmbeddedViewer(frm, &pad);
  m_frame->AddFrame(m_view->GetFrame(),new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  m_view->SetCurrentCamera(camera_typ);
  return *this;
}

/// Set the view dockable and add buttons to undock/swap
EmbeddedView& EmbeddedView::SetDockable(Callback swap, Callback undock)   {
  if ( m_view )  {
    TGLOverlayButton* b = new TGLOverlayButton(m_view, "Swap", 10.0, -10.0, 55.0, 16.0);
    b->Connect("Clicked(TGLViewerBase*)", "EmbeddedView", this, "Swap(TGLViewerBase*)");
    b = new TGLOverlayButton(m_view, "Undock", 70.0, -10.0, 55.0, 16.0);
    b->Connect("Clicked(TGLViewerBase*)", "EmbeddedView", this, "Undock(TGLViewerBase*)");
    m_undock = undock;
    m_swap = swap;
    return *this;
  }
  return invalidView("SetDockable");
}

/// Add the embedded viewer to Eve
EmbeddedView& EmbeddedView::Attach(TEveViewerList* l)   {
  if ( m_view )  {
    TEveViewer* e = new TEveViewer(m_frame->GetName());
    e->SetGLViewer(m_view, m_frame);
    e->IncDenyDestroy();
    if ( l ) l->AddElement(e);
    m_eveView = e;
    return *this;
  }
  return invalidView("AttachToEve");
}

/// Create (and add) projection scene to embedded viewer
TEveProjectionManager* EmbeddedView::Project(const string& projection_title, TEveProjection::EPType_e projection_type)  {
  if ( m_view )  {
    m_projMgr = new TEveProjectionManager(projection_type);
    TEveProjectionAxes* a = new TEveProjectionAxes(m_projMgr);
    TEveScene* s = m_eve->SpawnNewScene(projection_title.c_str());
    a->SetNdivisions(3);
    s->AddElement(m_projMgr);
    s->AddElement(a);
    m_eveView->AddScene(s);
    m_eve->AddToListTree(m_projMgr, kTRUE);
    return m_projMgr;
  }
  invalidView("Project");
  return 0;
}

/// Add scene to the embedded viewer
EmbeddedView& EmbeddedView::AddScene(TEveScene* s)  {
  if ( m_view )  {
    m_eveView->AddScene(s);
    return *this;
  }
  return invalidView("AddScene");
}

/// Update scene
void EmbeddedView::UpdateScene()   {
  m_view->UpdateScene();
}

/// Swap frame embedded in a splitframe to the main view (slot method).
void EmbeddedView::Swap(TGLViewerBase *viewer)  {
  const void* args[1] = {&viewer}; 
  m_swap.execute(args);
}

/// Undock frame embedded in a splitframe (slot method).
void EmbeddedView::Undock(TGLViewerBase *viewer)   {
  const void* args[1] = {&viewer}; 
  m_undock.execute(args);
}
