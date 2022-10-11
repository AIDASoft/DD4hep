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

typedef std::function<std::any(int, const char*, void*)> func_t;

namespace  {

  template <typename T> union input_data  {
    const void*      raw;
    std::vector<T*>* items;
    input_data(const void* p)   { this->raw = p; }
  };

  template <typename T> void* ddg4_hit_convert_function(const char* desc)   {
    std::string tag = desc;
    func_t* cnv = new func_t([tag] (int mask, const char* name, void* ptr)  {
	using wrap_t = std::shared_ptr<sim::Geant4HitData>;
	std::size_t len = 0;
	digi::DepositMapping out(name, mask);
	if ( ptr )   {
	  input_data<T> data(ptr);
	  digi::Key::mask_type msk = digi::Key::mask_type(mask);
	  for( auto* p : *data.items )   {
	    auto it = out.find(p->cellID);
	    if ( it == out.end() )   {
	      digi::EnergyDeposit dep(p->energyDeposit);
	      dep.history.emplace_back(std::any(wrap_t(p)), msk);
	      out.emplace(p->cellID, std::move(dep));
	      continue;
	    }
	    (*it).second.history.emplace_back(std::any(wrap_t(p)), msk);
	    (*it).second.deposit += p->energyDeposit;
	  }
	  len = data.items->size();
	  data.items->clear();
	}
	printout(DEBUG,"DDG4Converter","++ Converted %ld %s to %ld cell deposits",
		 len, tag.c_str(), out.size());
	return std::make_any<digi::DepositMapping>(std::move(out));
      });
    return cnv;
  }
}

static void* convert_sim_geant4calorimeter_hits()     {
  return ddg4_hit_convert_function<sim::Geant4Calorimeter::Hit>("DDG4 calorimeter hits");
}
DECLARE_CREATE(DD4hep_DDDigiConverter_vector_dd4hep_sim_Geant4Calorimeter_Hit_,convert_sim_geant4calorimeter_hits);

static void* convert_sim_geant4tracker_hits()     {
  return ddg4_hit_convert_function<sim::Geant4Tracker::Hit>("DDG4 tracker hits");
}
DECLARE_CREATE(DD4hep_DDDigiConverter_vector_dd4hep_sim_Geant4Tracker_Hit_,convert_sim_geant4tracker_hits);

static void* convert_sim_geant4particles()     {
  func_t* cnv = new func_t([] (int mask, const char* name, void* ptr)  {
      std::any res = std::make_any<digi::ParticleMapping>(name, mask);
      digi::ParticleMapping* out = std::any_cast<digi::ParticleMapping>(&res);
      if ( ptr )   {
	using wrap_t = std::shared_ptr<sim::Geant4Particle>;
	auto* items = (std::vector<sim::Geant4Particle*>*)ptr;
	for( auto* p : *items )   {
	  digi::Key key(0);
	  key.values.mask = mask;
	  key.values.item = out->size();
	  p->mask = mask;
	  out->push(key.key, {std::make_any<wrap_t>(p)});
	}
	items->clear();
      }
      printout(DEBUG,"DDG4Converter","++ Converted %ld DDG4 particles", out->size());
      return res;
    });
  return cnv;
}
DECLARE_CREATE(DD4hep_DDDigiConverter_vector_dd4hep_sim_Geant4Particle_,convert_sim_geant4particles);
