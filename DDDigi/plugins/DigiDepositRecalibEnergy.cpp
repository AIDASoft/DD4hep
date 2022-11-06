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
    class DigiDepositRecalibEnergy : public DigiContainerProcessor   {
    protected:
      /// Property: Polynomial parameters to recalibrate the energy
      std::vector<double> m_polynomial        {  };
      /// Property: Energy base point to compute delta(energy)
      double              m_e0                { 0e0 };
      /// Property: Flag to update existing container in-place or create a new container
      bool                m_update_in_place   { true };

    public:
      /// Standard constructor
      DigiDepositRecalibEnergy(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("e0", m_e0);
	declareProperty("parameters" , m_polynomial);
	InstanceCount::increment(this);
      }

      /// Default destructor
      virtual ~DigiDepositRecalibEnergy() {
	InstanceCount::decrement(this);
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void recalibrate_deposit_energy(DigiContext& context,
							    T& cont,
							    work_t& work,
							    const predicate_t& predicate)  const  {
	T output_cont(cont.name, work.output.mask);
	std::size_t updated = 0UL;
	std::size_t created = 0UL;
	std::size_t killed  = 0UL;

	info("%s+++ Recalibrarting deposit container: %s %6ld entries",
	     context.event->id(), cont.name.c_str(), cont.size());
	for( auto& dep : cont )    {
	  if ( predicate(dep) )   {
	    double deposit = dep.second.deposit;
	    double delta_E = deposit - m_e0;
	    double fac     = 1e0;
	    for( double param : m_polynomial )   {
	      deposit += param * fac;
	      fac     *= delta_E;
	    }
	    if ( m_update_in_place )   {
	      EnergyDeposit& d = dep.second;
	      d.deposit = deposit;
	      d.flag |= EnergyDeposit::RECALIBRATED;
	      ++updated;
	    }
	    else if ( !(dep.second.flag&EnergyDeposit::KILLED) )  {
	      EnergyDeposit d(dep.second);
	      d.deposit = deposit;
	      d.flag |= EnergyDeposit::RECALIBRATED;
	      output_cont.emplace(dep.first, std::move(d));
	      ++created;
	    }
	    else  {
	      ++killed;
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
	  recalibrate_deposit_energy(context, *v, work, predicate);
	else if ( auto* m = work.get_input<DepositMapping>() )
	  recalibrate_deposit_energy(context, *m, work, predicate);
	else
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositRecalibEnergy)
