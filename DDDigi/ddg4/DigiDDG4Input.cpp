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
#include <DD4hep/DD4hepUnits.h>

#include <DDG4/Geant4Data.h>
#include <DDG4/Geant4Particle.h>

#include <DDDigi/DigiData.h>
#include <DDDigi/DigiROOTInput.h>
#include <DDDigi/DigiFactories.h>

// ROOT include files
#include <TBranch.h>

// C/C++ include files
#include <stdexcept>
#include <any>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiDDG4ROOT : public DigiROOTInput    {
    public:
      static constexpr double epsilon = std::numeric_limits<double>::epsilon();
      /// Property to generate extra history records
      bool m_seperate_history  { false };
      mutable TClass* m_trackerHitClass { nullptr };
      mutable TClass* m_caloHitClass    { nullptr };
      mutable TClass* m_particlesClass  { nullptr };

      template <typename T> union input_data  {
	const void*      raw;
	std::vector<T*>* items;
	input_data(const void* p)   { this->raw = p; }
      };

      TClass* get_class_pointers(TClass* c , const char* nam) const   {
	if ( strstr(nam, "vector<dd4hep::sim::Geant4Particle*") ) 
	  return m_particlesClass  = c;
	if ( strstr(nam, "vector<dd4hep::sim::Geant4Tracker::Hit*") ) 
	  return m_trackerHitClass = c;
	if ( strstr(nam, "vector<dd4hep::sim::Geant4Calorimeter::Hit*") ) 
	  return m_caloHitClass    = c;
	except("Unknown data class: %s Cannot be handled", nam);
	return c;
      }

      void add_particle_history(const sim::Geant4Calorimeter::Hit* hit, Key key, History& hist)  const {
	for( const auto& truth : hit->truth )   {
	  key.set_item(truth.trackID);
	  hist.particles.emplace_back(key, truth.deposit);
	}
      }
      void add_particle_history(const sim::Geant4Tracker::Hit* hit, Key key, History& hist)  const  {
	key.set_item(hit->truth.trackID);
	hist.particles.emplace_back(key, hit->truth.deposit);
      }

    public:
      /// Initializing constructor
      DigiDDG4ROOT(const DigiKernel& krnl, const std::string& nam)
	: DigiROOTInput(krnl, nam)
      {
	declareProperty("seperate_history", m_seperate_history);
      }

      template <typename T>
      void conv_hits(DataSegment& segment, const std::string& tag, int mask, const char* name, void* ptr)   const {
	using wrap_t = std::shared_ptr<sim::Geant4HitData>;
	std::size_t len = 0;
	std::string nam = name;
	std::vector<wrap_t> geant4_hits;
	DepositVector   out(nam, mask);
	DetectorHistory hist(nam+".hist", mask);
	if ( ptr )   {
	  input_data<T> data(ptr);
	  for(size_t i=0; i < data.items->size(); ++i)   {
	    auto* p = (*data.items)[i];
	    if ( p->energyDeposit > epsilon )   {
	      Key history_key;
	      EnergyDeposit dep { };
	      Position pos = p->position;
	      pos *= 1./dd4hep::mm;

	      dep.flag = p->flag;
	      dep.deposit = p->energyDeposit;
	      dep.position = pos;

	      history_key.set_mask(Key::mask_type(mask));
	      history_key.set_item(geant4_hits.size());
	      history_key.set_segment(segment.id);

	      dep.history.hits.emplace_back(history_key, dep.deposit);
	      add_particle_history(p, history_key, dep.history);

	      out.emplace(p->cellID, std::move(dep));
	      if ( m_seperate_history )   {
		hist.insert(p->cellID, dep.history);
	      }
	      geant4_hits.emplace_back(wrap_t(p));
	    }
	  }
	  len = data.items->size();
	  data.items->clear();
	}
	debug("++ Converted %ld %s to %ld cell deposits", len, tag.c_str(), out.size());
	Key depo_key(out.name, mask);
	segment.emplace(depo_key, std::make_any<DepositVector>(std::move(out)));
	Key src_key(nam+".ddg4", mask);
	segment.emplace(src_key, std::make_any<std::vector<wrap_t>>(std::move(geant4_hits)));
	if ( m_seperate_history )   {
	  Key hist_key(hist.name, mask);
	  segment.emplace(hist_key, std::make_any<DetectorHistory>(std::move(hist)));
	}
      }

      void conv_particles(DataSegment& segment, int mask, const char* name, void* ptr)   const {
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
	debug("++ Converted %ld DDG4 particles", out.size());
	std::string nam = name;
	segment.emplace(part_key, std::make_any<ParticleMapping>(std::move(out)));
	Key src_key(nam+".ddg4", mask);
	segment.emplace(src_key, std::make_any<std::map<Key, wrap_t> >(std::move(source)));
      }

      /// Callback to handle single branch
      virtual void operator()(DigiContext& /* context */, work_t& work)  const  override  {
	TBranch& br = work.branch;
	TClass&  cl = work.cl;
	TClass*  c  = get_class_pointers(&cl, br.GetClassName());
	void** addr = (void**)br.GetAddress();
	const char* nam = br.GetName();
	int mask = work.input_key.mask();
	auto& seg = work.input_segment;
	if ( c == m_caloHitClass )
	  conv_hits<sim::Geant4Calorimeter::Hit>(seg, "DDG4 calorimeter hits", mask, nam, *addr);
	else if ( c == m_trackerHitClass )
	  conv_hits<sim::Geant4Tracker::Hit>(seg, "DDG4 tracker hits", mask, nam, *addr);
	else if ( c == m_particlesClass )
	  conv_particles(seg, mask, nam, *addr);
	else
	  except("Unknown data type encountered in branch: %s", nam);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDDG4ROOT)
