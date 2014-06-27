// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/BasicView.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"

// ROOT include files
#include "TApplication.h"
#include "TSystem.h"
#include "TClass.h"
//#include "TGeoManager.h"
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

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TFormula.h"
#include "TF1.h"
#include "TH1F.h"

#include <stdexcept>
#include "libgen.h"

using namespace std;
using namespace DD4hep;

ClassImp(BasicView)

static const char *filetypes[] = { 
  "ROOT files",    "*.root",
  "All files",     "*",
  0,               0 
};

static const char *rcfiletypes[] = { 
  "All files",     "*",
  0,               0 
};
static TString rcdir(".");

/// Standard constructor
BasicView::BasicView(const TGWindow *parent, unsigned int width, unsigned int height, const Embedded& embed) :
  TGMainFrame(parent, width, height), 
  m_pad(), m_menuBar(0), m_statusBar(0), m_toolTip(0), m_embedded(embed)
{
  // create eve pad (our geometry container)
  m_pad.SetFillColor(kBlack);
  printout(INFO,"BasicView","+++ Running BasicView in %s mode.", 
	   isEmbedded() ? "embedded" : "standalone");
}

/// Default destructor
BasicView::~BasicView()    {
  // Clean up main frame...
  if ( !isEmbedded() )
    delete m_menuBar;
  deletePtr(m_frame);
  deletePtr(m_toolTip);
  if ( !isEmbedded() ) {
    delete m_statusBar;
    gApplication->Terminate(0);
  }
}

/// Build the view. Will call subsequently menu, frame creators etc.
void BasicView::Build()   {
  // Create menu bar
  m_menuBar = CreateMenuBar(*this);
  // Configure menu bar
  if ( m_menuBar )  {
    OnCustomMenuBar(*m_menuBar);
    AddFrame(m_menuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  }
  // Create/attach status bar
  if ( !isEmbedded() )   {
    std::pair<TGStatusBar*,TGLayoutHints*> status = CreateStatusBar(*this);
    if ( status.first )  {
      AddFrame(status.first, status.second);
    }
    m_statusBar = status.first;
  }
  else if ( isEmbedded() )  {    // use status bar from the browser
    m_statusBar = m_embedded.status;
  }
  // Configure status bar
  if ( m_statusBar ) OnCustomStatusBar(*m_statusBar);

  // Create and configure the canvas frame
  m_frame = CreateFrame(*this, GetWidth(), GetHeight());
  if ( m_frame )  {
    OnCustomFrame(*m_frame);
    AddFrame(m_frame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  }

  // Create and configure tool tips
  m_toolTip = CreateToolTip(*this);
  if ( m_toolTip ) OnCustomToolTip(*m_toolTip);
  RealizeView();
}

/// User overridable callback to create a menu bar associated to this view
TGMenuBar* BasicView::CreateMenuBar(TGCompositeFrame& /* parent */)  {
  return 0;
}

/// User overridable callback to customize the menu bar
void BasicView::OnCustomMenuBar(TGMenuBar& /* bar */)   {
}

/// User overridable callback to fill/modify the status bar
pair<TGStatusBar*,TGLayoutHints*> BasicView::CreateStatusBar(TGCompositeFrame& parent)   {
  int parts[] = {45, 15, 10, 30};
  TGStatusBar* bar = new TGStatusBar(&parent, 50, 10);
  bar->SetParts(parts, 4);
  return make_pair(bar,new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 10, 0));
}

/// User overridable callback to customize the view's frame
void BasicView::OnCustomStatusBar(TGStatusBar& /* bar */)   {
}

/// User overridable callback to create tooltip object
ToolTip* BasicView::CreateToolTip(TGCompositeFrame& /* parent */)   {
  return 0;
}

/// User overridable callback to customize the tool tip
void BasicView::OnCustomToolTip(ToolTip& /* tip */)   {
}

/// User overridable callback to set information before showing the tool tip
void BasicView::OnShowToolTip(ToolTip& /* tip */)  {
}

/// User overridable callback to create the main frame for placing objects
TGCompositeFrame* BasicView::CreateFrame(TGCompositeFrame& parent, unsigned int width, unsigned int height)  {
  TGCompositeFrame* frame = new TGSplitFrame(&parent, width, height);
  return frame;
}

/// User overridable callback to customize the view's frame
void BasicView::OnCustomFrame(TGCompositeFrame& /* frame */)   {
}

/// Create an empty menu and add register it to the view
PopupMenu* BasicView::CreateMenu()   {
  PopupMenu* menu = new PopupMenu(GetParent());
  m_menus.push_back(menu);
  return menu;
}

/// Create 'File' menu and add it to the menu bar
PopupMenu* BasicView::CreateFileMenu()  {
  // create the "file" popup menu
  PopupMenu* menu = CreateMenu();
  menu->AddEntry("&Open...",this, &BasicView::OnFileOpen);
  menu->AddSeparator();
  menu->AddEntry("&Load Config...", this, &BasicView::OnFileLoadConfig);
  menu->AddEntry("&Save Config...", this, &BasicView::OnFileSaveConfig);
  menu->AddSeparator();
  menu->AddEntry("E&xit", this, &BasicView::OnFileExit);
  return menu;
}

/// Create 'File' menu and add it to the menu bar
PopupMenu* BasicView::CreateFileMenu(TGMenuBar& menubar)   {
  TGLayoutHints* hints = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  PopupMenu* menu = CreateFileMenu();
  menubar.AddPopup("&File", *menu, hints);
  return menu;
}

/// Create 'Help' menu and add it to the menu bar
PopupMenu* BasicView::CreateHelpMenu()   {
  PopupMenu* menu = CreateMenu();
  // create the "help" popup menu
  menu->AddEntry("&About", this, &BasicView::OnHelpAbout);
  return menu;
}

/// Create 'Help' menu and add it to the menu bar
PopupMenu* BasicView::CreateHelpMenu(TGMenuBar& menubar)   {
  TGLayoutHints* hints = new TGLayoutHints(kLHintsTop | kLHintsRight);
  PopupMenu* menu = CreateHelpMenu();
  // create the main menu bar
  menubar.AddPopup("&Help", *menu, hints);
  return menu;
}

/// Map the view and load sub components
void BasicView::RealizeView()  {
  Resize(GetDefaultSize());
  MapSubwindows();
  MapWindow();
  if ( !m_config.empty() ) LoadConfig(m_config.c_str());
}

/// Handle when the view is activated
void BasicView::OnActivateView(bool /* activate */)    {
}

/// Callback when opening a new file
void BasicView::OnFileOpen(TGMenuEntry* /* entry */, void* /* ptr */)  {
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
void BasicView::OnFileLoadConfig(TGMenuEntry* /* entry */, void* /* ptr */)  {
  TGFileInfo fi;
  string cfg1 = m_config, cfg2 = m_config;
  fi.fFileTypes = rcfiletypes;
  fi.fIniDir    = StrDup(::dirname((char*)cfg1.c_str()));
  fi.fFilename  = StrDup(::basename((char*)cfg2.c_str()));
  new TGFileDialog(GetParent(), this, kFDOpen, &fi);
  if ( fi.fFilename ) {
    LoadConfig(fi.fFilename);
  }
  rcdir = fi.fIniDir;
}

/// Callback when saving the configuration
void BasicView::OnFileSaveConfig(TGMenuEntry* /* entry */, void* /* ptr */)  {
  TGFileInfo fi;
  string cfg1 = m_config, cfg2 = m_config;
  fi.fFileTypes = rcfiletypes;
  fi.fIniDir    = StrDup(::dirname((char*)cfg1.c_str()));
  fi.fFilename  = StrDup(::basename((char*)cfg2.c_str()));
  new TGFileDialog(GetParent(), this, kFDSave, &fi);
  if ( fi.fFilename )    {
    SaveConfig(fi.fFilename);
  }
  rcdir = fi.fIniDir;
}

/// Callback when saving the configuration
void BasicView::OnFileExit(TGMenuEntry* /* entry */, void* /* ptr */)  {
  CloseWindow();
}

/// Help menu: Callback when calling about
void BasicView::OnHelpAbout(TGMenuEntry* /* entry */, void* /* ptr */)    {
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

/// Handle click events in GL viewer
void BasicView::OnClicked(TObject *obj)   {
  printout(INFO,"BasicView","User clicked on: \"%s\"", obj ? obj->GetName() : "Unknown");
  if (obj)
    m_statusBar->SetText(Form("User clicked on: \"%s\"", obj->GetName()), 1);
  else
    m_statusBar->SetText("", 1);
}

/// Slot used to handle "OnMouseIdle" signal coming from any GL viewer.
void BasicView::OnMouseIdle(TGLPhysicalShape *shape, unsigned int posx, unsigned int posy)  {
  // Slot used to handle "OnMouseIdle" signal coming from any GL viewer.
  // We receive a pointer on the physical shape in which the mouse cursor is
  // and the actual cursor position (x,y)
  Window_t wtarget;
  int    x = 0, y = 0;

  m_toolTip->UnmapWindow();
  if (shape && shape->GetLogical() && shape->GetLogical()->GetExternal()) {
    // get the actual viewer who actually emitted the signal
    TGLEmbeddedViewer *actViewer = dynamic_cast<TGLEmbeddedViewer*>((TQObject*)gTQSender);
    // then translate coordinates from the root (screen) coordinates 
    // to the actual frame (viewer) ones
    gVirtualX->TranslateCoordinates(actViewer->GetFrame()->GetId(),
				    gClient->GetDefaultRoot()->GetId(), posx, posy, x, y,
				    wtarget);
    // Then display our tooltip at this x,y location
    OnShowToolTip(*m_toolTip);
    m_toolTip->Show(x+5, y+5, Form("%s\n     \n%s",
				   shape->GetLogical()->GetExternal()->IsA()->GetName(), 
				   shape->GetLogical()->GetExternal()->GetName()));
  }
}

/// Slot used to handle "OnMouseOver" signal coming from any GL viewer.
void BasicView::OnMouseOver(TGLPhysicalShape *shape)   {
  // Slot used to handle "OnMouseOver" signal coming from any GL viewer.
  // We receive a pointer on the physical shape in which the mouse cursor is.

  // display information on the physical shape in the status bar
  if (shape && shape->GetLogical() && shape->GetLogical()->GetExternal())
    m_statusBar->SetText(Form("Mouse Over: \"%s\"", 
			      shape->GetLogical()->GetExternal()->GetName()), 0);
  else
    m_statusBar->SetText("", 0);
}

/// Open a Root file to display a geometry in the GL viewers.
void BasicView::OpenFile(const char *fname)   {
  TString filename = fname;
  // check if the file type is correct
  if (!filename.EndsWith(".root")) {
    new TGMsgBox(GetParent(), this, "OpenFile",
		 Form("The file \"%s\" is not a root file!", fname),
		 kMBIconExclamation, kMBOk);
    return;
  }
}

/// Item has been clicked, based on mouse button do:
void BasicView::ItemClicked(TGListTreeItem*, int, int, int)   {
}

/// Load configuration parameters from file
void BasicView::LoadConfig(const char * /* fname */)  {
}

/// Save configuration parameters to file
void BasicView::SaveConfig(const char * /* fname */)   {
}

/// Undock frame embedded in a splitframe (slot method).
void BasicView::Undock(TGLViewerBase *viewer)   {
  TGCompositeFrame *src = ((TGLEmbeddedViewer *)viewer)->GetFrame();
  if (!src) return;
  TGLOverlayButton *but = (TGLOverlayButton *)((TQObject *)gTQSender);
  but->ResetState();
  TGCompositeFrame *parent = (TGCompositeFrame *)src->GetParent();
  if (parent && parent->InheritsFrom("TGSplitFrame"))
    ((TGSplitFrame *)parent)->ExtractFrame();
}
