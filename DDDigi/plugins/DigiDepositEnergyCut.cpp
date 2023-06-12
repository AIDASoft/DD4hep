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
    class DigiDepositEnergyCut : public DigiDepositsProcessor   {
    protected:
      /// Property: Energy cutoff. No hits will be merged with a deposit smaller
      double m_cutoff { -std::numeric_limits<double>::epsilon() };

    public:
      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      cut_energy(context_t& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
        std::size_t dropped = 0UL;
        for( auto& dep : cont )    {
          if ( predicate(dep) )   {
            EnergyDeposit& depo = dep.second;
            if ( depo.deposit < m_cutoff )   {
              depo.flag |= EnergyDeposit::KILLED;
              ++dropped;
            }
          }
        }
        if ( m_monitor ) m_monitor->count_shift(cont.size(), dropped);
        info("%s+++ %-32s dropped %6ld out of %6ld entries from mask: %04X",
             context.event->id(), cont.name.c_str(), dropped, cont.size(), cont.key.mask());
      }

      /// Standard constructor
      DigiDepositEnergyCut(const DigiKernel& krnl, const std::string& nam)
        : DigiDepositsProcessor(krnl, nam)
      {
        declareProperty("deposit_cutoff", m_cutoff);
        DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositEnergyCut::cut_energy);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositEnergyCut)
