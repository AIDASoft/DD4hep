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
#include <DD4hep/DD4hepUnits.h>

/// C/C++ include files
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to smear geographically energy deposits
    /**
     *   Intrinsic fluctuation:     sigma(E)/E = const(fluctuation) / sqrt(E)
     *                              sigma(E)   = const(fluctuation) * sqrt(E)
     *   Sampling fluctuation:      sigma(E)/E = const(sampling) * sqrt(deltaE/E)
     *                              sigma(E)   = const(sampling) * sqrt(deltaE*E)
     *                              deltaE = energy loss to create single electron/ion
     *                                       pair in sampling layer
     *   Instrumetation:            sigma(E)/E = const(instrumentaion) / E
     *                              sigma(E)   = const(instrumentaion)
     *   Systematics:               sigma(E)/E = const(systematics)
     *                              sigma(E)   = const(systematics) * E
     *
     *   electron conversion
     *   statictical fluctuation:   poissonian change of converted e-ion pairs
     *
     *   Electromagnetic showers:
     *   Shower counters:
     *   Fluctuation resolution: sigma(E)/E ~ 0.005 / sqrt(E)
     *
     *   Sampling calorimeters:
     *   Sampling resolution:    sigma(E)/E ~ 0.04 * sqrt(1000*dE/E)
     *   dE = energy loss of a single charged particle in one sampling layer
     *
     *   Hadronic showers:
     *   Shower counters:
     *   Fluctuation resolution: sigma(E)/E ~ 0.45 / sqrt(E)
     *   With compensation for nuclear effects: sigma(E)/E ~ 0.25 / sqrt(E)
     *   Sampling resolution:    sigma(E)/E ~ 0.09 * sqrt(1000*dE/E)
     *   dE = energy loss of a single charged particle in one sampling layer
     *
     *   (See all: https://handwiki.org/wiki/Physics:Energy_resolution_in_calorimeters)
     *
     *   Properties:
     *   intrinsic_fluctuation:      Intrinsic energy resolution constant (gaussian ~ std::sqrt(energy))
     *   systematic_resolution:      Systematic energy resolution constant (gaussian ~deposit energy)
     *   instrumentation_resolution: Instrumentation energy resolution constant (gaussian(CONSTANT))
     *   pair_ionization_energy:     Ionization constant to create electron ION pair in absorber
     *   ionization_fluctuation:     Flag to use ionization fluctuation smearing (poisson ~ # e-ion-pairs)
     *   modify_energy:              Flag to modify energy deposit according to error - otherwise only compute error
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositSmearEnergy : public DigiDepositsProcessor   {
    protected:
      /// Property: Intrinsic energy resolution constant (gaussian ~ std::sqrt(energy))
      double m_intrinsic_fluctuation      { 0e0 };
      /// Property: Systematic energy resolution constant (gaussian ~deposit energy)
      double m_systematic_resolution      { 0e0 };
      /// Property: Instrumentation energy resolution constant (gaussian(CONSTANT))
      double m_instrumentation_resolution { 0e0 };
      /// Property: Ionization constant to create electron ION pair in absorber
      double m_pair_ionization_energy     { 0e0 };
      /// Property: Flag to use ionization fluctuation smearing (poisson ~ # e-ion-pairs)
      bool   m_ionization_fluctuation     { false };
      /// Property: Flag to modify energy deposit according to error - otherwise only compute error
      bool   m_modify_energy              { true };

    public:
      /// Standard constructor
      DigiDepositSmearEnergy(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositsProcessor(krnl, nam)
      {
	declareProperty("intrinsic_fluctuation",      m_intrinsic_fluctuation = 0e0);
	declareProperty("instrumentation_resolution", m_instrumentation_resolution = 0e0);
	declareProperty("systematic_resolution",      m_systematic_resolution  = 0e0);	
	declareProperty("pair_ionisation_energy",     m_pair_ionization_energy = 3.6*dd4hep::eV);
	declareProperty("ionization_fluctuation",     m_ionization_fluctuation = false);
	declareProperty("modify_energy",              m_modify_energy = true);
	DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositSmearEnergy::smear)
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      smear(DigiContext& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
	auto& random = context.randomGenerator();
	std::size_t updated = 0UL;

	for( auto& dep : cont )    {
	  if ( predicate(dep) )   {
	    CellID cell = dep.first;
	    EnergyDeposit& depo = dep.second;
	    double deposit = depo.deposit;
	    double delta_E = 0e0;
	    double energy  = deposit / dd4hep::GeV; // E in units of GeV
	    double sigma_E_systematic   = m_systematic_resolution * energy;
	    double sigma_E_intrin_fluct = m_intrinsic_fluctuation * std::sqrt(energy);
	    double sigma_E_instrument   = m_instrumentation_resolution / dd4hep::GeV;
	    double delta_ion = 0e0, num_pairs = 0e0;
	    constexpr static double eps = std::numeric_limits<double>::epsilon();
	    if ( sigma_E_systematic > eps )   {
	      delta_E += sigma_E_systematic * random.gaussian(0e0, 1e0);
	    }
	    if ( sigma_E_intrin_fluct > eps )   {
	      delta_E += sigma_E_intrin_fluct * random.gaussian(0e0, 1e0);
	    }
	    if ( sigma_E_instrument > eps )   {
	      delta_E += sigma_E_instrument * random.gaussian(0e0, 1e0);
	    }
	    if ( m_ionization_fluctuation )   {
	      num_pairs = energy / (m_pair_ionization_energy/dd4hep::GeV);
	      delta_ion = energy * (random.poisson(num_pairs)/num_pairs);
	      delta_E += delta_ion;
	    }
	    if ( dd4hep::isActivePrintLevel(outputLevel()) )   {
	      print("%s+++ %016lX [GeV] E:%9.2e [%9.2e %9.2e] intrin_fluct:%9.2e systematic:%9.2e instrument:%9.2e ioni:%9.2e/%.0f",
		    context.event->id(), cell, energy, deposit/dd4hep::GeV, delta_E,
		    sigma_E_intrin_fluct, sigma_E_systematic, sigma_E_instrument, delta_ion, num_pairs);
	    }
	    /// delta_E is in GeV
	    delta_E *= dd4hep::GeV;
	    depo.depositError = delta_E;
	    if ( m_monitor )  {
	      m_monitor->energy_shift(dep, delta_E);
	    }
	    if ( m_modify_energy )  {
	      depo.deposit = deposit + delta_E;
	      depo.flag |= EnergyDeposit::ENERGY_SMEARED;
	    }
	    ++updated;
	  }
	}
	info("%s+++ %-32s Smear energy: updated %6ld out of %6ld entries from mask: %04X",
	     context.event->id(), cont.name.c_str(), updated, cont.size(), cont.key.mask());
      }
    };

    /// Actor to only set energy error (as above, but with preset option
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositSetEnergyError : public DigiDepositSmearEnergy   {
    public:
      /// Standard constructor
      DigiDepositSetEnergyError(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositSmearEnergy(krnl, nam)
      {
	m_modify_energy = false;
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSmearEnergy)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSetEnergyError)
