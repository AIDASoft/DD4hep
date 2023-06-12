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
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositSmearTime : public DigiDepositsProcessor  {
    protected:
      using limit_t = std::numeric_limits<double>;
      /// Property: Intrinsic energy resolution constant (gaussian ~ std::sqrt(energy))
      double m_resolution_time                { 0e0 };
      /// Property: Time window within the smeared deposits shall be accepted
      std::pair<double, double> m_window_time { limit_t::min(), limit_t::max() };

    public:
      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      smear(DigiContext& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
        auto& random = context.randomGenerator();
        std::size_t killed  = 0UL;
        std::size_t updated = 0UL;
        for( auto& dep : cont )  {
          if ( predicate(dep) )  {
            int flag = EnergyDeposit::TIME_SMEARED;
            double delta_T = m_resolution_time * random.gaussian();
            if ( delta_T < m_window_time.first || delta_T > m_window_time.second )   {
              flag |= EnergyDeposit::KILLED;
              ++killed;
            }
            if ( m_monitor ) m_monitor->time_shift(dep, delta_T);
            dep.second.time += delta_T;
            dep.second.flag |= flag;
            ++updated;
          }
        }
        if ( m_monitor ) m_monitor->count_shift(cont.size(), -killed);
        info("%s+++ %-32s Smeared time resolution: %6ld entries, updated %6ld killed %6ld entries from mask: %04X",
             context.event->id(), cont.name.c_str(), cont.size(), updated, killed, cont.key.mask());
      }

      /// Standard constructor
      DigiDepositSmearTime(const DigiKernel& krnl, const std::string& nam)
        : DigiDepositsProcessor(krnl, nam)
      {
        declareProperty("resolution_time", m_resolution_time);
        declareProperty("window_time",     m_window_time);
        DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositSmearTime::smear);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSmearTime)
