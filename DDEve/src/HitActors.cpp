// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/HitActors.h"
#include "DD4hep/Objects.h"

// ROOT include files
#include "TH2.h"
#include "TVector3.h"
#include "TEveBoxSet.h"
#include "TEvePointSet.h"
#include "TEveCompound.h"

using namespace std;
using namespace DD4hep;


/// Action callback of this functor: 
void EtaPhiHistogramActor::operator()(const DDEveHit& hit)   {
  const Geometry::Position pos(hit.x/MM_2_CM,hit.y/MM_2_CM,hit.z/MM_2_CM);
  histogram->Fill(pos.Eta(),pos.Phi(),hit.deposit);
}

/// Standard initializing constructor
PointsetCreator::PointsetCreator(const std::string& collection, size_t length) 
  : pointset(0), deposit(0), count(0) 
{
  pointset = new TEvePointSet(collection.c_str(),length);
  pointset->SetMarkerSize(0.2);
}

/// Standard initializing constructor
PointsetCreator::PointsetCreator(const std::string& collection, size_t length, const DisplayConfiguration::Config& cfg) 
  : pointset(0), deposit(0), count(0) 
{
  pointset = new TEvePointSet(collection.c_str(),length);
  pointset->SetMarkerSize(cfg.data.hits.size);
  pointset->SetMarkerStyle(cfg.data.hits.type);
  //pointset->SetMarkerAlpha(cfg.data.hits.alpha);
  pointset->SetMainColor(cfg.data.hits.color);
}
/// Return eve element
TEveElement* PointsetCreator::element() const   {
  return pointset;
}

/// Standard destructor
PointsetCreator::~PointsetCreator()   {
  if ( pointset )  {
    pointset->SetTitle(Form("Hit collection:\n"
			  "Container%s\n"
			  "with %d hits\n"
			  "total deposit:%.3f GeV",
			  pointset->GetName(), count, deposit));
  }
}

/// Action callback of this functor: 
void PointsetCreator::operator()(const DDEveHit& hit)   {
  pointset->SetPoint(count++, hit.x*MM_2_CM, hit.y*MM_2_CM, hit.z*MM_2_CM); 
}

/// Standard initializing constructor
BoxsetCreator::BoxsetCreator(const std::string& collection, size_t /*length */, const DisplayConfiguration::Config& cfg)
  : boxset(0), emax(1e12), towerH(1e12), deposit(0.0), count(0)
{
  emax   = cfg.data.hits.emax;
  towerH = cfg.data.hits.towerH;
  boxset = new TEveBoxSet(collection.c_str());
  boxset->Reset(TEveBoxSet::kBT_FreeBox, kFALSE, 64);
  boxset->SetMainTransparency(0);
  boxset->SetMainColor(cfg.data.hits.color);
  boxset->SetRenderMode(TEveBoxSet::kRM_Fill);
  boxset->CSCApplyMainColorToAllChildren();
  boxset->CSCApplyMainTransparencyToAllChildren();
}

/// Standard initializing constructor
BoxsetCreator::BoxsetCreator(const std::string& collection, size_t /*length */)
  : boxset(0), emax(1e12), towerH(1e12), deposit(0.0), count(0)
{
  boxset = new TEveBoxSet(collection.c_str());
  boxset->SetMainTransparency(0);
  boxset->Reset(TEveBoxSet::kBT_FreeBox, kFALSE, 64);
  boxset->CSCApplyMainColorToAllChildren();
  boxset->CSCApplyMainTransparencyToAllChildren();
}

/// Standard destructor
BoxsetCreator::~BoxsetCreator()  {
  if ( boxset )  {
    boxset->SetTitle(Form("Hit collection:\n"
			  "Container%s\n"
			  "with %d hits\n"
			  "total deposit:%.3f GeV",
			  boxset->GetName(), count, deposit));
  }
}

/// Return eve element
TEveElement* BoxsetCreator::element() const   {
  return boxset;
}

/// Action callback of this functor: 
void BoxsetCreator::operator()(const DDEveHit& hit)   {
  double ene = hit.deposit*MEV_2_GEV <= emax ? hit.deposit*MEV_2_GEV : emax;
  TVector3 scale(ene/towerH,ene/towerH,ene/towerH);
  cout << "Hit:" << ene << " deposit:" << hit.deposit << " " << " emax:" << emax << " towerH:" << towerH << endl;
  TVector3 p(hit.x*MM_2_CM, hit.y*MM_2_CM, hit.z*MM_2_CM);
  double phi = p.Phi();
  float s1X = -0.5*(scale(0)*std::sin(phi)+scale(2)*std::cos(phi));
  float s1Y =  0.5*(scale(0)*std::cos(phi)-scale(2)*std::sin(phi));
  float s2X = -0.5*(scale(0)*std::sin(phi)-scale(2)*std::cos(phi));
  float s2Y =  0.5*(scale(0)*std::cos(phi)+scale(2)*std::sin(phi));
  float s1Z =  scale(1)/2.0;
  float s2Z = s1Z;
  float coords[24]= { p.X()+s1X, p.Y()+s1Y, p.Z()-s1Z,
		      p.X()+s1X, p.Y()+s1Y, p.Z()+s1Z,
		      p.X()-s2X, p.Y()-s2Y, p.Z()+s2Z,
		      p.X()-s2X, p.Y()-s2Y, p.Z()-s2Z,
		      p.X()+s2X, p.Y()+s2Y, p.Z()-s2Z,
		      p.X()+s2X, p.Y()+s2Y, p.Z()+s2Z,
		      p.X()-s1X, p.Y()-s1Y, p.Z()+s1Z,
		      p.X()-s1X, p.Y()-s1Y, p.Z()-s1Z};
  ++count;
  deposit += hit.deposit*MEV_2_GEV;
  boxset->AddBox(coords);
  boxset->DigitColor(boxset->GetMainColor());
}

/// Action callback of this functor: 
void TowersetCreator::operator()(const DDEveHit& hit)   {
  double ene = hit.deposit*MEV_2_GEV <= emax ? hit.deposit*MEV_2_GEV : emax;
  TVector3 scale(1,1,ene/towerH);
  TVector3 p(hit.x*MM_2_CM, hit.y*MM_2_CM, hit.z*MM_2_CM);
  double phi = p.Phi();
  float s1X = -0.5*(scale(0)*std::sin(phi)+scale(2)*std::cos(phi));
  float s1Y =  0.5*(scale(0)*std::cos(phi)-scale(2)*std::sin(phi));
  float s2X = -0.5*(scale(0)*std::sin(phi)-scale(2)*std::cos(phi));
  float s2Y =  0.5*(scale(0)*std::cos(phi)+scale(2)*std::sin(phi));
  float s1Z =  scale(1)/2.0;
  float s2Z = s1Z;
  p = TVector3(hit.x*MM_2_CM-s1X, hit.y*MM_2_CM-s1Y, hit.z*MM_2_CM-s1Z);
  float coords[24]= { p.X()+s1X, p.Y()+s1Y, p.Z()-s1Z,
		      p.X()+s1X, p.Y()+s1Y, p.Z()+s1Z,
		      p.X()-s2X, p.Y()-s2Y, p.Z()+s2Z,
		      p.X()-s2X, p.Y()-s2Y, p.Z()-s2Z,
		      p.X()+s2X, p.Y()+s2Y, p.Z()-s2Z,
		      p.X()+s2X, p.Y()+s2Y, p.Z()+s2Z,
		      p.X()-s1X, p.Y()-s1Y, p.Z()+s1Z,
		      p.X()-s1X, p.Y()-s1Y, p.Z()-s1Z};
  ++count;
  deposit += hit.deposit*MEV_2_GEV;
  boxset->AddBox(coords);
  boxset->DigitColor(boxset->GetMainColor());
}
