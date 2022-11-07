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
#include <DDDigi/DigiContainerProcessor.h>
#include <DD4hep/InstanceCount.h>

/// C/C++ include files
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to merge energy deposits weighted with their energy deposit
    /**
     *  The selected deposits are placed in the output container
     *  supplied by the arguments. Multiple CellIDs will be merged to one single 
     *  deposit, where the merge computes the resulting position and
     *  momentum according to the contribution of the hits.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositWeightedPosition : public DigiDepositsProcessor   {
    protected:
      /// Property: Energy cutoff. No hits will be merged with a deposit smaller
      double m_cutoff { -std::numeric_limits<double>::epsilon() };

    public:
      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      create_deposits(context_t& context, const T& cont, work_t& work, const predicate_t& predicate)  const  {
	Key key(cont.name, work.output.mask);
	DepositMapping m(cont.name, work.output.mask);
	std::size_t dropped = 0UL, updated = 0UL, added = 0UL;
	for( const auto& dep : cont )    {
	  if ( predicate(dep) )   {
	    const EnergyDeposit& depo = dep.second;
	    if ( depo.deposit >= m_cutoff )   {
	      CellID cell = dep.first;
	      auto   iter = m.data.find(cell);
	      if ( iter == m.data.end() )
		m.data.emplace(dep.first, depo), ++added;
	      else
		iter->second.update_deposit_weighted(depo), ++updated;
	      continue;
	    }
	    ++dropped;
	  }
	}
	info("%s+++ %-32s added %6ld updated %6ld dropped %6ld entries (now: %6ld) from mask: %04X to mask: %04X",
	     context.event->id(), cont.name.c_str(), added, updated, dropped, m.size(), cont.key.mask(), m.key.mask());
	work.output.data.put(m.key, std::move(m));
      }

      /// Standard constructor
      DigiDepositWeightedPosition(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositsProcessor(krnl, nam)
      {
	InstanceCount::increment(this);
	declareProperty("deposit_cutoff", m_cutoff);
	DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositWeightedPosition::create_deposits)
      }

      /// Default destructor
      virtual ~DigiDepositWeightedPosition() {
	InstanceCount::decrement(this);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositWeightedPosition)
