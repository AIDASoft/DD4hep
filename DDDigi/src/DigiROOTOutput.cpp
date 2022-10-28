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

/// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiROOTOutput.h>

/// ROOT include files
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

/// C/C++ include files
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace dd4hep::digi;

class DigiROOTOutput::internals_t  {
public:
  typedef std::map<std::string, TBranch*> Branches;
  typedef std::map<std::string, TTree*> Sections;
  /// Reference to the ROOT file to open
  TFile* m_file   { nullptr };
  /// Reference to the event data tree
  TTree* m_tree   { nullptr };
  /// Known file sections
  Sections m_sections;
  /// Branches in the event tree
  Branches m_branches;
  DigiROOTOutput* output { nullptr };

public:
  internals_t(DigiROOTOutput* o) 
    : output(o) 
  {
  }
  ~internals_t()    {
    if (m_file) {
      TDirectory::TContext ctxt(m_file);
      m_tree->Write();
      m_file->Close();
      m_tree = 0;
      detail::deletePtr (m_file);
    }
  }
  /// Create/access tree by name for non collection user data
  TTree* section(const std::string& nam)   {
    auto i = m_sections.find(nam);
    if ( i == m_sections.end() ) {
      TDirectory::TContext ctxt(m_file);
      TTree* t = new TTree(nam.c_str(), ("Digitization " + nam + " information").c_str());
      m_sections.emplace(nam, t);
      return t;
    }
    return i->second;
  }
  /// Commit data at end of filling procedure
  void commit()   {
    if ( m_file ) {
      TObjArray* a = m_tree->GetListOfBranches();
      Long64_t evt = m_tree->GetEntries() + 1;
      Int_t nb = a->GetEntriesFast();
      /// Fill NULL pointers to all branches, which have less entries than the Event branch
      for (Int_t i = 0; i < nb; ++i) {
	TBranch* br_ptr = (TBranch*) a->UncheckedAt(i);
	Long64_t br_evt = br_ptr->GetEntries();
	if (br_evt < evt) {
	  Long64_t num = evt - br_evt;
	  br_ptr->SetAddress(0);
	  while (num > 0) {
	    br_ptr->Fill();
	    --num;
	  }
	}
      }
      m_tree->SetEntries(evt);
    }
  }
  /// Fill single EVENT branch entry (Geant4 collection data)
  int fill(const string& nam, const std::type_info& type, void* ptr) {
    if (m_file) {
      TBranch* b = 0;
      auto i = m_branches.find(nam);
      if (i == m_branches.end()) {
	TClass* cl = TBuffer::GetClass(type);
	if (cl) {
	  b = m_tree->Branch(nam.c_str(), cl->GetName(), (void*) 0);
	  b->SetAutoDelete(false);
	  m_branches.emplace(nam, b);
	}
	else {
	  throw runtime_error("No ROOT TClass object availible for object type:" + typeName(type));
	}
      }
      else {
	b = (*i).second;
      }
      Long64_t evt = b->GetEntries(), nevt = b->GetTree()->GetEntries(), num = nevt - evt;
      if (nevt > evt) {
	b->SetAddress(0);
	while (num > 0) {
	  b->Fill();
	  --num;
	}
      }
      b->SetAddress(&ptr);
      int nbytes = b->Fill();
      if (nbytes < 0) {
	throw runtime_error("Failed to write ROOT collection:" + nam + "!");
      }
      return nbytes;
    }
    return 0;
  }
  /// Callback to store the Geant4 event
  void saveEvent(DigiContext& /* ctxt */) {
    if ( !output->m_disableParticles )  {
#if 0
      Geant4ParticleMap* parts = context()->event().extension<Geant4ParticleMap>();
      if ( parts )   {
	typedef Geant4HitWrapper::HitManipulator Manip;
	typedef Geant4ParticleMap::ParticleMap ParticleMap;
	Manip* manipulator = Geant4HitWrapper::manipulator<Geant4Particle>();
	const ParticleMap& pm = parts->particles();
	vector<void*> particles;
	particles.reserve(pm.size());
	for ( const auto& i : pm )   {
	  particles.emplace_back((ParticleMap::mapped_type*)i.second);
	}
	fill("MCParticles",manipulator->vec_type,&particles);
      }
#endif
    }
  }
};

/// Standard constructor
DigiROOTOutput::DigiROOTOutput(const DigiKernel& kernel, const std::string& nam)
  : DigiOutputAction(kernel, nam)
{
  imp = std::make_unique<internals_t>(this);
  declareProperty("section",              m_section = "EVENT");
  declareProperty("max_file_size",        m_max_file_size);
  declareProperty("disabled_collections", m_disabledCollections);
  declareProperty("disable_particles",    m_disableParticles);
  InstanceCount::increment(this);
}

/// Default destructor
DigiROOTOutput::~DigiROOTOutput() {
  imp.reset();
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiROOTOutput::execute(DigiContext& context)  const   {
  imp->saveEvent(context);
#if 0
  for (int i = 0; i < nCol; ++i) {
    G4VHitsCollection* hc = hce->GetHC(i);
    saveCollection(ctxt, hc);
  }
#endif
}

#if 0
/// Callback to store the Geant4 run information
void DigiROOTOutput::beginRun(const G4Run* run) {
  if (!m_file && !m_output.empty()) {
    TDirectory::TContext ctxt(TDirectory::CurrentDirectory());
    m_file = TFile::Open(m_output.c_str(), "RECREATE", "DD4hep digitization data");
    if (m_file->IsZombie()) {
      detail::deletePtr (m_file);
      except("+++ Failed to open ROOT output file:'" + m_output + "'");
    }
    m_tree = section("EVENT");
  }
  Geant4OutputAction::beginRun(run);
}



/// Callback to store each Geant4 hit collection
void DigiROOTOutput::saveCollection(DigiContext& /* ctxt */, G4VHitsCollection* collection) {
  Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(collection);
  string hc_nam = collection->GetName();
  for(const auto& n : m_disabledCollections)  {
    if ( n == hc_nam )   {
      return;
    }
  }
  if (coll) {
    vector<void*> hits;
    coll->getHitsUnchecked(hits);
    size_t nhits = coll->GetSize();
    if ( m_handleMCTruth && m_truth && nhits > 0 )   {
      hits.reserve(nhits);
      try  {
        for(size_t i=0; i<nhits; ++i)   {
          Geant4HitData* h = coll->hit(i);
          Geant4Tracker::Hit* trk_hit = dynamic_cast<Geant4Tracker::Hit*>(h);
          if ( 0 != trk_hit )   {
            Geant4HitData::Contribution& t = trk_hit->truth;
            int trackID = t.trackID;
            t.trackID = m_truth->particleID(trackID);
          }
          Geant4Calorimeter::Hit* cal_hit = dynamic_cast<Geant4Calorimeter::Hit*>(h);
          if ( 0 != cal_hit )   {
            Geant4HitData::Contributions& c = cal_hit->truth;
            for(Geant4HitData::Contributions::iterator j=c.begin(); j!=c.end(); ++j)  {
              Geant4HitData::Contribution& t = *j;
              int trackID = t.trackID;
              t.trackID = m_truth->particleID(trackID);
            }
          }
        }
      }
      catch(...)   {
        printout(ERROR,name(),"+++ Exception while saving collection %s.",hc_nam.c_str());
      }
    }
    fill(hc_nam, coll->vector_type(), &hits);
  }
}
#endif
