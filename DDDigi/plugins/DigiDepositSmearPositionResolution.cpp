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
#include <DDDigi/DigiKernel.h>

#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/Detector.h>
#include <DD4hep/VolumeManager.h>

/// C/C++ include files
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to smear geographically energy deposits
    /**
     *
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositSmearPositionResolution : public DigiDepositsProcessor   {
    protected:
      /// Property: Resolution in loacl sensor's U coordinates along (1, 0, 0)
      double m_resolution_u     { 0e0 * dd4hep::mm };
      /// Property: Resolution in loacl sensor's V coordinates along (0, 1, 0)
      double m_resolution_v     { 0e0 * dd4hep::mm };

      /// Optional local position monitor
      DigiDepositMonitor* m_local_monitor { nullptr };

    public:
      /// Standard constructor
      DigiDepositSmearPositionResolution(const DigiKernel& krnl, const std::string& nam)
        : DigiDepositsProcessor(krnl, nam)
      {
        declareProperty("resolution_u",               m_resolution_u);
        declareProperty("resolution_v",               m_resolution_v);
        DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositSmearPositionResolution::smear);
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      smear(DigiContext& context, T& cont, work_t& /* work */, const predicate_t& predicate)  const  {
        VolumeManager volMgr = m_kernel.detectorDescription().volumeManager();
        auto& random = context.randomGenerator();
        std::size_t updated = 0UL;

        for( auto& dep : cont )    {
          if ( predicate(dep) )   {
            CellID cell = dep.first;
            EnergyDeposit& depo = dep.second;
            auto*     ctxt = volMgr.lookupContext(cell);
            Position  local_pos = ctxt->worldToLocal(depo.position);
            double    delta_u   = m_resolution_u * random.gaussian();
            double    delta_v   = m_resolution_v * random.gaussian();
            Position  delta_pos(delta_u, delta_v, 0e0);
            Position  oldpos = depo.position;
            Position  newpos = ctxt->localToWorld(local_pos + delta_pos);

            if ( m_local_monitor ) m_local_monitor->position_shift(dep, local_pos, delta_pos);
            delta_pos = newpos - oldpos;
            if ( m_monitor ) m_monitor->position_shift(dep, oldpos, delta_pos);

            info("+++ %016lX Smeared position [%9.2e %9.2e %9.2e] by [%9.2e %9.2e %9.2e] to [%9.2e %9.2e %9.2e] [mm]",
                 oldpos.X(), oldpos.Y(), oldpos.Z(), delta_pos.X(), delta_pos.Y(), delta_pos.Z(),
                 newpos.X(), newpos.Y(), newpos.Z());

            depo.position = newpos;
            depo.flag |= EnergyDeposit::POSITION_SMEARED;
            ++updated;
          }
        }
        info("%s+++ %-32s Smear position(resolution): updated %6ld out of %6ld entries from mask: %04X",
             context.event->id(), cont.name.c_str(), updated, cont.size(), cont.key.mask());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSmearPositionResolution)
