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
    class DigiDepositRecalibEnergy : public DigiDepositsProcessor   {
    protected:
      /// Property: Polynomial parameters to recalibrate the energy
      std::vector<double> m_polynomial        {  };
      /// Property: Energy base point to compute delta(energy)
      double              m_e0                { 0e0 };

    public:
      /// Standard constructor
      DigiDepositRecalibEnergy(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositsProcessor(krnl, nam)
      {
	declareProperty("e0", m_e0);
	declareProperty("parameters" , m_polynomial);
	DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositRecalibEnergy::recalibrate_deposit_energy)
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void 
      recalibrate_deposit_energy(DigiContext& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
	std::size_t updated = 0UL;
	for( auto& dep : cont )    {
	  if ( predicate(dep) )   {
	    double deposit = dep.second.deposit;
	    double delta_E = deposit - m_e0;
	    double fac     = 1e0;
	    for( double param : m_polynomial )   {
	      deposit += param * fac;
	      fac     *= delta_E;
	    }
	    if ( m_monitor ) m_monitor->energy_shift(dep, delta_E);
	    dep.second.deposit = deposit;
	    dep.second.flag |= EnergyDeposit::RECALIBRATED;
	    ++updated;
	  }
	}
	info("%s+++ %-32s Recalibrating: updated %6ld out of %6ld entries from mask: %04X",
	     context.event->id(), cont.name.c_str(), updated, cont.size(), cont.key.mask());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositRecalibEnergy)
