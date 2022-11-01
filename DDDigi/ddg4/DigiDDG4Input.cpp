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
#include <DD4hep/Factories.h>
#include <DD4hep/DD4hepUnits.h>
#include <DDG4/Geant4Data.h>
#include <DDG4/Geant4Particle.h>
#include <DDDigi/DigiData.h>
#include <CLHEP/Units/SystemOfUnits.h>

// ROOT include files

// C/C++ include files
#include <stdexcept>
#include <any>

using namespace dd4hep;
using namespace dd4hep::digi;

typedef std::function<void(DataSegment& segment, int, const char*, void*)> func_t;

namespace  {

  double epsilon = std::numeric_limits<double>::epsilon();

  template <typename T> union input_data  {
    const void*      raw;
    std::vector<T*>* items;
    input_data(const void* p)   { this->raw = p; }
  };

  template <typename T> void add_particle_history(const T* hit, Key key, History& hist);

  template <> void add_particle_history(const sim::Geant4Calorimeter::Hit* hit, Key key, History& hist)   {
    for( const auto& truth : hit->truth )   {
      key.set_item(truth.trackID);
      hist.particles.emplace_back(key, truth.deposit);
    }
  }
  template <> void add_particle_history(const sim::Geant4Tracker::Hit* hit, Key key, History& hist)   {
    key.set_item(hit->truth.trackID);
    hist.particles.emplace_back(key, hit->truth.deposit);
  }
  template <typename T> void* ddg4_hit_convert_function(const char* desc)   {
    std::string tag = desc;
    func_t* cnv = new func_t([tag] (DataSegment& segment, int mask, const char* name, void* ptr)  {
	using wrap_t = std::shared_ptr<sim::Geant4HitData>;
	std::size_t len = 0;
	std::string nam = name;
	std::vector<wrap_t> geant4_hits;
	DepositVector out(name, mask);
	if ( ptr )   {
	  input_data<T> data(ptr);
	  for(size_t i=0; i < data.items->size(); ++i)   {
	    auto* p = (*data.items)[i];
	    if ( p->energyDeposit > epsilon )   {
	      Key history_key;
	      EnergyDeposit dep { };
	      dep.flag = p->flag;
	      dep.deposit = p->energyDeposit;
	      dep.position = p->position;

	      history_key.set_mask(Key::mask_type(mask));
	      history_key.set_item(geant4_hits.size());
	      history_key.set_segment(segment.id);

	      dep.history.hits.emplace_back(history_key, p->energyDeposit);
	      add_particle_history(p, history_key, dep.history);

	      out.emplace(p->cellID, std::move(dep));
	      geant4_hits.emplace_back(wrap_t(p));
	    }
	  }
	  len = data.items->size();
	  data.items->clear();
	}
	printout(DEBUG,"DDG4Converter","++ Converted %ld %s to %ld cell deposits",
		 len, tag.c_str(), out.size());
	Key depo_key(nam, mask);
	segment.emplace(depo_key, std::make_any<DepositVector>(std::move(out)));
	Key src_key(nam+".ddg4", mask);
	segment.emplace(src_key, std::make_any<std::vector<wrap_t>>(std::move(geant4_hits)));
      });
    return cnv;
  }
}

static void* convert_sim_geant4calorimeter_hits()     {
  return ddg4_hit_convert_function<sim::Geant4Calorimeter::Hit>("DDG4 calorimeter hits");
}
DECLARE_CREATE(DD4hep_DDDigiConverter_vector_dd4hep_sim_Geant4Calorimeter_Hit_,convert_sim_geant4calorimeter_hits)

static void* convert_sim_geant4tracker_hits()     {
  return ddg4_hit_convert_function<sim::Geant4Tracker::Hit>("DDG4 tracker hits");
}
DECLARE_CREATE(DD4hep_DDDigiConverter_vector_dd4hep_sim_Geant4Tracker_Hit_,convert_sim_geant4tracker_hits)

static void* convert_sim_geant4particles()     {
  func_t* cnv = new func_t([] (DataSegment& segment, int mask, const char* name, void* ptr)  {
      using wrap_t  = std::shared_ptr<sim::Geant4Particle>;
      ParticleMapping out(name, mask);
      std::map<Key, wrap_t> source;
      Key part_key(name, mask);
      if ( ptr )   {
	auto* items = (std::vector<sim::Geant4Particle*>*)ptr;
	Key key;
	key.set_segment(segment.id);
	key.set_mask(mask);
	for( auto* p : *items )   {
	  Particle part;
	  p->mask             = mask;
	  part.start_position = Position(p->vsx, p->vsy, p->vsz);
	  part.end_position   = Position(p->vex, p->vey, p->vez);
	  part.momentum       = Direction(p->psx, p->psy, p->psz);
	  part.pdgID          = p->pdgID;
	  part.charge         = p->charge;
	  part.mass           = p->mass;

	  part.history        = key.set_item(part_key.item());
	  key.set_item(out.size());
	  out.push(key, std::move(part));
	  source[key] = wrap_t(p);
	}
	items->clear();
      }
      printout(DEBUG,"DDG4Converter","++ Converted %ld DDG4 particles", out.size());
      std::string nam = name;
      segment.emplace(part_key, std::make_any<ParticleMapping>(std::move(out)));
      Key src_key(nam+".ddg4", mask);
      segment.emplace(src_key, std::make_any<std::map<Key, wrap_t> >(std::move(source)));
    });
  return cnv;
}
DECLARE_CREATE(DD4hep_DDDigiConverter_vector_dd4hep_sim_Geant4Particle_,convert_sim_geant4particles)
