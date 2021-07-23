#include "Geant4Output2Podio.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4Particle.h"
#include "DD4hep/DD4hepUnits.h"

#include "TBuffer.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4ParticleDefinition.hh"

#include "dd4pod/ThreeVector.h"
#include "dd4pod/TrackerHitCollection.h"
#include "dd4pod/CalorimeterHitCollection.h"
#include "dd4pod/Geant4ParticleCollection.h"

#include "dd4pod/PhotoMultiplierHitCollection.h"
#include "PMTHit.h"

// Geant4 include files
#include "G4HCofThisEvent.hh"
using std::string;

namespace dd4hep::sim {

  /// Standard constructor
  Geant4Output2Podio::Geant4Output2Podio(Geant4Context* ctxt, const string& nam)
      : Geant4OutputAction(ctxt, nam) {
    declareProperty("HandleMCTruth", m_handleMCTruth = true);
    declareProperty("DisabledCollections", m_disabledCollections);
    declareProperty("EnabledCollections", m_enabledCollections);
    declareProperty("DisableParticles", m_disableParticles);
    InstanceCount::increment(this);
  }

  /// Default destructor
  Geant4Output2Podio::~Geant4Output2Podio() {
    std::cout << "deleting ouput2podio\n";
    if (writer) {
      writer->finish();
      delete writer;
    }
    writer = nullptr;
    InstanceCount::decrement(this);
    // if (m_file) {
    //  TDirectory::TContext ctxt(m_file);
    //  m_tree->Write();
    //  m_file->Close();
    //  m_tree = 0;
    //  detail::deletePtr (m_file);
    //}
  }

  /// Callback to store the Geant4 run information
  void Geant4Output2Podio::beginRun(const G4Run* run) {
    std::cout << " Begin of run action \n";
    if (!writer)
      writer = new podio::ROOTWriter(m_output, &store);
    Geant4OutputAction::beginRun(run);
  }
  /// Callback to store the Geant4 run information
  void Geant4Output2Podio::endRun(const G4Run*  run) {
    std::cout << " end of run\n";
    store.clearCollections();
    //if (writer)
    //  writer->finish();
    Geant4OutputAction::endRun(run);
  }

  ///// Fill single EVENT branch entry (Geant4 collection data)
  // int Geant4Output2Podio::fill(const string& nam, const ComponentCast& type,
  // void* ptr) {
  //  if (m_file) {
  //    TBranch* b = 0;
  //    Branches::const_iterator i = m_branches.find(nam);
  //    if (i == m_branches.end()) {
  //      // create new branch if type exists
  //      TClass* cl = TBuffer::GetClass(type.type);
  //      if (cl) {
  //        b = m_tree->Branch(nam.c_str(), cl->GetName(), (void*) 0);
  //        b->SetAutoDelete(false);
  //        m_branches.emplace(nam, b);
  //      }
  //      else {
  //        throw runtime_error("No ROOT TClass object availible for object
  //        type:"
  //        + typeName(type.type));
  //      }
  //    }
  //    else {
  //      b = (*i).second;
  //    }
  //    Long64_t evt = b->GetEntries(), nevt = b->GetTree()->GetEntries(), num =
  //    nevt - evt; if (nevt > evt) {
  //      b->SetAddress(0);
  //      while (num > 0) {
  //        b->Fill();
  //        --num;
  //      }
  //    }
  //    b->SetAddress(&ptr);
  //    int nbytes = b->Fill();
  //    if (nbytes < 0) {
  //      throw runtime_error("Failed to write ROOT collection:" + nam + "!");
  //    }
  //    return nbytes;
  //  }
  //  return 0;
  //}

  void Geant4Output2Podio::saveRun(const G4Run* /* run */) {
  }

  /// Commit data at end of filling procedure
  void Geant4Output2Podio::commit(OutputContext<G4Event>& ctxt) {
    if (writer)  writer->writeEvent();
    store.clearCollections();
  }


  /// Callback to store the Geant4 event
  void Geant4Output2Podio::saveEvent(OutputContext<G4Event>& /* ctxt */) {
    if (!writer){
      //std::cout << "creating writer in saveEVent\n";
      writer = new podio::ROOTWriter(m_output, &store);
    }
    if (!m_disableParticles) {
      Geant4ParticleMap* parts = context()->event().extension<Geant4ParticleMap>();
      if (!(store.getCollectionIDTable()->present("mcparticles"))) {
        //std::cout << " creating mcparticles\n";
        auto& pcol = store.create<dd4pod::Geant4ParticleCollection>("mcparticles");
        pcol.clear();
        writer->registerForWrite("mcparticles");
      }
      podio::CollectionBase* podcol = nullptr;
      if (store.get(store.getCollectionIDTable()->collectionID("mcparticles"), podcol)) {
        auto podcol2 = dynamic_cast<dd4pod::Geant4ParticleCollection*>(podcol);
        // copy all the hits
        if (parts) {
          using ParticleMap              = Geant4ParticleMap::ParticleMap;
          const ParticleMap& pm          = parts->particles();
          // loop over particle and copy them to podio data model
          //std::cout << pm.size() <<  " particles\n";
          for (const auto& i : pm) {
            auto part = (dd4hep::sim::Geant4Particle*)(i.second);
            dd4pod::Geant4Particle podpart(
                (int)part->id, (int)part->g4Parent, (int)part->reason, (int)part->mask, (int)part->steps,
                (int)part->secondaries, (int)part->pdgID, (int)part->status,
                std::array<int, 2>{{part->colorFlow[0], part->colorFlow[1]}}, (int)part->genStatus, (int)part->charge,
                std::array<int, 1>{{(int)part->_spare[0]}},
                std::array<float, 3>{{(float)part->spin[0], (float)part->spin[1], (float)part->spin[2]}},
                (double)part->vsx, (double)part->vsy, (double)part->vsz, 
                (double)part->vex, (double)part->vey, (double)part->vez, 
                (double)part->psx/1000.0, (double)part->psy/1000.0, (double)part->psz/1000.0, 
                (double)part->pex/1000.0, (double)part->pey/1000.0, (double)part->pez/1000.0, 
                (double)part->mass/1000.0, (double)part->time, (double)part->properTime);
            for (const auto& ip : part->parents) {
              podpart.addparents(ip);
            }
            for (const auto& ip : part->daughters) {
              podpart.adddaughters(ip);
            }
            podcol2->push_back(podpart);
          }
        }
      }
    }
  }

  /// Callback to store each Geant4 hit collection
  void Geant4Output2Podio::saveCollection(OutputContext<G4Event>& /* ctxt */, G4VHitsCollection* collection) {
    Geant4HitCollection* coll    = dynamic_cast<Geant4HitCollection*>(collection);
    std::string          hc_name = collection->GetName();
    for (const auto& n : m_disabledCollections) {
      if (n == hc_name) {
        return;
      }
    }
    size_t  nhits = coll->GetSize();
    TClass* cl    = TBuffer::GetClass(coll->vector_type().type());
    auto    cn    = cl->GetName();
    if (cl) {
      // populate the collections
      //std::cout << cl->GetName() << "\n";
      // not ideal comparison
      if (std::string("vector<dd4hep::sim::Geant4Calorimeter::Hit*>") == cn) {
        if (!(store.getCollectionIDTable()->present(hc_name))) {
          auto& pcol = store.create<dd4pod::CalorimeterHitCollection>(hc_name);
          writer->registerForWrite(hc_name);
        }
        podio::CollectionBase* podcol = nullptr;
        //std::cout << nhits << " cal hits\n";
        if (store.get(store.getCollectionIDTable()->collectionID(hc_name),
                      podcol)) {
          auto podcol2 =
              dynamic_cast<dd4pod::CalorimeterHitCollection*>(podcol);
          // copy all the hits
          for (size_t i = 0; i < nhits; ++i) {
            dd4hep::sim::Geant4HitData* h = coll->hit(i);

            auto cal_hit =
                dynamic_cast<dd4hep::sim::Geant4Calorimeter::Hit*>(h);
            //Geant4HitData::Contributions& c = cal_hit->truth;
            //for (Geant4HitData::Contributions::iterator j = c.begin(); j != c.end(); ++j) {
            //  Geant4HitData::Contribution& t       = *j;
            //  int                          trackID = t.trackID;
            //  t.trackID = m_truth->particleID(trackID);
            //}
            if (cal_hit) {
              auto phit = podcol2->create();
              phit.cellID(cal_hit->cellID);
              phit.flag(cal_hit->flag);
              phit.g4ID(cal_hit->g4ID);
              phit.position({cal_hit->position.x(), cal_hit->position.y(),
                             cal_hit->position.z(), 0.0});
              const auto& first_truth = cal_hit->truth[0];
              phit.truth(dd4pod::MonteCarloContrib({
                (int)first_truth.trackID, 
                (int)first_truth.pdgID,
                (double)first_truth.deposit/1000.0,
                (double)first_truth.time, 
                (double)first_truth.length,
                (double)first_truth.x, 
                (double)first_truth.y,
                (double)first_truth.z}));
              //phit.truth({cal_hit->truth.trackID, cal_hit->truth.pdgID, cal_hit->truth.deposit, cal_hit->truth.time,
              //            cal_hit->truth.length, cal_hit->truth.x, cal_hit->truth.y, cal_hit->truth.z});
              phit.energyDeposit(cal_hit->energyDeposit/1000.0);
            }
          }
        }
      } else if (std::string("vector<dd4hep::sim::Geant4Tracker::Hit*>") == cn) {
        //std::cout << "track hit\n";
        if (!(store.getCollectionIDTable()->present(hc_name))) {
          auto& pcol = store.create<dd4pod::TrackerHitCollection>(hc_name);
          writer->registerForWrite(hc_name);
        }
        podio::CollectionBase* podcol = nullptr;
        //std::cout << nhits << " trk hits\n";
        if (store.get(store.getCollectionIDTable()->collectionID(hc_name), podcol)) {
          auto podcol2 =
              dynamic_cast<dd4pod::TrackerHitCollection*>(podcol);
          // copy all the hits
          for (size_t i = 0; i < nhits; ++i) {
            dd4hep::sim::Geant4HitData* h = coll->hit(i);
            auto trk_hit = dynamic_cast<dd4hep::sim::Geant4Tracker::Hit*>(h);

            //Geant4HitData::Contribution& t       = trk_hit->truth;
            //int                          trackID = t.trackID;
            //t.trackID                            = m_truth->particleID(trackID);
            if (trk_hit) {
              auto phit = podcol2->create();
              phit.cellID(trk_hit->cellID);
              phit.flag(trk_hit->flag);
              phit.g4ID(trk_hit->g4ID);
              phit.position({trk_hit->position.x(), trk_hit->position.y(),
                             trk_hit->position.z()});
              phit.momentum({trk_hit->momentum.x()/1000.0, trk_hit->momentum.y()/1000.0,
                             trk_hit->momentum.z()/1000.0});
              phit.length(trk_hit->length);
              phit.truth(dd4pod::MonteCarloContrib({
                (int)trk_hit->truth.trackID, 
                (int)trk_hit->truth.pdgID,
                (double)trk_hit->truth.deposit/1000.0,
                (double)trk_hit->truth.time, 
                (double)trk_hit->truth.length,
                (double)trk_hit->truth.x, 
                (double)trk_hit->truth.y,
                (double)trk_hit->truth.z}));
              phit.energyDeposit(trk_hit->energyDeposit/1000.0);
            }
          }
        }
      } else if (std::string("vector<npdet::PMTHit*>") == cn) {
        //std::cout << "track hit\n";
        if (!(store.getCollectionIDTable()->present(hc_name))) {
          auto& pcol = store.create<dd4pod::PhotoMultiplierHitCollection>(hc_name);
          writer->registerForWrite(hc_name);
        }
        podio::CollectionBase* podcol = nullptr;
        if (store.get(store.getCollectionIDTable()->collectionID(hc_name), podcol)) {
          auto podcol2 =
              dynamic_cast<dd4pod::PhotoMultiplierHitCollection*>(podcol);
          // copy all the hits
          for (size_t i = 0; i < nhits; ++i) {
            //auto h = coll->hit(i);
            dd4hep::sim::Geant4HitData* h = coll->hit(i);
            auto trk_hit = dynamic_cast<npdet::PMTHit*>(h);
            //Geant4HitData::Contribution& t       = trk_hit->truth;
            //int                          trackID = t.trackID;
            //t.trackID                            = m_truth->particleID(trackID);
            if (trk_hit) {
              auto phit = podcol2->create();
              phit.cellID(trk_hit->cellID);
              phit.flag(trk_hit->flag);
              phit.g4ID(trk_hit->g4ID);
              phit.position({trk_hit->position.x(), trk_hit->position.y(),
                             trk_hit->position.z()});
              phit.momentum({trk_hit->momentum.x()/1000.0, trk_hit->momentum.y()/1000.0,
                             trk_hit->momentum.z()/1000.0});
              phit.length(trk_hit->length);
              phit.truth({trk_hit->truth.trackID, trk_hit->truth.pdgID,
                trk_hit->truth.deposit/1000.0,
                trk_hit->truth.time, trk_hit->truth.length,
                trk_hit->truth.x, trk_hit->truth.y,
                trk_hit->truth.z});
              phit.energy(trk_hit->energy/1000.0);
            }
          }
        }
      } else { 
         //std::cout << " unknown hit class name " << cn  << "\n";
      }
    } else {
      throw std::runtime_error("No ROOT TClass object availible for object type" );
    }
    //
    // if (coll) {
    //  vector<void*> hits;
    //  coll->getHitsUnchecked(hits);
    //  size_t nhits = coll->GetSize();
    //  if (m_handleMCTruth && m_truth && nhits > 0) {
    //    hits.reserve(nhits);
    //    try {
    //      for (size_t i = 0; i < nhits; ++i) {
    //        auto h       = coll->hit(i);
    //        auto trk_hit = dynamic_cast<dd4pod::TrackerHit*>(h);
    //        if (0 != trk_hit) {
    //          Geant4HitData::Contribution& t       = trk_hit->truth;
    //          int                          trackID = t.trackID;
    //          t.trackID                            =
    //          m_truth->particleID(trackID);
    //        }
    //        auto cal_hit = dynamic_cast<dd4pod::CalorimeterHit*>(h);
    //        if (0 != cal_hit) {
    //          Geant4HitData::Contributions& c = cal_hit->truth;
    //          for (Geant4HitData::Contributions::iterator j = c.begin();
    //               j != c.end(); ++j) {
    //            Geant4HitData::Contribution& t       = *j;
    //            int                          trackID = t.trackID;
    //            t.trackID = m_truth->particleID(trackID);
    //          }
    //        }
    //      }
    //    } catch (...) {
    //      printout(ERROR, name(), "+++ Exception while saving collection %s.",
    //               hc_nam.c_str());
    //    }
    //  }
    //  fill(hc_nam, coll->vector_type(), &hits);
    //  }
  }
}  // namespace dd4hep::sim

using namespace dd4hep::sim;

#include "DDG4/Factories.h"

// clang-format off
DECLARE_GEANT4ACTION(Geant4Output2Podio)
