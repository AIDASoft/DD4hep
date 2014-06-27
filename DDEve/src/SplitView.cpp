// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"

#include "DDEve/SplitView.h"
#include "DDEve/EmbeddedView.h"

// ROOT include files
#include "TApplication.h"
#include "TSystem.h"
#include "TClass.h"
#include "TGeoManager.h"
#include "TEnv.h"
#include "TGTab.h"
#include "TGButton.h"
#include "TGMsgBox.h"
#include "TGListTree.h"
#include "TGFileDialog.h"
#include "TGSplitFrame.h"

#include "TGLWidget.h"
#include "TGLEmbeddedViewer.h"
#include "TGLOverlayButton.h"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveProjectionManager.h"
#include "TEveSelection.h"
#include "TEveEventManager.h"

#include <iostream>

using namespace std;
using namespace DD4hep;

ClassImp(SplitView)

/// Standard constructor
SplitView::SplitView(TEveManager* m, TEveElement* geom, 
		     const TGWindow *parent, unsigned int width, unsigned int height, const Embedded& embed)
: BasicView(parent, width, height, embed), 
  m_activeViewer(0), m_cameraMenu(0), m_sceneMenu(0),
  m_eve(m), m_geom(geom)
{
}

/// User overridable callback to create a menu bar associated to this view
TGMenuBar* SplitView::CreateMenuBar(TGCompositeFrame& parent)  {
  return new TGMenuBar(&parent, 1, 1, kHorizontalFrame);
}

/// User overridable callback to customize the menu bar
void SplitView::OnCustomMenuBar(TGMenuBar& bar)   {
  m_cameraMenu = CreateMenu();
  m_cameraMenu->AddEntry("Perspective (Floor XOZ)", this, &SplitView::OnCameraGLPerspXOZ);
  m_cameraMenu->AddEntry("Perspective (Floor YOZ)", this, &SplitView::OnCameraGLPerspYOZ);
  m_cameraMenu->AddEntry("Perspective (Floor XOY)", this, &SplitView::OnCameraGLPerspXOY);
  m_cameraMenu->AddEntry("Orthographic (XOY)", this, &SplitView::OnCameraGLXOY);
  m_cameraMenu->AddEntry("Orthographic (XOZ)", this, &SplitView::OnCameraGLXOZ);
  m_cameraMenu->AddEntry("Orthographic (ZOY)", this, &SplitView::OnCameraGLZOY);
  m_cameraMenu->AddSeparator();
  m_GLOrthoDolly = m_cameraMenu->AddEntry("Ortho allow rotate", this, &SplitView::OnCameraGLOrthoRotate);
  m_GLOrthoRotate = m_cameraMenu->AddEntry("Ortho allow dolly", this, &SplitView::OnCameraGLOrthoDolly);

  m_sceneMenu = CreateMenu();
  m_sceneMenu->AddEntry("&Update Current", this, &SplitView::OnSceneUpdate);
  m_sceneMenu->AddEntry("Update &All", this, &SplitView::OnSceneUpdateAll);
  
  CreateFileMenu(bar);
  bar.AddPopup("&Camera", *m_cameraMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  bar.AddPopup("&Scene", *m_sceneMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  CreateHelpMenu(bar);
}

/// User overridable callback to create the main frame for placing objects
TGCompositeFrame* SplitView::CreateFrame(TGCompositeFrame& parent, unsigned int width, unsigned int height)  {
  return new TGSplitFrame(&parent, width, height);
}

/// User overridable callback to customize the view's frame
void SplitView::OnCustomFrame(TGCompositeFrame& frame)   {
  bool  modify_scenes = isEmbedded() && m_eve;
  TGSplitFrame* splitter = (TGSplitFrame*)&frame;
  splitter->HSplit(434);  // split it once
  splitter->GetSecond()->VSplit(266);  // split lower part again (will make three parts)

  m_views.resize(3,0);
  // get top (main) split frame
  TGSplitFrame* frm = splitter->GetFirst();
  frm->SetName("Main_View");

  // create (embed) a GL viewer inside and set the camera to perspective (XOZ)
  m_views[0] = CreateView(frm,TGLViewer::kCameraPerspXOZ);

  if ( modify_scenes ) {
    m_views[0]->AddScene(m_eve->GetGlobalScene());
    m_views[0]->AddScene(m_eve->GetEventScene());
  }

  // get bottom left split frame
  frm = splitter->GetSecond()->GetFirst();
  frm->SetName("Bottom_Left");
  // create (embed) a GL viewer inside and set the camera to orthographic (XOY)
  m_views[1] = CreateView(frm,TGLViewer::kCameraOrthoXOY);
  if ( modify_scenes )   {
    m_views[1]->Project("Rho-Z Projection",TEveProjection::kPT_RhoZ);
  }

  // get bottom center split frame
  frm = splitter->GetSecond()->GetSecond();
  frm->SetName("Bottom_Center");
  // create (embed) a GL viewer inside and set the camera to orthographic (XOY)
  m_views[2] = CreateView(frm,TGLViewer::kCameraOrthoXOY);
  if ( modify_scenes ) {
    m_views[2]->Project("R-Phi Projection",TEveProjection::kPT_RPhi);
  }

  if ( modify_scenes ) {
    m_eve->GetListTree()->Connect("Clicked(TGListTreeItem*, int, int, int)",
				  "SplitView", this, "ItemClicked(TGListTreeItem*, int, int, int)");
  }
}

/// Create a dockable viewer with "swap" and "undock" buttons
EmbeddedView* SplitView::CreateView(TGCompositeFrame* frm, TGLViewer::ECameraType camera_typ)   {
  EmbeddedView* view = new EmbeddedView(m_eve);
  view->Create(m_pad,frm,camera_typ)
    .SetDockable(this,&SplitView::SwapToMainView,&SplitView::Undock)
    .Attach(m_eve->GetViewers())
    ;
  //cout << "Attach:" << (void*)view->GetFrame() << " -> " << view->GetFrame()->GetName() << endl;
  //cout << "Attach:" << (void*)frm << " -> " << frm->GetName() << endl;
  TGLEmbeddedViewer* v = view->viewer();
  // connect signal we are interested to
  v->Connect("MouseOver(TGLPhysicalShape*)", "BasicView", this, "OnMouseOver(TGLPhysicalShape*)");
  v->Connect("Activated()", "SplitView", this, "OnViewerActivated()");
  v->Connect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)", "BasicView", this, 
	     "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  v->Connect("Clicked(TObject*)", "BasicView", this, "OnClicked(TObject*)");
  return view;
}

/// Destroy view 
void SplitView::DestroyView(EmbeddedView*& view)   {
  if ( view )  {
    TGLEmbeddedViewer* v = view->viewer();
    v->Disconnect("MouseOver(TGLPhysicalShape*)", this, "OnMouseOver(TGLPhysicalShape*)");
    v->Disconnect("Activated()", this, "OnViewerActivated()");
    v->Disconnect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)", this, "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
    if ( !isEmbedded() )  {
      delete view;
    }
    view = 0;
  }
}

/// Default destructor
SplitView::~SplitView()   {  // Clean up main frame...
  for(Views::iterator i=m_views.begin(); i!=m_views.end(); ++i)
    DestroyView(*i);
}

/// User overridable callback to create tooltip object
ToolTip* SplitView::CreateToolTip(TGCompositeFrame& /* parent */)   {
  return new ToolTip("Default.png", 120, 22, 160, 110, 23, 115, 12, "#ffff80");
}

/// Scene menu: Callback when calling update
void SplitView::OnSceneUpdate(TGMenuEntry* /* entry */, void* /* ptr */)   {
  if (m_activeViewer)
    m_activeViewer->UpdateScene();
}

/// Scene menu: Callback when calling update all
void SplitView::OnSceneUpdateAll(TGMenuEntry* /* entry */, void* /* ptr */)   {
  for(Views::iterator i=m_views.begin(); i!=m_views.end(); ++i)
    (*i)->UpdateScene();
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLPerspXOZ(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraPerspYOZ);
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLPerspYOZ(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLPerspXOY(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLXOY(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLXOZ(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLZOY(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLOrthoRotate(TGMenuEntry* /* entry */, void* /* ptr */)  {
  ToggleOrthoRotate();
}

/// Camera menu: Changing to requested perspective
void SplitView::OnCameraGLOrthoDolly(TGMenuEntry* /* entry */, void* /* ptr */)  {
  ToggleOrthoDolly();
}


/// Slot used to handle "Activated" signal coming from any GL viewer.
void SplitView::OnViewerActivated()   {
  // Slot used to handle "Activated" signal coming from any GL viewer.
  // Used to know which GL viewer is active.

  static Pixel_t green = 0;
  // set the actual GL viewer frame to default color
  if (m_activeViewer && m_activeViewer->GetFrame())
    m_activeViewer->GetFrame()->ChangeBackground(GetDefaultFrameBackground());

  // change the actual GL viewer to the one who emitted the signal
  // m_activeViewer = (TGLEmbeddedViewer *)gTQSender;
  m_activeViewer = dynamic_cast<TGLEmbeddedViewer*>((TQObject*)gTQSender);

  if (m_activeViewer == 0) {
    printf ("dyncast failed ...\n");
    return;
  }

  // get the highlight color (only once)
  if (green == 0) {
    gClient->GetColorByName("green", green);
  }
  // set the new actual GL viewer frame to highlight color
  if (m_activeViewer->GetFrame())
    m_activeViewer->GetFrame()->ChangeBackground(green);

  // update menu entries to match actual viewer's options
  if (m_activeViewer->GetOrthoXOYCamera()->GetDollyToZoom() &&
      m_activeViewer->GetOrthoXOZCamera()->GetDollyToZoom() &&
      m_activeViewer->GetOrthoZOYCamera()->GetDollyToZoom())
    m_cameraMenu->UnCheckEntry(m_GLOrthoDolly);
  else
    m_cameraMenu->CheckEntry(m_GLOrthoDolly);

  if (m_activeViewer->GetOrthoXOYCamera()->GetEnableRotate() &&
      m_activeViewer->GetOrthoXOYCamera()->GetEnableRotate() &&
      m_activeViewer->GetOrthoXOYCamera()->GetEnableRotate())
    m_cameraMenu->CheckEntry(m_GLOrthoRotate);
  else
    m_cameraMenu->UnCheckEntry(m_GLOrthoRotate);
}

/// Open a Root file to display a geometry in the GL viewers.
void SplitView::OpenFile(const char *fname)   {
  TString filename = fname;
  // check if the file type is correct
  if (!filename.EndsWith(".root")) {
    new TGMsgBox(GetParent(), this, "OpenFile",
		 Form("The file \"%s\" is not a root file!", fname),
		 kMBIconExclamation, kMBOk);
    return;
  }
  // check if the root file contains a geometry
  if (TGeoManager::Import(fname) == 0) {
    new TGMsgBox(GetParent(), this, "OpenFile",
		 Form("The file \"%s\" does't contain a geometry", fname),
		 kMBIconExclamation, kMBOk);
    return;
  }
  gGeoManager->DefaultColors();
  // delete previous primitives (if any)
  m_pad.GetListOfPrimitives()->Delete();
  // and add the geometry to eve pad (container)
  m_pad.GetListOfPrimitives()->Add(gGeoManager->GetTopVolume());
  // paint the geometry in each GL viewer
  for(Views::iterator i=m_views.begin(); i!=m_views.end(); ++i)
    (*i)->viewer()->PadPaint(&m_pad);
}

/// Toggle state of the 'Ortho allow rotate' menu entry.
void SplitView::ToggleOrthoRotate()  {
  if (m_cameraMenu->IsEntryChecked(m_GLOrthoRotate))
    m_cameraMenu->UnCheckEntry(m_GLOrthoRotate);
  else
    m_cameraMenu->CheckEntry(m_GLOrthoRotate);
  Bool_t state = m_cameraMenu->IsEntryChecked(m_GLOrthoRotate);
  if (m_activeViewer) {
    m_activeViewer->GetOrthoXOYCamera()->SetEnableRotate(state);
    m_activeViewer->GetOrthoXOYCamera()->SetEnableRotate(state);
    m_activeViewer->GetOrthoXOYCamera()->SetEnableRotate(state);
  }
}

/// Toggle state of the 'Ortho allow dolly' menu entry.
void SplitView::ToggleOrthoDolly()  {
  if (m_cameraMenu->IsEntryChecked(m_GLOrthoDolly))
    m_cameraMenu->UnCheckEntry(m_GLOrthoDolly);
  else
    m_cameraMenu->CheckEntry(m_GLOrthoDolly);
  Bool_t state = ! m_cameraMenu->IsEntryChecked(m_GLOrthoDolly);
  if (m_activeViewer) {
    m_activeViewer->GetOrthoXOYCamera()->SetDollyToZoom(state);
    m_activeViewer->GetOrthoXOZCamera()->SetDollyToZoom(state);
    m_activeViewer->GetOrthoZOYCamera()->SetDollyToZoom(state);
  }
}

/// Item has been clicked, based on mouse button do:
void SplitView::ItemClicked(TGListTreeItem *item, int, int, int)   {
  static const TEveException eh("SplitView::ItemClicked ");
  TEveElement* re = (TEveElement*)item->GetUserData();
  if(re == 0) return;
  TObject* obj = re->GetObject(eh);
  if (obj->InheritsFrom("TEveViewer")) {
    TGLViewer *v = ((TEveViewer *)obj)->GetGLViewer();
    //v->Activated();
    if (v->InheritsFrom("TGLEmbeddedViewer")) {
      TGLEmbeddedViewer *ev = (TGLEmbeddedViewer *)v;
      gVirtualX->SetInputFocus(ev->GetGLWidget()->GetId());
    }
  }
}

/// Load configuration parameters from file
void SplitView::LoadConfig(const char *fname)  {
  int height, width;
  TEnv *env = new TEnv(fname);

  int mainheight = env->GetValue("MainView.Height", 434);
  int blwidth    = env->GetValue("Bottom.Left.Width", 266);
  int bcwidth    = env->GetValue("Bottom.Center.Width", 266);
  int top_height = env->GetValue("Right.Tab.Height", 0);
  int bottom_height = env->GetValue("Bottom.Tab.Height", 0);

  if (isEmbedded() && m_eve) {
    int sel = env->GetValue("Eve.Selection", m_eve->GetSelection()->GetPickToSelect());
    int hi = env->GetValue("Eve.Highlight", m_eve->GetHighlight()->GetPickToSelect());
    m_eve->GetBrowser()->EveMenu(9+sel);
    m_eve->GetBrowser()->EveMenu(13+hi);

    width  = env->GetValue("Eve.Width", (int)m_eve->GetBrowser()->GetWidth());
    height = env->GetValue("Eve.Height", (int)m_eve->GetBrowser()->GetHeight());
    m_eve->GetBrowser()->Resize(width, height);
  }

  // top (main) split frame
  width = splitFrame()->GetFirst()->GetWidth();
  splitFrame()->GetFirst()->Resize(width, mainheight);
  // bottom left split frame
  height = splitFrame()->GetSecond()->GetFirst()->GetHeight();
  splitFrame()->GetSecond()->GetFirst()->Resize(blwidth, height);
  // bottom center split frame
  height = splitFrame()->GetSecond()->GetSecond()->GetHeight();
  splitFrame()->GetSecond()->GetSecond()->GetFirst()->Resize(bcwidth, height);

  splitFrame()->Layout();

  if (isEmbedded() && m_eve) {
    width = ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabBottom()->GetParent())->GetWidth();
    ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabBottom()->GetParent())->Resize(width, bottom_height);
    width = ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabRight()->GetParent())->GetWidth();
    ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabRight()->GetParent())->Resize(width, top_height);
  }
}

/// Save configuration parameters to file
void SplitView::SaveConfig(const char *fname)   {
  int bottom_height = 0;
  int top_height = 0;
  TGSplitFrame *frm;
  TEnv *env = new TEnv(fname);

  if (isEmbedded() && m_eve) {
    env->SetValue("Eve.Width", (int)m_eve->GetBrowser()->GetWidth());
    env->SetValue("Eve.Height", (int)m_eve->GetBrowser()->GetHeight());
  }
  // get top (main) split frame
  frm = splitFrame()->GetFirst();
  env->SetValue("MainView.Height", (int)frm->GetHeight());
  // get bottom left split frame
  frm = splitFrame()->GetSecond()->GetFirst();
  env->SetValue("Bottom.Left.Width", (int)frm->GetWidth());
  // get bottom center split frame
  frm = splitFrame()->GetSecond()->GetSecond();
  env->SetValue("Bottom.Center.Width", (int)frm->GetWidth());
  if (isEmbedded() && m_eve) {
    top_height = (int)((TGCompositeFrame *)m_eve->GetBrowser()->GetTabRight()->GetParent())->GetHeight();
    env->SetValue("Right.Tab.Height", top_height);
    bottom_height = (int)((TGCompositeFrame *)m_eve->GetBrowser()->GetTabBottom()->GetParent())->GetHeight();
    env->SetValue("Bottom.Tab.Height", bottom_height);

    env->SetValue("Eve.Selection", m_eve->GetSelection()->GetPickToSelect());
    env->SetValue("Eve.Highlight", m_eve->GetHighlight()->GetPickToSelect());
  }

  env->SaveLevel(kEnvLocal);
#ifdef R__WIN32
  if (!gSystem->AccessPathName(Form("%s.new", fname))) {
    gSystem->Exec(Form("del %s", fname));
    gSystem->Rename(Form("%s.new", fname), fname);
  }
#endif
}

/// Swap frame embedded in a splitframe to the main view (slot method).
void SplitView::SwapToMainView(TGLViewerBase *viewer)  {
  TGCompositeFrame *parent = 0;
  if (!splitFrame()->GetFirst()->GetFrame())
    return;
  if (viewer == 0) {
    TGPictureButton *src = (TGPictureButton*)gTQSender;
    parent = (TGCompositeFrame *)src->GetParent();
    while (parent && !parent->InheritsFrom("TGSplitFrame")) {
      parent = (TGCompositeFrame *)parent->GetParent();
    }
  }
  else {
    TGCompositeFrame *src = ((TGLEmbeddedViewer *)viewer)->GetFrame();
    if (!src) return;
    TGLOverlayButton *but = (TGLOverlayButton *)((TQObject *)gTQSender);
    but->ResetState();
    parent = (TGCompositeFrame *)src->GetParent();
  }
  if (parent && parent->InheritsFrom("TGSplitFrame"))
    ((TGSplitFrame *)parent)->SwitchToMain();
}

/// Undock frame embedded in a splitframe (slot method).
void SplitView::Undock(TGLViewerBase *viewer)   {
  TGCompositeFrame *src = ((TGLEmbeddedViewer *)viewer)->GetFrame();
  if (!src) return;
  TGLOverlayButton *but = (TGLOverlayButton *)((TQObject *)gTQSender);
  but->ResetState();
  TGCompositeFrame *parent = (TGCompositeFrame *)src->GetParent();
  if (parent && parent->InheritsFrom("TGSplitFrame"))
    ((TGSplitFrame *)parent)->ExtractFrame();
}

/// Update displays and projections
void SplitView::update()   {
  printout(ERROR,"SplitView","+++ Calling SplitView::update....");
#if 0
  // cleanup then import geometry and event in the projection managers
  TEveElement* top = m_eve->GetCurrentEvent();
  for(Views::iterator i=m_views.begin(); i!=m_views.end(); ++i)  {
    TEveProjectionManager* p = (*i)->projection();
    if ( p ) {
      p->DestroyElements();
      p->ImportElements(m_geom);
      printout(INFO,"SplitView","+++ Imported geometry in %s",(*i)->viewer()->GetFrame()->GetName());
      if ( top ) p->ImportElements(top);
    }
  }
#endif
}
