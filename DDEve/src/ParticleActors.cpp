// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDEve/ParticleActors.h"
#include "DD4hep/Objects.h"

#include "TEveCompound.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"

#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TGeoManager.h"

using namespace std;
using namespace DD4hep;

static Color_t Colors[] = {
  kRed, kBlue, kYellow, kGreen, kPink, kAzure, kOrange, kTeal, kViolet, kSpring, kMagenta, kCyan
};

/// Standard initializing constructor
MCParticleCreator::MCParticleCreator(TEveTrackPropagator* p, TEveCompound* ps, const DisplayConfiguration::Config* cfg) 
  : propagator(p), particles(ps), count(0), lineWidth(4)
{
  propagator->SetName("Track propagator for charged particles");
  propagator->SetMaxR(1000);
  propagator->SetMaxZ(1000);
  propagator->SetMaxOrbs(10.0);
  propagator->SetDelta(0.01); // Step
  propagator->RefPMAtt().SetMarkerColor(kYellow);
  propagator->RefPMAtt().SetMarkerStyle(kCircle);
  propagator->RefPMAtt().SetMarkerSize(1.0);
  if ( cfg )  {
    lineWidth = cfg->data.hits.width;
    propagator->RefPMAtt().SetMarkerSize(cfg->data.hits.size);
    propagator->RefPMAtt().SetMarkerStyle(cfg->data.hits.type);
  }
}

/// Access sub-compound by name
void MCParticleCreator::addCompound(const std::string& name, TEveLine* e)   {
  Compounds::const_iterator i = types.find(name);
  if ( i == types.end() )  {
    static int icol = 0;
    TEveCompound* o = new TEveCompound(name.c_str(),name.c_str());
    particles->AddElement(o);
    i = types.insert(make_pair(name,o)).first;
    Color_t col = Colors[icol%(sizeof(Colors)/sizeof(Colors[0]))];
    col += icol/sizeof(Colors)/sizeof(Colors[0]);
    o->SetMainColor(col);
    o->CSCApplyMainColorToAllChildren();
    ++icol;
  }
  TEveCompound* c = (*i).second;
  e->SetMainColor(c->GetMainColor());
  c->AddElement(e);
}

/// Access sub-compound by name
void MCParticleCreator::addCompoundLight(const std::string& name, TEveLine* e)   {
  Compounds::const_iterator i = types.find(name);
  if ( i == types.end() )  {
    static int icol = 0;
    TEveCompound* o = new TEveCompound(name.c_str(),name.c_str());
    particles->AddElement(o);
    i = types.insert(make_pair(name,o)).first;
    o->SetMainColor(kBlack);
    o->CSCApplyMainColorToAllChildren();
    ++icol;
  }
  TEveCompound* c = (*i).second;
  e->SetLineWidth(1);
  e->SetLineStyle(2);
  e->SetMainColor(c->GetMainColor());
  c->AddElement(e);
}

/// Close compounds
void MCParticleCreator::close()   {
  for(Compounds::const_iterator i = types.begin(); i!=types.end(); ++i)  {
    (*i).second->CSCApplyMainColorToAllChildren();
    (*i).second->CloseCompound();
  }
  particles->CloseCompound();
  particles->SetRnrSelfChildren(kTRUE,kTRUE);
  propagator->SetRnrDecay(kTRUE);
  propagator->SetRnrDaughters(kTRUE);
  propagator->SetRnrCluster2Ds(kTRUE);
  propagator->SetRnrReferences(kTRUE);
}

void MCParticleCreator::operator()(const DDEveParticle& p)  {
  TEveVector momentum(p.psx*MEV_2_GEV, p.psy*MEV_2_GEV, p.psz*MEV_2_GEV);
  TEveVector start(p.vsx*MM_2_CM, p.vsy*MM_2_CM, p.vsz*MM_2_CM);
  TEveVector end(p.vex*MM_2_CM, p.vey*MM_2_CM, p.vez*MM_2_CM);
  TEveVector dir = end-start;

  // Tracks longer than 100 micron and energy > 100 MeV
  if ( dir.R()*CM_2_MM > 100e-3 && p.energy > 10e0 )  {
    TDatabasePDG* db = TDatabasePDG::Instance();
    TParticlePDG* def = db->GetParticle(p.pdgID);
    TParticle part(p.pdgID,
                   0,0,0,0,0,
                   p.psx*MEV_2_GEV, p.psy*MEV_2_GEV, p.psz*MEV_2_GEV, p.energy*MEV_2_GEV,
                   p.vsx*MM_2_CM, p.vsy*MM_2_CM, p.vsz*MM_2_CM, p.time);
  
    TEveTrack* t = new TEveTrack(&part,p.id,propagator);

    // Add start-vertex as path mark
    //t->AddPathMark(TEvePathMark(TEvePathMark::kDecay,start));
    // Add end-vertex as path mark (kDecay)
    t->AddPathMark(TEvePathMark(TEvePathMark::kLineSegment,start,momentum,dir));
    t->AddPathMark(TEvePathMark(TEvePathMark::kReference,start,momentum));
    t->AddPathMark(TEvePathMark(TEvePathMark::kDecay,end,momentum));
    t->SetLineWidth(lineWidth);
    t->SetTitle(Form("MCParticle: Track ID=%d Parent:%d\n"
                     "Type:%s Charge=%.3f Time:%.3f ns\n"
                     "Start(Vx, Vy, Vz, t) = (%.3f, %.3f, %.3f) [cm]\n"
                     "End  (Vx, Vy, Vz, t) = (%.3f, %.3f, %.3f) [cm]\n"
                     "Length:%.3f [cm]\n"
                     "(Px, Py, Pz, E) = (%.3f, %.3f, %.3f, %.3f) [GeV]",
                     p.id, p.parent,
                     def ? def->GetName() : "Unknown",
                     def ? def->Charge() : 0.0, p.time,
                     p.vsx*MM_2_CM, p.vsy*MM_2_CM, p.vsz*MM_2_CM,
                     p.vex*MM_2_CM, p.vey*MM_2_CM, p.vez*MM_2_CM,
                     dir.R(),
                     p.psx*MEV_2_GEV, p.psy*MEV_2_GEV, p.psz*MEV_2_GEV, p.energy*MEV_2_GEV));

    // Add element to collection
    int pdg = abs(p.pdgID);
    if ( pdg == 11 )
      addCompound("e+-", t);
    else if ( pdg == 12 || pdg == 14 || pdg == 16 )
      addCompoundLight("Neutrinos", t);
    else if ( pdg == 22 )
      addCompound("Gamma", t);
    else if ( pdg == 13 )
      addCompound("Muon+-", t);
    else if ( pdg == 211 )
      addCompound("Pi+-", t);
    else if ( pdg == 321 )
      addCompound("K+-", t);
    else if ( pdg == 2112 )
      addCompound("Neutrons", t);
    else if ( pdg == 2212 )
      addCompound("Protons", t);
    else
      addCompound("Other", t);
    //cout << "Add particle " << p.id << " to compound." << endl;
  }
  else  {
    cout << "SKIP particle " << p.id << "." << endl;
  }
}
