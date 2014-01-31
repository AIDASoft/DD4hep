// $Id: display.cpp 590 2013-06-03 17:02:43Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Generic ROOT based geometry display program
// 
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/Factories.h"
#include "DD4hep/LCDD.h"
#include "run_plugin.h"
#include "TRint.h"
#include "TEveGeoNode.h"
//#include "TEveElement.h"
#include "TGLViewer.h"
#include "TGeoManager.h"
//#include "TGLUtil.h"
#include "TGLClip.h"
//#include "TSysEvtHandler.h"
#include "TMap.h"
#include "TObjString.h"

#define private public
#include "TEveManager.h"

static long teve_display(LCDD& lcdd, int /* argc */, char** /* argv */) {
  TGeoManager* mgr = &lcdd.manager();
  TEveManager::Create();
  gEve->fGeometries->Add(new TObjString("DefaultGeometry"),mgr);
  TEveGeoTopNode* tn = new TEveGeoTopNode(mgr, mgr->GetTopNode());
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
  return 1;
}
DECLARE_APPLY(DD4hepTEveDisplay,teve_display)

//______________________________________________________________________________
int main(int argc,char** argv)  {
  return main_default("DD4hepTEveDisplay",argc,argv);
}
