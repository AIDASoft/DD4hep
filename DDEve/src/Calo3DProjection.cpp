// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
// Framework include files
#include "DDEve/Calo3DProjection.h"
#include "DDEve/Annotation.h"
#include "DDEve/Factories.h"
#include "DD4hep/InstanceCount.h"

// Root include files
#include "TEveCalo.h"
#include "TEveScene.h"

using namespace std;
using namespace DD4hep;

ClassImp(Calo3DProjection)
DECLARE_VIEW_FACTORY(Calo3DProjection)

/// Initializing constructor
Calo3DProjection::Calo3DProjection(Display* eve, const std::string& name)
: View(eve, name)
{
  InstanceCount::increment(this);
}

/// Default destructor
Calo3DProjection::~Calo3DProjection()  {
  InstanceCount::decrement(this);
}

/// Build the projection view and map it to the given slot
View& Calo3DProjection::Build(TEveWindow* slot)   {
  CreateGeoScene();
  AddToGlobalItems(name());
  return Map(slot);
}

/// Configure a single geometry view
void Calo3DProjection::ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)    {
  DisplayConfiguration::Configurations::const_iterator j = config.subdetectors.begin();
  float legend_y = Annotation::DefaultTextSize()+Annotation::DefaultMargin();
  for( ; j != config.subdetectors.end(); ++j)   {
    const char* n = (*j).name.c_str();
    const Display::CalodataContext& ctx = m_eve->GetCaloHistogram(n);
    if ( ctx.config.use.empty() ) ImportGeo(ctx.calo3D);
    printout(INFO,"Calo3DProjection","+++ %s: add detector %s  %s",
	     name().c_str(), n, ctx.config.use.c_str());
    Color_t col = ctx.calo3D->GetDataSliceColor(ctx.slice);
    Annotation* a = new Annotation(viewer(),n,Annotation::DefaultMargin(),legend_y,col);
    legend_y += a->GetTextSize();
  }
}

/// Configure a single event scene view
void Calo3DProjection::ConfigureEvent(const DisplayConfiguration::ViewConfig& /* config */)  {
}
