//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Define the ROOT dictionaries for all data classes to be saved 
//  which are created by the DDG4 examples.
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/Detector.h"

// ROOT include files
#include "TGeoManager.h"
#include "TEveGeoNode.h"
#include "TGLViewer.h"
#include "TGLUtil.h"
#include "TGLClip.h"
#include "TMap.h"
#include "TEveElement.h"
#include "TSysEvtHandler.h"
#include "TTimer.h"
#include "TVirtualPad.h"

#define private public
#include "TEveManager.h"

using namespace dd4hep::detail;

void TEve()    {
  Detector& description = Detector::getInstance();
  const char* fname = "file:../DD4hep/examples/CLICSiD/compact/compact.xml";
  description.apply("DD4hep_CompactLoader",1,(char**)&fname);

  TEveManager::Create();
  //TFile::SetCacheFileDir(".");
  //gGeoManager = gEve->GetGeometry("LHCb.gdml");
  //gGeoManager = gEve->GetGeometry("lhcbfull_v1.root");

  gEve->fGeometries->Add(new TObjString(fname),&description.manager());
  //gGeoManager->DefaultColors();

  TEveGeoTopNode* tn = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
  tn->SetVisLevel(4);
  gEve->AddGlobalElement(tn);

  gEve->FullRedraw3D(kTRUE);

  // EClipType not exported to CINT (see TGLUtil.h):
  // 0 - no clip, 1 - clip plane, 2 - clip box
  TGLViewer *v = gEve->GetDefaultGLViewer();
  v->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  v->ColorSet().Background().SetColor(kMagenta+4);
  v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
  v->RefreshPadEditor(v);
  v->CurrentCamera().RotateRad(-1.2, 0.5);
  v->DoDraw();
}
