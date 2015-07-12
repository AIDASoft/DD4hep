// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDEve/CaloLego.h"
#include "DDEve/Annotation.h"
#include "DDEve/Factories.h"
#include "DD4hep/InstanceCount.h"

// Root include files
#include "TH2.h"
#include "TEveCalo.h"
#include "TEveTrans.h"
#include "TEveScene.h"
#include "TGLViewer.h"
#include "TGLWidget.h"
#include "TEveCaloLegoOverlay.h"
#include "TEveLegoEventHandler.h"

#include <limits>
using namespace std;
using namespace DD4hep;

ClassImp(CaloLego)
DECLARE_VIEW_FACTORY(CaloLego)

/// Initializing constructor
CaloLego::CaloLego(Display* eve, const std::string& nam)
: View(eve, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
CaloLego::~CaloLego()  {
  InstanceCount::decrement(this);
}


/// Build the projection view and map it to the given slot
View& CaloLego::Build(TEveWindow* slot)   {
  CreateGeoScene();
  AddToGlobalItems(name());
  return Map(slot);
}

/// Configure a single geometry view
void CaloLego::ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)    {
  DisplayConfiguration::Configurations::const_iterator j = config.subdetectors.begin();
  Display::CalodataContext& ctx = m_data;
  float legend_y = Annotation::DefaultTextSize()+Annotation::DefaultMargin();

  ctx.eveHist = new TEveCaloDataHist();
  for( ; j != config.subdetectors.end(); ++j )   {
    const char* n = (*j).name.c_str();
    Display::CalodataContext& calo = m_eve->GetCaloHistogram(n);
    DisplayConfiguration::Calodata& cd = calo.config.data.calodata;
    if ( calo.config.use.empty() )  {
      for(int isl = 0; isl<calo.eveHist->GetNSlices(); ++isl)  {
	int nslice = ctx.eveHist->GetNSlices();
	TH2F* h = new TH2F(*calo.eveHist->GetHist(isl));
	ctx.eveHist->AddHistogram(h);
	ctx.eveHist->RefSliceInfo(nslice).Setup(n,cd.threshold,cd.color,101);
	Annotation* a = new Annotation(viewer(),n,Annotation::DefaultMargin(),legend_y,cd.color);
	legend_y += a->GetTextSize();
      }
    }
  }
  TEveCaloLego* lego = new TEveCaloLego(ctx.eveHist);
  ImportGeo(lego);
  ctx.caloViz = lego;
  ctx.caloViz->SetPlotEt(kFALSE);
  // By the default lego extends is (1x1x1). Resize it to put in 'natural' 
  // coordinates, so that y extend in 2*Pi and set height of lego two times
  //  smaller than y extend to have better view in 3D perspective.
  lego->InitMainTrans();
  lego->RefMainTrans().SetScale(TMath::TwoPi(), TMath::TwoPi(), TMath::Pi());

  // draws scales and axis on borders of window
  TGLViewer* glv = viewer()->GetGLViewer();
  TEveCaloLegoOverlay* overlay = new TEveCaloLegoOverlay();
  glv->AddOverlayElement(overlay);
  overlay->SetCaloLego(lego);
  
  // set event handler to move from perspective to orthographic view.
  glv->SetEventHandler(new TEveLegoEventHandler(glv->GetGLWidget(), glv, lego));
  glv->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  ctx.eveHist->GetEtaBins()->SetTitleFont(120);
  ctx.eveHist->GetEtaBins()->SetTitle("h");
  ctx.eveHist->GetPhiBins()->SetTitleFont(120);
  ctx.eveHist->GetPhiBins()->SetTitle("f");
  ctx.eveHist->IncDenyDestroy();
  ctx.eveHist->DataChanged();
}

/// Configure a single event scene view
void CaloLego::ConfigureEvent(const DisplayConfiguration::ViewConfig& config)  {
  DisplayConfiguration::Configurations::const_iterator j = config.subdetectors.begin();
  printout(INFO,"CaloLego","+++ Import geometry topics for view %s.",name().c_str());
  for(int ihist=0; j != config.subdetectors.end(); ++j)   {
    const char* n = (*j).name.c_str();
    const Display::CalodataContext& ctx = m_eve->GetCaloHistogram(n);
    if ( ctx.config.use.empty() )  {
      for(int isl = 0; isl<ctx.eveHist->GetNSlices(); ++isl)  {
	TH2F* global = ctx.eveHist->GetHist(isl);
	TH2F* local  = m_data.eveHist->GetHist(ihist);
	*local = *global;
	ihist++;
      }
    }
  }
  m_data.eveHist->DataChanged();
}

/// Call to import geometry topics
void CaloLego::ImportGeoTopics(const string&)   {
}
