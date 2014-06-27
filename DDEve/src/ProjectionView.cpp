// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/ProjectionView.h"
#include "DD4hep/Printout.h"

// ROOT include files
#include "TApplication.h"
#include "TSystem.h"
#include "TClass.h"
#include "TGeoManager.h"
#include "TEnv.h"
#include "TGTab.h"
#include "TGMenu.h"
#include "TGButton.h"
#include "TGMsgBox.h"
#include "TGStatusBar.h"
#include "TGFileDialog.h"
#include "TGSplitFrame.h"
#include "TGDockableFrame.h"

#include "TGLWidget.h"
#include "TGLEmbeddedViewer.h"
#include "TGLOverlayButton.h"
#include "TGLPhysicalShape.h"
#include "TGLLogicalShape.h"

#include "TEvePad.h"
#include "TEveManager.h"
#include "TEveViewer.h"
#include "TEveBrowser.h"
#include "TEveProjectionManager.h"
#include "TEveProjectionAxes.h"
#include "TEveScene.h"
#include "TEveGeoNode.h"
#include "TEveSelection.h"
#include "TEveEventManager.h"

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TFormula.h"
#include "TF1.h"
#include "TH1F.h"

using namespace DD4hep;

ClassImp(ProjectionView)

const char *filetypes[] = { 
  "ROOT files",    "*.root",
  "All files",     "*",
  0,               0 
};

const char *rcfiletypes[] = { 
  "All files",     "*",
  0,               0 
};
static TString rcdir(".");
static TString rcfile(".everc");

/// Standard constructor
ProjectionView::ProjectionView(TEveManager* m, TEveElement* geom, const TGWindow *p, unsigned int w, unsigned int h, Bool_t embed) :
  TGMainFrame(p, w, h), m_pad(0), m_splitFrame(0), m_viewer0(0), m_viewer1(0), m_viewer2(0),
  m_activeViewer(0), m_menuBar(0), m_fileMenu(GetParent()), m_helpmenu(GetParent()), m_cameraMenu(GetParent()), m_sceneMenu(GetParent()),
  m_statusBar(0), 
  m_toolTip("Default.png", 120, 22, 160, 110, 23, 115, 12, "#ffff80"),
  m_isEmbedded(embed), m_rphiMgr(0), m_rhozMgr(0), m_eve(m), m_geom(geom)
{
  // Main frame constructor.

  TGSplitFrame *frm;
  TEveScene *s = 0;
  TGHorizontalFrame *hfrm;
  TGDockableFrame *dfrm;
  TGPictureButton *button;

  printout(INFO,"ProjectionView","+++ Running ProjectionView in %s mode.",
	   embed ? "embedded" : "standalone");

  // create the "file" popup menu
  m_fileMenu.AddEntry("&Open...",this, &ProjectionView::OnFileOpen);
  m_fileMenu.AddSeparator();
  m_fileMenu.AddEntry("&Load Config...", this, &ProjectionView::OnFileLoadConfig);
  m_fileMenu.AddEntry("&Save Config...", this, &ProjectionView::OnFileSaveConfig);
  m_fileMenu.AddSeparator();
  m_fileMenu.AddEntry("E&xit", this, &ProjectionView::OnFileExit);

  // create the "camera" popup menu
  m_cameraMenu.AddEntry("Perspective (Floor XOZ)", this, &ProjectionView::OnCameraGLPerspXOZ);
  m_cameraMenu.AddEntry("Perspective (Floor YOZ)", this, &ProjectionView::OnCameraGLPerspYOZ);
  m_cameraMenu.AddEntry("Perspective (Floor XOY)", this, &ProjectionView::OnCameraGLPerspXOY);
  m_cameraMenu.AddEntry("Orthographic (XOY)", this, &ProjectionView::OnCameraGLXOY);
  m_cameraMenu.AddEntry("Orthographic (XOZ)", this, &ProjectionView::OnCameraGLXOZ);
  m_cameraMenu.AddEntry("Orthographic (ZOY)", this, &ProjectionView::OnCameraGLZOY);
  m_cameraMenu.AddSeparator();
  m_GLOrthoDolly = m_cameraMenu.AddEntry("Ortho allow rotate", this, &ProjectionView::OnCameraGLOrthoRotate);
  m_GLOrthoRotate = m_cameraMenu.AddEntry("Ortho allow dolly", this, &ProjectionView::OnCameraGLOrthoDolly);

  m_sceneMenu.AddEntry("&Update Current", this, &ProjectionView::OnSceneUpdate);
  m_sceneMenu.AddEntry("Update &All", this, &ProjectionView::OnSceneUpdateAll);

  // create the "help" popup menu
  m_helpmenu.AddEntry("&About", this, &ProjectionView::OnHelpAbout);

  // create the main menu bar
  m_menuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  m_menuBar->AddPopup("&File", m_fileMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  m_menuBar->AddPopup("&Camera", m_cameraMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  m_menuBar->AddPopup("&Scene", m_sceneMenu, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  m_menuBar->AddPopup("&Help", m_helpmenu, new TGLayoutHints(kLHintsTop | kLHintsRight));

  AddFrame(m_menuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

  if (m_isEmbedded && m_eve) {
    // use status bar from the browser
    m_statusBar = m_eve->GetBrowser()->GetStatusBar();
  }
  else {
    // create the status bar
    int parts[] = {45, 15, 10, 30};
    m_statusBar = new TGStatusBar(this, 50, 10);
    m_statusBar->SetParts(parts, 4);
    AddFrame(m_statusBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 
					    0, 0, 10, 0));
  }

  // create eve pad (our geometry container)
  m_pad = new TEvePad();
  m_pad->SetFillColor(kBlack);

  // create the split frames
  m_splitFrame = new TGSplitFrame(this, 800, 600);
  AddFrame(m_splitFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  // split it once
  m_splitFrame->HSplit(434);
  // then split each part again (this will make four parts)
  m_splitFrame->GetSecond()->VSplit(266);
  m_splitFrame->GetSecond()->GetSecond()->VSplit(266);

  TGLOverlayButton *but1, *but2, *but3, *but4, *but5, *but6;
  // get top (main) split frame
  frm = m_splitFrame->GetFirst();
  frm->SetName("Main_View");

  // create (embed) a GL viewer inside
  m_viewer0 = new TGLEmbeddedViewer(frm, m_pad);
  but1 = new TGLOverlayButton(m_viewer0, "Swap", 10.0, -10.0, 55.0, 16.0);
  but1->Connect("Clicked(TGLViewerBase*)", "ProjectionView", this, "SwapToMainView(TGLViewerBase*)");
  but2 = new TGLOverlayButton(m_viewer0, "Undock", 70.0, -10.0, 55.0, 16.0);
  but2->Connect("Clicked(TGLViewerBase*)", "ProjectionView", this, "Undock(TGLViewerBase*)");
  frm->AddFrame(m_viewer0->GetFrame(), new TGLayoutHints(kLHintsExpandX | 
							 kLHintsExpandY));
  // set the camera to perspective (XOZ) for this viewer
  m_viewer0->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
  // connect signal we are interested to
  m_viewer0->Connect("MouseOver(TGLPhysicalShape*)", "ProjectionView", this, 
		     "OnMouseOver(TGLPhysicalShape*)");
  m_viewer0->Connect("Activated()", "ProjectionView", this, 
		     "OnViewerActivated()");
  m_viewer0->Connect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)", 
		     "ProjectionView", this, 
		     "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  m_viewer0->Connect("Clicked(TObject*)", "ProjectionView", this, 
		     "OnClicked(TObject*)");
  m_viewer[0] = new TEveViewer("ProjectionViewer[0]");
  m_viewer[0]->SetGLViewer(m_viewer0, m_viewer0->GetFrame());
  m_viewer[0]->IncDenyDestroy();
  if (m_isEmbedded && m_eve) {
    m_viewer[0]->AddScene(m_eve->GetGlobalScene());
    //m_viewer[0]->AddScene(m_eve->GetEventScene());
    m_eve->GetViewers()->AddElement(m_viewer[0]);
    s = m_eve->SpawnNewScene("Rho-Z Projection");
    // projections
    m_rhozMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    s->AddElement(m_rhozMgr);
    m_eve->AddToListTree(m_rhozMgr, kTRUE);
    TEveProjectionAxes* a = new TEveProjectionAxes(m_rhozMgr);
    s->AddElement(a);
  }

  // get bottom left split frame
  frm = m_splitFrame->GetSecond()->GetFirst();
  frm->SetName("Bottom_Left");

  // create (embed) a GL viewer inside
  m_viewer1 = new TGLEmbeddedViewer(frm, m_pad);
  but3 = new TGLOverlayButton(m_viewer1, "Swap", 10.0, -10.0, 55.0, 16.0);
  but3->Connect("Clicked(TGLViewerBase*)", "ProjectionView", this, "SwapToMainView(TGLViewerBase*)");
  but4 = new TGLOverlayButton(m_viewer1, "Undock", 70.0, -10.0, 55.0, 16.0);
  but4->Connect("Clicked(TGLViewerBase*)", "ProjectionView", this, "Undock(TGLViewerBase*)");
  frm->AddFrame(m_viewer1->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  // set the camera to orthographic (XOY) for this viewer
  m_viewer1->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  // connect signal we are interested to
  m_viewer1->Connect("MouseOver(TGLPhysicalShape*)", "ProjectionView", this, 
		     "OnMouseOver(TGLPhysicalShape*)");
  m_viewer1->Connect("Activated()", "ProjectionView", this, 
		     "OnViewerActivated()");
  m_viewer1->Connect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)", 
		     "ProjectionView", this, 
		     "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  m_viewer1->Connect("Clicked(TObject*)", "ProjectionView", this, 
		     "OnClicked(TObject*)");
  m_viewer[1] = new TEveViewer("ProjectionViewer[1]");
  m_viewer[1]->SetGLViewer(m_viewer1, m_viewer1->GetFrame());
  m_viewer[1]->IncDenyDestroy();
  if (m_isEmbedded && m_eve) {
    m_rhozMgr->ImportElements(m_eve->GetGlobalScene());
    //m_rhozMgr->ImportElements(m_eve->GetEventScene());
    m_viewer[1]->AddScene(s);
    m_eve->GetViewers()->AddElement(m_viewer[1]);

    s = m_eve->SpawnNewScene("R-Phi Projection");
    // projections
    m_rphiMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    s->AddElement(m_rphiMgr);
    m_eve->AddToListTree(m_rphiMgr, kTRUE);
    TEveProjectionAxes* a = new TEveProjectionAxes(m_rphiMgr);
    s->AddElement(a);
  }

  // get bottom center split frame
  frm = m_splitFrame->GetSecond()->GetSecond()->GetFirst();
  frm->SetName("Bottom_Center");

  // create (embed) a GL viewer inside
  m_viewer2 = new TGLEmbeddedViewer(frm, m_pad);
  but5 = new TGLOverlayButton(m_viewer2, "Swap", 10.0, -10.0, 55.0, 16.0);
  but5->Connect("Clicked(TGLViewerBase*)", "ProjectionView", this, "SwapToMainView(TGLViewerBase*)");
  but6 = new TGLOverlayButton(m_viewer2, "Undock", 70.0, -10.0, 55.0, 16.0);
  but6->Connect("Clicked(TGLViewerBase*)", "ProjectionView", this, "Undock(TGLViewerBase*)");
  frm->AddFrame(m_viewer2->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  // set the camera to orthographic (XOY) for this viewer
  m_viewer2->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  // connect signal we are interested to
  m_viewer2->Connect("MouseOver(TGLPhysicalShape*)", "ProjectionView", this, "OnMouseOver(TGLPhysicalShape*)");
  m_viewer2->Connect("Activated()", "ProjectionView", this, "OnViewerActivated()");
  m_viewer2->Connect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)", "ProjectionView", this, 
		     "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  m_viewer2->Connect("Clicked(TObject*)", "ProjectionView", this, "OnClicked(TObject*)");
  m_viewer[2] = new TEveViewer("ProjectionViewer[2]");
  m_viewer[2]->SetGLViewer(m_viewer2, m_viewer2->GetFrame());
  m_viewer[2]->IncDenyDestroy();
  if (m_isEmbedded && m_eve) {
    m_rphiMgr->ImportElements(m_eve->GetGlobalScene());
    //m_rphiMgr->ImportElements(m_eve->GetEventScene());
    m_viewer[2]->AddScene(s);
    m_eve->GetViewers()->AddElement(m_viewer[2]);
  }

  // get bottom right split frame
  //frm = m_splitFrame->GetSecond()->GetSecond()->GetSecond();
  //frm->SetName("Bottom_Right");

  dfrm = new TGDockableFrame(frm);
  dfrm->SetFixedSize(kFALSE);
  dfrm->EnableHide(kFALSE);
  hfrm = new TGHorizontalFrame(dfrm);
  button= new TGPictureButton(hfrm, gClient->GetPicture("swap.png"));
  button->SetToolTipText("Swap to big view");
  hfrm->AddFrame(button);
  button->Connect("Clicked()","ProjectionView",this,"SwapToMainView(TGLViewerBase*=0)");
  if (m_isEmbedded && m_eve) {
    m_eve->GetListTree()->Connect("Clicked(TGListTreeItem*, int, int, int)",
				  "ProjectionView", this, "ItemClicked(TGListTreeItem*, int, int, int)");
  }
  Resize(GetDefaultSize());
  MapSubwindows();
  MapWindow();
  LoadConfig(".everc");
}

//______________________________________________________________________________
ProjectionView::~ProjectionView()
{
  // Clean up main frame...
  //Cleanup();
  m_viewer0->Disconnect("MouseOver(TGLPhysicalShape*)", this, "OnMouseOver(TGLPhysicalShape*)");
  m_viewer0->Disconnect("Activated()", this, "OnViewerActivated()");
  m_viewer0->Disconnect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)", this, "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  m_viewer1->Disconnect("MouseOver(TGLPhysicalShape*)", this, "OnMouseOver(TGLPhysicalShape*)");
  m_viewer1->Disconnect("Activated()", this, "OnViewerActivated()");
  m_viewer1->Disconnect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)",this, "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  m_viewer2->Disconnect("MouseOver(TGLPhysicalShape*)", this, "OnMouseOver(TGLPhysicalShape*)");
  m_viewer2->Disconnect("Activated()", this, "OnViewerActivated()");
  m_viewer2->Disconnect("MouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)",this, "OnMouseIdle(TGLPhysicalShape*,UInt_t,UInt_t)");
  if (!m_isEmbedded) {
    delete m_viewer[0];
    delete m_viewer[1];
    delete m_viewer[2];
  }
  if (!m_isEmbedded)
    delete m_menuBar;
  delete m_viewer0;
  delete m_viewer1;
  delete m_viewer2;
  delete m_splitFrame;
  delete m_pad;
  if (!m_isEmbedded) {
    delete m_statusBar;
    gApplication->Terminate(0);
  }
}

/// Callback when opening a new file
void ProjectionView::OnFileOpen(TGMenuEntry* /* entry */, void* /* ptr */)  {
  static TString dir(".");
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir    = StrDup(dir);
  new TGFileDialog(GetParent(), this, kFDOpen, &fi);
  if (fi.fFilename)
    OpenFile(fi.fFilename);
  dir = fi.fIniDir;
}

/// Callback when loading the configuration
void ProjectionView::OnFileLoadConfig(TGMenuEntry* /* entry */, void* /* ptr */)  {
  TGFileInfo fi;
  fi.fFileTypes = rcfiletypes;
  fi.fIniDir    = StrDup(rcdir);
  fi.fFilename  = StrDup(rcfile);
  new TGFileDialog(GetParent(), this, kFDOpen, &fi);
  if (fi.fFilename) {
    rcfile = fi.fFilename;
    LoadConfig(fi.fFilename);
  }
  rcdir = fi.fIniDir;
}

/// Callback when saving the configuration
void ProjectionView::OnFileSaveConfig(TGMenuEntry* /* entry */, void* /* ptr */)  {
  TGFileInfo fi;
  fi.fFileTypes = rcfiletypes;
  fi.fIniDir    = StrDup(rcdir);
  fi.fFilename  = StrDup(rcfile);
  new TGFileDialog(GetParent(), this, kFDSave, &fi);
  if (fi.fFilename) {
    rcfile = fi.fFilename;
    SaveConfig(fi.fFilename);
  }
  rcdir = fi.fIniDir;
}

/// Callback when saving the configuration
void ProjectionView::OnFileExit(TGMenuEntry* /* entry */, void* /* ptr */)  {
  CloseWindow();
}

/// Help menu: Callback when calling about
void ProjectionView::OnHelpAbout(TGMenuEntry* /* entry */, void* /* ptr */)    {
#ifdef R__UNIX
  TString rootx;
# ifdef ROOTBINDIR
  rootx = ROOTBINDIR;
# else
  rootx = gSystem->Getenv("ROOTSYS");
  if (!rootx.IsNull()) rootx += "/bin";
# endif
  rootx += "/root -a &";
  gSystem->Exec(rootx);
#else
#ifdef WIN32
  new TWin32SplashThread(kTRUE);
#else
  char str[32];
  sprintf(str, "About ROOT %s...", gROOT->GetVersion());
  hd = new TRootHelpDialog(this, str, 600, 400);
  hd->SetText(gHelpAbout);
  hd->Popup();
#endif
#endif
}

/// Scene menu: Callback when calling update
void ProjectionView::OnSceneUpdate(TGMenuEntry* /* entry */, void* /* ptr */)   {
  if (m_activeViewer)
    m_activeViewer->UpdateScene();
}

/// Scene menu: Callback when calling update all
void ProjectionView::OnSceneUpdateAll(TGMenuEntry* /* entry */, void* /* ptr */)   {
  m_viewer0->UpdateScene();
  m_viewer1->UpdateScene();
  m_viewer2->UpdateScene();
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLPerspXOZ(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraPerspYOZ);
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLPerspYOZ(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLPerspXOY(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLXOY(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLXOZ(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLZOY(TGMenuEntry* /* entry */, void* /* ptr */)  {
  if (m_activeViewer)
    m_activeViewer->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLOrthoRotate(TGMenuEntry* /* entry */, void* /* ptr */)  {
  ToggleOrthoRotate();
}

/// Camera menu: Changing to requested perspective
void ProjectionView::OnCameraGLOrthoDolly(TGMenuEntry* /* entry */, void* /* ptr */)  {
  ToggleOrthoDolly();
}

/// Handle click events in GL viewer
void ProjectionView::OnClicked(TObject *obj)   {
  printout(INFO,"ProjectionView","User clicked on: \"%s\"", obj ? obj->GetName() : "Unknown");
  if (obj)
    m_statusBar->SetText(Form("User clicked on: \"%s\"", obj->GetName()), 1);
  else
    m_statusBar->SetText("", 1);
}

/// Slot used to handle "OnMouseIdle" signal coming from any GL viewer.
void ProjectionView::OnMouseIdle(TGLPhysicalShape *shape, unsigned int posx, unsigned int posy)  {
  // Slot used to handle "OnMouseIdle" signal coming from any GL viewer.
  // We receive a pointer on the physical shape in which the mouse cursor is
  // and the actual cursor position (x,y)

  Window_t wtarget;
  int    x = 0, y = 0;

  static TH1F *h1f = 0;
  TFormula *form1 = new TFormula("form1","abs(sin(x)/x)");
  form1->Update(); // silent warning about unused variable...
  TF1 *sqroot = new TF1("sqroot","x*gaus(0) + [3]*form1",0,10);
  sqroot->SetParameters(10,4,1,20);
  if (h1f == 0)
    h1f = new TH1F("h1f","",50,0,10);
  h1f->Reset();
  h1f->SetFillColor(45);
  h1f->SetStats(0);
  h1f->FillRandom("sqroot",200);

  m_toolTip.UnmapWindow();

  if (shape && shape->GetLogical() && shape->GetLogical()->GetExternal()) {
    // get the actual viewer who actually emitted the signal
    TGLEmbeddedViewer *actViewer = dynamic_cast<TGLEmbeddedViewer*>((TQObject*)gTQSender);
    // then translate coordinates from the root (screen) coordinates 
    // to the actual frame (viewer) ones
    gVirtualX->TranslateCoordinates(actViewer->GetFrame()->GetId(),
				    gClient->GetDefaultRoot()->GetId(), posx, posy, x, y,
				    wtarget);
    // Then display our tooltip at this x,y location
    m_toolTip.Show(x+5, y+5, Form("%s\n     \n%s",
				   shape->GetLogical()->GetExternal()->IsA()->GetName(), 
				   shape->GetLogical()->GetExternal()->GetName()), h1f);
  }
}

/// Slot used to handle "OnMouseOver" signal coming from any GL viewer.
void ProjectionView::OnMouseOver(TGLPhysicalShape *shape)   {
  // Slot used to handle "OnMouseOver" signal coming from any GL viewer.
  // We receive a pointer on the physical shape in which the mouse cursor is.

  // display information on the physical shape in the status bar
  if (shape && shape->GetLogical() && shape->GetLogical()->GetExternal())
    m_statusBar->SetText(Form("Mouse Over: \"%s\"", 
			      shape->GetLogical()->GetExternal()->GetName()), 0);
  else
    m_statusBar->SetText("", 0);
}


/// Slot used to handle "Activated" signal coming from any GL viewer.
void ProjectionView::OnViewerActivated()   {
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
void ProjectionView::OpenFile(const char *fname)   {
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
  m_pad->GetListOfPrimitives()->Delete();
  // and add the geometry to eve pad (container)
  m_pad->GetListOfPrimitives()->Add(gGeoManager->GetTopVolume());
  // paint the geometry in each GL viewer
  m_viewer0->PadPaint(m_pad);
  m_viewer1->PadPaint(m_pad);
  m_viewer2->PadPaint(m_pad);
}

/// Toggle state of the 'Ortho allow rotate' menu entry.
void ProjectionView::ToggleOrthoRotate()  {
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
void ProjectionView::ToggleOrthoDolly()  {
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
void ProjectionView::ItemClicked(TGListTreeItem *item, int, int, int)   {
  static const TEveException eh("ProjectionView::ItemClicked ");
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
void ProjectionView::LoadConfig(const char *fname)  {
  int height, width;
  TEnv *env = new TEnv(fname);

  int mainheight = env->GetValue("MainView.Height", 434);
  int blwidth    = env->GetValue("Bottom.Left.Width", 266);
  int bcwidth    = env->GetValue("Bottom.Center.Width", 266);
  int brwidth    = env->GetValue("Bottom.Right.Width", 266);
  int top_height = env->GetValue("Right.Tab.Height", 0);
  int bottom_height = env->GetValue("Bottom.Tab.Height", 0);

  if (m_isEmbedded && m_eve) {
    int sel = env->GetValue("Eve.Selection", m_eve->GetSelection()->GetPickToSelect());
    int hi = env->GetValue("Eve.Highlight", m_eve->GetHighlight()->GetPickToSelect());
    m_eve->GetBrowser()->EveMenu(9+sel);
    m_eve->GetBrowser()->EveMenu(13+hi);

    width  = env->GetValue("Eve.Width", (int)m_eve->GetBrowser()->GetWidth());
    height = env->GetValue("Eve.Height", (int)m_eve->GetBrowser()->GetHeight());
    m_eve->GetBrowser()->Resize(width, height);
  }

  // top (main) split frame
  width = m_splitFrame->GetFirst()->GetWidth();
  m_splitFrame->GetFirst()->Resize(width, mainheight);
  // bottom left split frame
  height = m_splitFrame->GetSecond()->GetFirst()->GetHeight();
  m_splitFrame->GetSecond()->GetFirst()->Resize(blwidth, height);
  // bottom center split frame
  height = m_splitFrame->GetSecond()->GetSecond()->GetFirst()->GetHeight();
  m_splitFrame->GetSecond()->GetSecond()->GetFirst()->Resize(bcwidth, height);
  // bottom right split frame
  height = m_splitFrame->GetSecond()->GetSecond()->GetSecond()->GetHeight();
  m_splitFrame->GetSecond()->GetSecond()->GetSecond()->Resize(brwidth, height);

  m_splitFrame->Layout();

  if (m_isEmbedded && m_eve) {
    width = ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabBottom()->GetParent())->GetWidth();
    ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabBottom()->GetParent())->Resize(width, bottom_height);
    width = ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabRight()->GetParent())->GetWidth();
    ((TGCompositeFrame *)m_eve->GetBrowser()->GetTabRight()->GetParent())->Resize(width, top_height);
  }
}

/// Save configuration parameters to file
void ProjectionView::SaveConfig(const char *fname)   {
  int bottom_height = 0;
  int top_height = 0;
  TGSplitFrame *frm;
  TEnv *env = new TEnv(fname);

  if (m_isEmbedded && m_eve) {
    env->SetValue("Eve.Width", (int)m_eve->GetBrowser()->GetWidth());
    env->SetValue("Eve.Height", (int)m_eve->GetBrowser()->GetHeight());
  }
  // get top (main) split frame
  frm = m_splitFrame->GetFirst();
  env->SetValue("MainView.Height", (int)frm->GetHeight());
  // get bottom left split frame
  frm = m_splitFrame->GetSecond()->GetFirst();
  env->SetValue("Bottom.Left.Width", (int)frm->GetWidth());
  // get bottom center split frame
  frm = m_splitFrame->GetSecond()->GetSecond()->GetFirst();
  env->SetValue("Bottom.Center.Width", (int)frm->GetWidth());
  // get bottom right split frame
  frm = m_splitFrame->GetSecond()->GetSecond()->GetSecond();
  env->SetValue("Bottom.Right.Width", (int)frm->GetWidth());
  if (m_isEmbedded && m_eve) {
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

//______________________________________________________________________________
void ProjectionView::SwapToMainView(TGLViewerBase *viewer)
{
  // Swap frame embedded in a splitframe to the main view (slot method).

  TGCompositeFrame *parent = 0;
  if (!m_splitFrame->GetFirst()->GetFrame())
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
void ProjectionView::Undock(TGLViewerBase *viewer)   {
  TGCompositeFrame *src = ((TGLEmbeddedViewer *)viewer)->GetFrame();
  if (!src) return;
  TGLOverlayButton *but = (TGLOverlayButton *)((TQObject *)gTQSender);
  but->ResetState();
  TGCompositeFrame *parent = (TGCompositeFrame *)src->GetParent();
  if (parent && parent->InheritsFrom("TGSplitFrame"))
    ((TGSplitFrame *)parent)->ExtractFrame();
}

/// Update displays and projections
void ProjectionView::update()   {
  // cleanup then import geometry and event 
  // in the projection managers

  TEveElement* top = m_eve->GetCurrentEvent();
  if (m_rphiMgr) {
    m_rphiMgr->DestroyElements();
    m_rphiMgr->ImportElements(m_geom);
    printout(INFO,"ProjectionView","+++ Imported geometry in R-PHI View");
    if ( top ) m_rphiMgr->ImportElements(top);
  }
  if (m_rhozMgr) {
    m_rhozMgr->DestroyElements();
    m_rhozMgr->ImportElements(m_geom);
    printout(INFO,"ProjectionView","+++ Imported geometry in RHO-Z View");
    if ( top ) m_rhozMgr->ImportElements(top);
  }
}

/// Add axis to the projections
void ProjectionView::defineAxis(TEveProjectionManager* projection,const std::string& title,int num_divisions)  {
  TEveProjectionAxes* a = new TEveProjectionAxes(projection);
  a->SetNdivisions(num_divisions);
  m_eve->GetScenes()->FindChild(title.c_str())->AddElement(a);
}
