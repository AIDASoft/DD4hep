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

    /// Actor to zero-suppress a container of energy deposits
    /**
     *
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositZeroSuppress : public DigiDepositsProcessor   {
    protected:
      /// Property: Intrinsic energy resolution constant (gaussian ~ std::sqrt(energy))
      double m_energy_threshold                { 0e0 };

    public:
      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      handle_deposits(DigiContext& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
        std::size_t killed  = 0UL;
        std::size_t handled = 0UL;
        for( auto& dep : cont )    {
          if ( predicate(dep) )   {
            int flag = EnergyDeposit::ZERO_SUPPRESSED;
            if ( dep.second.deposit * dd4hep::GeV < m_energy_threshold )   {
              flag |= EnergyDeposit::KILLED;
              ++killed;
            }
            dep.second.flag |= flag;
            ++handled;
          }
        }
        info("%s+++ %-32s Zero suppression: entries: %6ld handled: %6ld killed %6ld entries from mask: %04X",
             context.event->id(), cont.name.c_str(), cont.size(), handled, killed, cont.key.mask());
      }
      /// Standard constructor
      DigiDepositZeroSuppress(const DigiKernel& krnl, const std::string& nam)
        : DigiDepositsProcessor(krnl, nam)
      {
        declareProperty("threshold", m_energy_threshold);
        DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositZeroSuppress::handle_deposits);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositZeroSuppress)
