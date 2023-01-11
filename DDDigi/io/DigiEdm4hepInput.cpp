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
#include <DDDigi/DigiROOTInput.h>
#include <DDDigi/DigiFactories.h>
#include "DigiIO.h"

#include <DDG4/Geant4Data.h>

// ROOT include files
#include <TBranch.h>
#include <TClass.h>

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
    
    using namespace std::placeholders;

    class DigiEdm4hepInput : public DigiROOTInput    {
    public:
      static constexpr double epsilon = std::numeric_limits<double>::epsilon();

      /// Property to generate extra history records
      bool m_keep_raw          { true };
      mutable TClass* m_trackerHitClass { nullptr };
      mutable TClass* m_caloHitClass    { nullptr };
      mutable TClass* m_particlesClass  { nullptr };

      TClass* get_class(TClass* c)  const   {
	if ( c == m_particlesClass || c == m_trackerHitClass || c == m_caloHitClass )
	  return c;
	const char* nam = c->GetName();
	if ( strstr(nam, "vector<dd4hep::sim::Geant4Particle*") ) 
	  return m_particlesClass  = c;
	else if ( strstr(nam, "vector<dd4hep::sim::Geant4Tracker::Hit*") ) 
	  return m_trackerHitClass = c;
	else if ( strstr(nam, "vector<dd4hep::sim::Geant4Calorimeter::Hit*") ) 
	  return m_caloHitClass    = c;
	except("Unknown data class: %s Cannot be handled", nam);
	return c;
      }

    public:
      /// Initializing constructor
      DigiEdm4hepInput(const DigiKernel& krnl, const std::string& nam)
	: DigiROOTInput(krnl, nam)
      {
	declareProperty("keep_raw", m_keep_raw);
      }

      template <typename T>
      void conv_hits(DigiContext& context, DataSegment& segment,
		     const std::string& tag,
		     Key::mask_type mask,
		     const char* nam,
		     void* ptr)   const
      {
	DepositVector out(nam, mask, SegmentEntry::UNKNOWN);
	std::map<CellID, std::shared_ptr<T> > hits;
	std::size_t len = 0;
	if ( ptr )   {
	  input_data<T> data(ptr);
	  const DepositPredicate<EnergyCut> predicate ({ this->epsilon });
	  len = data.size();
	  data_io<ddg4_input>()._to_digi_if(data.get(), hits, predicate);
	  data_io<ddg4_input>()._to_digi(Key(nam, segment.id, mask), hits, out);
	  data.clear();
	}
	info("%s+++ %-24s Converted %6ld Edm4hep %-14s hits to %6ld cell deposits",
	     context.event->id(), nam, len, tag.c_str(), out.size());
	put_data(segment, Key(out.name, mask), out);
	if ( m_keep_raw )   {
	  put_data(segment, Key(std::string(nam)+".ddg4", mask, segment.id), hits);
	}
      }

      void conv_particles(DigiContext& context, DataSegment& segment,
			  int mask, const std::string& nam, void* ptr)   const
      {
	ParticleMapping particles(nam, mask);
	if ( ptr )   {
	  input_data<sim::Geant4Particle> data(ptr);
	  data_io<ddg4_input>()._to_digi(Key(nam, segment.id, mask), data.get(), particles);
	  data.clear();
	}
	debug("%s+++ Converted %ld Edm4hep particles", context.event->id(), particles.size());
	put_data(segment, Key(nam, mask), particles);
      }

      /// Callback to handle single branch
      virtual void operator()(DigiContext& context, work_t& work)  const  override  {
	TBranch& br = work.branch;
	int     msk = work.input_key.mask();
	auto&   seg = work.input_segment;
	void**  add = (void**)br.GetAddress();
	TClass* cls = get_class(&work.cl);
	const char* nam = br.GetName();
	if ( cls == m_caloHitClass )
	  conv_hits<sim::Geant4Calorimeter::Hit>(context, seg, "calorimeter", msk, nam, *add);
	else if ( cls == m_trackerHitClass )
	  conv_hits<sim::Geant4Tracker::Hit>(context, seg, "tracker", msk, nam, *add);
	else if ( cls == m_particlesClass )
	  conv_particles(context, seg, msk, nam, *add);
	else
	  except("Unknown data type encountered in branch: %s", nam);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiEdm4hepInput)
