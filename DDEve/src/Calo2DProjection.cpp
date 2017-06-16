// $Id: $
//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DDEve/Calo2DProjection.h"
#include "DDEve/Annotation.h"
#include "DDEve/Factories.h"
#include "DD4hep/InstanceCount.h"

// Root include files
#include "TEveCalo.h"
#include "TEveScene.h"
#include "TGLViewer.h"
#include "TEveArrow.h"

using namespace std;
using namespace dd4hep;

ClassImp(Calo2DProjection)
DECLARE_VIEW_FACTORY(Calo2DProjection)

/// Initializing constructor
Calo2DProjection::Calo2DProjection(Display* eve, const std::string& nam)
: Projection(eve, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
Calo2DProjection::~Calo2DProjection()  {
  InstanceCount::decrement(this);
}

/// Build the projection view and map it to the given slot
View& Calo2DProjection::Build(TEveWindow* slot)   {
  CreateGeoScene();
  CreateEventScene();
  CreateRhoPhiProjection().AddAxis();
  AddToGlobalItems(name());
  return Map(slot);
}

/// Configure a single geometry view
void Calo2DProjection::ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)    {
  DisplayConfiguration::Configurations::const_iterator fit;
  DisplayConfiguration::Configurations::const_reverse_iterator rit;
  float legend_y = Annotation::DefaultTextSize()+Annotation::DefaultMargin();
  for(fit = config.subdetectors.begin(); fit != config.subdetectors.end(); ++fit)   {
    const DisplayConfiguration::Config& cfg = *fit;
    if ( cfg.type == DisplayConfiguration::DETELEMENT )  {
      TEveElementList& sens = m_eve->GetGeoTopic("Sensitive");
      TEveElementList& struc = m_eve->GetGeoTopic("Structure");
      for(TEveElementList::List_i i=sens.BeginChildren(); i!=sens.EndChildren(); ++i)  {
	TEveElementList* ll = dynamic_cast<TEveElementList*>(*i);
	if ( ll && cfg.name == ll->GetName() )  {
	  m_projMgr->ImportElements(*i,m_geoScene);
	  goto Done;
	}
      }
      for(TEveElementList::List_i i=struc.BeginChildren(); i!=struc.EndChildren(); ++i)  {
	TEveElementList* ll = dynamic_cast<TEveElementList*>(*i);
	if ( ll && cfg.name == ll->GetName() )  {
	  m_projMgr->ImportElements(*i,m_geoScene);
	  goto Done;
	}
      }
    Done:
      continue;
    }
  }
  float depth = 0.0;
  for(rit = config.subdetectors.rbegin(); rit != config.subdetectors.rend(); ++rit)   {
    const DisplayConfiguration::Config& cfg = *rit;
    if ( cfg.type == DisplayConfiguration::CALODATA && cfg.use.empty() )  {
      const char* n = cfg.name.c_str();
      const Display::CalodataContext& ctx = m_eve->GetCaloHistogram(cfg.name);
      const DisplayConfiguration::Calo3D& calo3d = ctx.config.data.calo3d;
      TEveCalo2D* calo2d = (TEveCalo2D*)m_projMgr->ImportElements(ctx.calo3D,m_geoScene);
      calo2d->SetPlotEt(kFALSE);
      calo2d->SetDepth(depth += 1.0);
      calo2d->SetAutoRange(kTRUE);
      calo2d->SetScaleAbs(kTRUE);
      calo2d->SetMainColor(calo3d.color);
      calo2d->SetMaxValAbs(calo3d.emax);   // In GeV
      calo2d->SetMaxTowerH(calo3d.towerH); // In GeV
      calo2d->SetBarrelRadius(calo3d.rmin);
      calo2d->SetEndCapPos(calo3d.dz);
      calo2d->InvalidateCellIdCache();
      calo2d->AssertCellIdCache();
      calo2d->ComputeBBox();
      Color_t col = ctx.calo3D->GetDataSliceColor(ctx.slice);
      Annotation* a = new Annotation(viewer(),n,Annotation::DefaultMargin(),legend_y,col);
#if 0
      // Tries to add a line indicating the echelle of the energy deposit
      TEveArrow* a1 = new TEveArrow(100,100, 100,0., 0., 0.);
      a1->SetMainColor(calo3d.color);
      a1->SetTubeR(10);
      a1->SetPickable(kFALSE);
      m_projMgr->ImportElements(a1,m_geoScene);
#endif
      legend_y += a->GetTextSize();
      printout(INFO,"Calo2DProjection","+++ %s: add detector %s [%s] rmin=%f towerH:%f emax=%f",
	       name().c_str(),n,ctx.config.hits.c_str(),calo3d.rmin,calo3d.towerH,
	       calo3d.emax);
    }
  }
}

/// Configure a single event scene view
void Calo2DProjection::ConfigureEvent(const DisplayConfiguration::ViewConfig& config)  {
  this->View::ConfigureEvent(config);
}

/// Call to import geometry topics
void Calo2DProjection::ImportGeoTopics(const string& /* title */)   {
}
