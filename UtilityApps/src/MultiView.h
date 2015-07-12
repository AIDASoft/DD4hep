// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

#ifndef MultiView_h
#define MultiView_h

#include <TEveManager.h>
#include <TEveViewer.h>
#include <TGLViewer.h>

#include <TEveScene.h>

#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>

#include <TEveBrowser.h>
#include <TEveWindow.h>

/** Multi-view (3d, rphi, rhoz) service class using EVE Window Manager.
 * Author: Matevz Tadel 2009
 * MultiView
 *
 * Structure encapsulating standard views: 3D, r-phi and rho-z.
 * Includes scenes and projection managers.
 *
 * Should be used in compiled mode.
 * 
 * @author F.Gaede, DESY (modified original version from Matevz Tadel)
 * @version "$Id:$"
 */

class MultiView {

public:

  TEveProjectionManager *fRPhiMgr;
  TEveProjectionManager *fRhoZMgr;

  TEveViewer            *f3DView;
  TEveViewer            *fRPhiView;
  TEveViewer            *fRhoZView;

  TEveScene             *fRPhiGeomScene;
  TEveScene             *fRhoZGeomScene;
  TEveScene             *fRPhiEventScene;
  TEveScene             *fRhoZEventScene;

  //---------------------------------------------------------------------------

  static MultiView* instance() {
    static MultiView mw ;
    return &mw ;
  }

  MultiView() {

    // Constructor --- creates required scenes, projection managers
    // and GL viewers.

    // Scenes
    //========

    fRPhiGeomScene  = gEve->SpawnNewScene("RPhi Geometry",
                                          "Scene holding projected geometry for the RPhi view.");
    fRhoZGeomScene  = gEve->SpawnNewScene("RhoZ Geometry",
                                          "Scene holding projected geometry for the RhoZ view.");
    fRPhiEventScene = gEve->SpawnNewScene("RPhi Event Data",
                                          "Scene holding projected event-data for the RPhi view.");
    fRhoZEventScene = gEve->SpawnNewScene("RhoZ Event Data",
                                          "Scene holding projected event-data for the RhoZ view.");


    // Projection managers
    //=====================

    fRPhiMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    gEve->AddToListTree(fRPhiMgr, kFALSE);
    {
      TEveProjectionAxes* a = new TEveProjectionAxes(fRPhiMgr);
      a->SetMainColor(kWhite);
      a->SetTitle("R-Phi");
      a->SetTitleSize(0.05);
      a->SetTitleFont(102);
      a->SetLabelSize(0.025);
      a->SetLabelFont(102);
      fRPhiGeomScene->AddElement(a);
    }

    fRhoZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    gEve->AddToListTree(fRhoZMgr, kFALSE);
    {
      TEveProjectionAxes* a = new TEveProjectionAxes(fRhoZMgr);
      a->SetMainColor(kWhite);
      a->SetTitle("Rho-Z");
      a->SetTitleSize(0.05);
      a->SetTitleFont(102);
      a->SetLabelSize(0.025);
      a->SetLabelFont(102);
      fRhoZGeomScene->AddElement(a);
    }


    // Viewers
    //=========

    TEveWindowSlot *slot = 0;
    TEveWindowPack *pack = 0;

    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    pack = slot->MakePack();
    pack->SetElementName("Multi View");
    pack->SetHorizontal();
    pack->SetShowTitleBar(kFALSE);
    pack->NewSlot()->MakeCurrent();
    f3DView = gEve->SpawnNewViewer("3D View", "");
    f3DView->AddScene(gEve->GetGlobalScene());
    f3DView->AddScene(gEve->GetEventScene());

    pack = pack->NewSlot()->MakePack();
    pack->SetShowTitleBar(kFALSE);
    pack->NewSlot()->MakeCurrent();
    fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
    fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    fRPhiView->AddScene(fRPhiGeomScene);
    fRPhiView->AddScene(fRPhiEventScene);

    pack->NewSlot()->MakeCurrent();
    fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
    fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    fRhoZView->AddScene(fRhoZGeomScene);
    fRhoZView->AddScene(fRhoZEventScene);
 

    //fg: set white background:
    f3DView->GetGLViewer()->ColorSet().Background().SetColor(kWhite);
    fRPhiView->GetGLViewer()->ColorSet().Background().SetColor(kWhite);
    fRhoZView->GetGLViewer()->ColorSet().Background().SetColor(kWhite);
  }

  //---------------------------------------------------------------------------

  void SetDepth(Float_t d)
  {
    // Set current depth on all projection managers.

    fRPhiMgr->SetCurrentDepth(d);
    fRhoZMgr->SetCurrentDepth(d);
  }

  //---------------------------------------------------------------------------

  void ImportGeomRPhi(TEveElement* el)
  { 
    fRPhiMgr->ImportElements(el, fRPhiGeomScene);
  }

  void ImportGeomRhoZ(TEveElement* el)
  { 
    fRhoZMgr->ImportElements(el, fRhoZGeomScene);
  }

  void ImportEventRPhi(TEveElement* el)
  { 
    fRPhiMgr->ImportElements(el, fRPhiEventScene);
  }

  void ImportEventRhoZ(TEveElement* el)
  { 
    fRhoZMgr->ImportElements(el, fRhoZEventScene);
  }


  void ImportEvent(TEveElement* el)
  { 
    gEve->AddElement( el ) ;
    fRPhiMgr->ImportElements(el, fRPhiEventScene);
    fRhoZMgr->ImportElements(el, fRhoZEventScene);
  }

  //---------------------------------------------------------------------------

  void DestroyEventRPhi()
  {
    fRPhiEventScene->DestroyElements();
  }

  void DestroyEventRhoZ()
  {
    fRhoZEventScene->DestroyElements();
  }
};

//=====================================================================================



#endif


