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
#include <DDDigi/DigiROOTInput.h>
#include "DigiIO.h"

#include <DDG4/Geant4Data.h>
#include <DDG4/Geant4Particle.h>

/// ROOT include files
#include <TROOT.h>
#include <TBranch.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// DDDigi input reader for DDG4 native ROOT output
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
   class DigiDDG4ROOT : public DigiROOTInput    {
    public:
      /// Class pointers of the objects to be imported
      TClass* m_trackerHitClass { nullptr };
      TClass* m_caloHitClass    { nullptr };
      TClass* m_particlesClass  { nullptr };

    public:
      /// Initializing constructor
      DigiDDG4ROOT(const DigiKernel& krnl, const std::string& nam) : DigiROOTInput(krnl, nam)      {
	m_particlesClass  = gROOT->GetClass(typeid(std::vector<dd4hep::sim::Geant4Particle*>), kTRUE);
	m_trackerHitClass = gROOT->GetClass(typeid(std::vector<dd4hep::sim::Geant4Tracker::Hit*>), kTRUE);
	m_caloHitClass    = gROOT->GetClass(typeid(std::vector<dd4hep::sim::Geant4Calorimeter::Hit*>), kTRUE);
	assert(m_particlesClass != 0);
	assert(m_trackerHitClass != 0);
	assert(m_caloHitClass != 0);
      }

      /// Convert DDG4 hit collections collection
      template <typename T>
      void from_dd4g4(DigiContext& context,
		      DataSegment& segment,
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
	  data_io<ddg4_input>::_to_digi_if(data.get(), hits, predicate);
	  data_io<ddg4_input>::_to_digi(Key(nam, segment.id, mask), hits, out);
	  data.clear();
	}
	info("%s+++ %-24s Converted %6ld DDG4 %-14s hits to %6ld cell deposits",
	     context.event->id(), nam, len, tag.c_str(), out.size());
	put_data(segment, Key(out.name, mask), out);
	if ( m_keep_raw )   {
	  put_data(segment, Key(std::string(nam)+".ddg4", mask, segment.id), hits);
	}
      }
      /// Convert DDG4 MC particle collection
      void from_dd4g4(DigiContext& context,
		      DataSegment& segment,
		      int mask,
		      const std::string& nam,
		      void* ptr)   const
      {
	ParticleMapping particles(nam, mask);
	if ( ptr )   {
	  input_data<sim::Geant4Particle> data(ptr);
	  data_io<ddg4_input>::_to_digi(Key(nam, segment.id, mask), data.get(), particles);
	  data.clear();
	}
	debug("%s+++ Converted %ld DDG4 particles", context.event->id(), particles.size());
	put_data(segment, Key(nam, mask), particles);
      }

      /// Callback to handle single branch
      virtual void operator()(DigiContext& context, work_t& work)  const  override  {
	TBranch& br = work.container.branch;
	void**  add = (void**)br.GetAddress();
	int     msk = work.container.key.mask();
	TClass* cls = &work.container.clazz;
	auto&   seg = work.segment;
	const char* nam = br.GetName();

	if ( cls == m_caloHitClass )
	  from_dd4g4<sim::Geant4Calorimeter::Hit>(context, seg, "calorimeter", msk, nam, *add);
	else if ( cls == m_trackerHitClass )
	  from_dd4g4<sim::Geant4Tracker::Hit>(context, seg, "tracker", msk, nam, *add);
	else if ( cls == m_particlesClass )
	  from_dd4g4(context, seg, msk, nam, *add);
	else
	  except("Unknown data type encountered in branch: %s", nam);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

/// Factory instantiation
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDDG4ROOT)
