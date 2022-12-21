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

    /// Actor to smear timing information of energy deposits
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositNoiseOnSignal : public DigiDepositsProcessor  {
    protected:
      /// Property: Mean of the added noise on signal in absolute values 
      double m_mean                { 0e0 };
      /// Property: Sigma of the noise in absolute values
      double m_sigma               { 0e0 };

    public:
      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      create_noise(DigiContext& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
	auto& random = context.randomGenerator();
	std::size_t updated = 0UL;
	for( auto& dep : cont )  {
	  if ( predicate(dep) )  {
	    int flag = EnergyDeposit::DEPOSIT_NOISE;
	    double delta_E = random.gaussian(m_mean, m_sigma);
	    if ( m_monitor ) m_monitor->energy_shift(dep, delta_E);
	    dep.second.deposit += delta_E;
	    dep.second.flag |= flag;
	    ++updated;
	  }
	}
	info("%s+++ %-32s Noise on signal: %6ld entries, updated %6ld entries. mask: %04X",
	     context.event->id(), cont.name.c_str(), cont.size(), updated, cont.key.mask());
      }

      /// Standard constructor
      DigiDepositNoiseOnSignal(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositsProcessor(krnl, nam)
      {
	declareProperty("mean",  m_mean);
	declareProperty("sigma", m_sigma);
	DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositNoiseOnSignal::create_noise)
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep
/// Factory instantiation:
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositNoiseOnSignal)
