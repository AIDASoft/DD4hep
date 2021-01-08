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
#include "DDEve/HitActors.h"
#include "DD4hep/Objects.h"
#include "DD4hep/DD4hepUnits.h"

// ROOT include files
#include "TH2.h"
#include "TVector3.h"
#include "TEveBoxSet.h"
#include "TEvePointSet.h"
#include "TEveCompound.h"

using namespace std;
using namespace dd4hep;

#ifdef DD4HEP_USE_GEANT4_UNITS
#define MM_2_CM        1.0
#define MEV_TO_GEV  1000.0
#else
#define MM_2_CM        0.1
#define MEV_TO_GEV     1.0
#endif

/// Action callback of this functor: 
void EtaPhiHistogramActor::operator()(const DDEveHit& hit)   {
  const Position pos(hit.x/MM_2_CM,hit.y/MM_2_CM,hit.z/MM_2_CM);
  histogram->Fill(pos.Eta(),pos.Phi(),hit.deposit);
}

/// Standard initializing constructor
PointsetCreator::PointsetCreator(const std::string& collection, size_t length) 
{
  pointset = new TEvePointSet(collection.c_str(),length);
  pointset->SetMarkerSize(0.2);
}

/// Standard initializing constructor
PointsetCreator::PointsetCreator(const std::string& collection, size_t length, const DisplayConfiguration::Config& cfg) 
{
  pointset = new TEvePointSet(collection.c_str(),length);
  pointset->SetMarkerSize(cfg.data.hits.size);
  pointset->SetMarkerStyle(cfg.data.hits.type);
  //pointset->SetMarkerAlpha(cfg.data.hits.alpha);
  pointset->SetMainColor(cfg.data.hits.color);
  threshold = cfg.data.hits.threshold * MEV_TO_GEV;
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
  if ( hit.deposit > threshold )   {
    deposit += hit.deposit;
    pointset->SetPoint(count++, hit.x*MM_2_CM, hit.y*MM_2_CM, hit.z*MM_2_CM);
  }
}

/// Standard initializing constructor
BoxsetCreator::BoxsetCreator(const std::string& collection, size_t /*length */, const DisplayConfiguration::Config& cfg)
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
  float coords[24]= { float(p.X()+s1X), float(p.Y()+s1Y), float(p.Z()-s1Z),
                      float(p.X()+s1X), float(p.Y()+s1Y), float(p.Z()+s1Z),
                      float(p.X()-s2X), float(p.Y()-s2Y), float(p.Z()+s2Z),
                      float(p.X()-s2X), float(p.Y()-s2Y), float(p.Z()-s2Z),
                      float(p.X()+s2X), float(p.Y()+s2Y), float(p.Z()-s2Z),
                      float(p.X()+s2X), float(p.Y()+s2Y), float(p.Z()+s2Z),
                      float(p.X()-s1X), float(p.Y()-s1Y), float(p.Z()+s1Z),
                      float(p.X()-s1X), float(p.Y()-s1Y), float(p.Z()-s1Z) };
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
  float coords[24]= { float(p.X()+s1X), float(p.Y()+s1Y), float(p.Z()-s1Z),
                      float(p.X()+s1X), float(p.Y()+s1Y), float(p.Z()+s1Z),
                      float(p.X()-s2X), float(p.Y()-s2Y), float(p.Z()+s2Z),
                      float(p.X()-s2X), float(p.Y()-s2Y), float(p.Z()-s2Z),
                      float(p.X()+s2X), float(p.Y()+s2Y), float(p.Z()-s2Z),
                      float(p.X()+s2X), float(p.Y()+s2Y), float(p.Z()+s2Z),
                      float(p.X()-s1X), float(p.Y()-s1Y), float(p.Z()+s1Z),
                      float(p.X()-s1X), float(p.Y()-s1Y), float(p.Z()-s1Z) };
  ++count;
  deposit += hit.deposit*MEV_2_GEV;
  boxset->AddBox(coords);
  boxset->DigitColor(boxset->GetMainColor());
}
