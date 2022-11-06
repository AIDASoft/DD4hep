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
#include <DD4hep/DD4hepUnits.h>

/// C/C++ include files
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to smear timing information of energy deposits
    /**
     *
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositSmearTime : public DigiContainerProcessor   {
    protected:
      using limit_t = std::numeric_limits<double>;
      /// Property: Intrinsic energy resolution constant (gaussian ~ std::sqrt(energy))
      double m_resolution_time                { 0e0 };
      /// Property: Time window within the smeared deposits shall be accepted
      std::pair<double, double> m_window_time { limit_t::min(), limit_t::max() };
      /// Property: Flag to update existing container in-place or create a new container
      bool   m_update_in_place                { true };

    public:
      /// Standard constructor
      DigiDepositSmearTime(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("resolution_time", m_resolution_time);
	declareProperty("window_time",     m_window_time);
	InstanceCount::increment(this);
      }

      /// Default destructor
      virtual ~DigiDepositSmearTime() {
	InstanceCount::decrement(this);
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      smear(DigiContext& context, T& cont, work_t& work, const predicate_t& predicate)  const  {
	auto& random = context.randomGenerator();
	T output_cont(cont.name, work.output.mask);
	std::size_t killed  = 0UL;
	std::size_t updated = 0UL;
	std::size_t created = 0UL;

	info("%s+++ TIME smearing deposit container: %s %6ld entries",
	     context.event->id(), cont.name.c_str(), cont.size());
	for( auto& dep : cont )    {
	  if ( predicate(dep) )   {
	    int flag = EnergyDeposit::TIME_SMEARED;
	    double delta_T = m_resolution_time * random.gaussian();
	    if ( delta_T < m_window_time.first || delta_T > m_window_time.second )   {
	      flag |= EnergyDeposit::KILLED;
	      ++killed;
	    }
	    if ( m_update_in_place )   {
	      EnergyDeposit& d = dep.second;
	      d.time += delta_T;
	      d.flag |= flag;
	      ++updated;
	    }
	    else if ( !(flag&EnergyDeposit::KILLED) )  {
	      EnergyDeposit d(dep.second);
	      d.time += delta_T;
	      d.flag |= flag;
	      output_cont.emplace(dep.first, std::move(d));
	      ++created;
	    }
	  }
	}
	if ( !m_update_in_place )   {
	  work.output.data.put(output_cont.key, std::move(output_cont));
	}
	info("%s+++ %-32s Smearing: created %6ld updated %6ld killed %6ld entries from mask: %04X",
	     context.event->id(), cont.name.c_str(), created, updated, killed, cont.key.mask());
      }

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const override final  {
	if ( auto* v = work.get_input<DepositVector>() )
	  smear(context, *v, work, predicate);
	else if ( auto* m = work.get_input<DepositMapping>() )
	  smear(context, *m, work, predicate);
	else
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSmearTime)
