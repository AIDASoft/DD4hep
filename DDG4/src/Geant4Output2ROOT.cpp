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
#include <DD4hep/Printout.h>
#include <DD4hep/InstanceCount.h>
#include <DDG4/Geant4HitCollection.h>
#include <DDG4/Geant4Output2ROOT.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4Data.h>

// Geant4 include files
#include <G4HCofThisEvent.hh>
#include <G4ParticleTable.hh>
#include <G4Run.hh>

// ROOT include files
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TSystem.h>

using namespace dd4hep::sim;

/// Standard constructor
Geant4Output2ROOT::Geant4Output2ROOT(Geant4Context* ctxt, const std::string& nam)
  : Geant4OutputAction(ctxt, nam) {
  declareProperty("Section",              m_section = "EVENT");
  declareProperty("HandleMCTruth",        m_handleMCTruth);
  declareProperty("DisabledCollections",  m_disabledCollections);
  declareProperty("DisableParticles",     m_disableParticles);
  declareProperty("FilesByRun",           m_filesByRun);
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2ROOT::~Geant4Output2ROOT() {
  closeOutput();
  InstanceCount::decrement(this);
}

/// Close current output file
void Geant4Output2ROOT::closeOutput()   {
  if (!m_file) return;
  TDirectory::TContext ctxt(m_file.get());
  info("+++ Closing ROOT output file %s", m_file->GetName());
  m_sections.clear();
  m_branches.clear();
  m_tree->Write();
  m_file->Close();
  m_tree = nullptr;
  m_file.reset();
}

/// Create/access tree by name
TTree* Geant4Output2ROOT::section(const std::string& nam) {
  auto i = m_sections.find(nam);
  if (i == m_sections.end()) {
    TDirectory::TContext ctxt(m_file.get());
    TTree* t = new TTree(nam.c_str(), ("Geant4 " + nam + " information").c_str());
    m_sections.emplace(nam, t);
    return t;
  }
  return i->second;
}

/// Callback to store the Geant4 run information
void Geant4Output2ROOT::beginRun(const G4Run* run) {
  std::string fname = m_output;
  if ( m_filesByRun )    {
    size_t idx = m_output.rfind(".");
    if ( m_file )
      closeOutput();
    fname  = m_output.substr(0, idx);
    fname += _toString(run->GetRunID(), ".run%08d");
    if ( idx != std::string::npos )
      fname += m_output.substr(idx);
  }
  if ( !m_file && !fname.empty() ) {
    TDirectory::TContext ctxt(TDirectory::CurrentDirectory());
    if ( !gSystem->AccessPathName(fname.c_str()) )  {
      gSystem->Unlink(fname.c_str());
    }
    std::unique_ptr<TFile> file(TFile::Open(fname.c_str(), "RECREATE", "dd4hep Simulation data"));
    if ( !file )  {
      file.reset(TFile::Open((fname+".1").c_str(), "RECREATE", "dd4hep Simulation data"));
    }
    if ( !file )  {
      except("Failed to create ROOT output file:'%s'", fname.c_str());
    }
    if (file->IsZombie()) {
      except("Failed to open ROOT output file:'%s'", fname.c_str());
    }
    m_file = std::move(file);
    m_tree = section(m_section);
  }
  Geant4OutputAction::beginRun(run);
}

/// Fill single EVENT branch entry (Geant4 collection data)
int Geant4Output2ROOT::fill(const std::string& nam, const ComponentCast& type, void* ptr) {
  if (!m_file) return 0;
  TBranch* b = nullptr;
  auto i = m_branches.find(nam);
  if (i == m_branches.end()) {
    const std::type_info& typ = type.type();
    if (auto* cl = TBuffer::GetClass(typ)) {
      b = m_tree->Branch(nam.c_str(), cl->GetName(), static_cast<void*>(nullptr));
      b->SetAutoDelete(false);
      m_branches.emplace(nam, b);
    }
    else {
      throw std::runtime_error("No ROOT TClass object available for object type:" + typeName(typ));
    }
  }
  else {
    b = i->second;
  }
  const Long64_t evt  = b->GetEntries();
  const Long64_t nevt = b->GetTree()->GetEntries();
  if (nevt > evt) {
    b->SetAddress(nullptr);
    for (Long64_t num = nevt - evt; num > 0; --num)
      b->Fill();
  }
  b->SetAddress(&ptr);
  const int nbytes = b->Fill();
  if (nbytes < 0) {
    throw std::runtime_error("Failed to write ROOT collection:" + nam + "!");
  }
  return nbytes;
}

/// Commit data at end of filling procedure
void Geant4Output2ROOT::commit(OutputContext<G4Event>& ctxt) {
  if (m_file) {
    auto* a = m_tree->GetListOfBranches();
    const Long64_t evt = m_tree->GetEntries() + 1;
    const Int_t nb = a->GetEntriesFast();
    /// Fill NULL pointers to all branches, which have less entries than the Event branch
    for (Int_t i = 0; i < nb; ++i) {
      auto* br_ptr = static_cast<TBranch*>(a->UncheckedAt(i));
      const Long64_t br_evt = br_ptr->GetEntries();
      if (br_evt < evt) {
        br_ptr->SetAddress(nullptr);
        for (Long64_t num = evt - br_evt; num > 0; --num)
          br_ptr->Fill();
      }
    }
    m_tree->SetEntries(evt);
  }
  Geant4OutputAction::commit(ctxt);
}

/// Callback to store the Geant4 event
void Geant4Output2ROOT::saveEvent(OutputContext<G4Event>& /* ctxt */) {
  if (m_disableParticles) return;
  auto* parts = context()->event().extension<Geant4ParticleMap>();
  if (!parts) return;
  Geant4HitWrapper::HitManipulator* manipulator = Geant4HitWrapper::manipulator<Geant4Particle>();
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  const Geant4ParticleMap::ParticleMap pm = parts->particles();
  std::vector<void*> particles;
  particles.reserve(pm.size());
  for ( const auto& i : pm )   {
    auto* p = i.second;
    G4ParticleDefinition* def = table->FindParticle(p->pdgID);
    p->charge = static_cast<char>(3.0 * (def ? def->GetPDGCharge() : -1.0)); // Assume e-/pi-
    particles.emplace_back(p);
  }
  fill("MCParticles",manipulator->vec_type,&particles);
}

/// Callback to store each Geant4 hit collection
void Geant4Output2ROOT::saveCollection(OutputContext<G4Event>& /* ctxt */, G4VHitsCollection* collection) {
  auto* coll = dynamic_cast<Geant4HitCollection*>(collection);
  if (!coll) return;
  const std::string hc_nam = collection->GetName();
  for(const auto& n : m_disabledCollections)  {
    if ( n == hc_nam )   {
      return;
    }
  }
  const size_t nhits = coll->GetSize();
  std::vector<void*> hits;
  hits.reserve(nhits);
  coll->getHitsUnchecked(hits);
  if ( m_handleMCTruth && m_truth && nhits > 0 )   {
    try  {
      for(size_t i=0; i<nhits; ++i)   {
        Geant4HitData* h = coll->hit(i);
        if (auto* trk_hit = dynamic_cast<Geant4Tracker::Hit*>(h))   {
          Geant4HitData::Contribution& t = trk_hit->truth;
          t.trackID = m_truth->particleID(t.trackID);
        }
        if (auto* cal_hit = dynamic_cast<Geant4Calorimeter::Hit*>(h))   {
          for(auto& t : cal_hit->truth)
            t.trackID = m_truth->particleID(t.trackID);
        }
      }
    }
    catch(...)   {
      error("+++ Exception while saving collection %s.",hc_nam.c_str());
    }
  }
  fill(hc_nam, coll->vector_type(), &hits);
}
