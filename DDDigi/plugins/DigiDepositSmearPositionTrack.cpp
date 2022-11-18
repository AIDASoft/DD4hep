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
    class DigiDepositSmearPositionTrack : public DigiDepositsProcessor   {
    protected:
      /// Property: Resolution in loacl sensor's U coordinates along (1, 0, 0)
      double m_resolution_u     { 0e0 * dd4hep::mm };
      /// Property: Resolution in loacl sensor's V coordinates along (0, 1, 0)
      double m_resolution_v     { 0e0 * dd4hep::mm };

      /// Optional local position monitor
      DigiDepositMonitor* m_local_monitor { nullptr };

    public:
      /// Standard constructor
      DigiDepositSmearPositionTrack(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositsProcessor(krnl, nam)
      {
	declareProperty("resolution_u",               m_resolution_u);
	declareProperty("resolution_v",               m_resolution_v);
	DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiDepositSmearPositionTrack::smear)
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void
      smear(DigiContext& context, T& cont, work_t& work, const predicate_t& predicate)  const  {
	constexpr double eps = detail::numeric_epsilon;
	auto& random = context.randomGenerator();
	const auto& ev = *(context.event);
	std::size_t updated = 0UL;
	auto* h = work.get_history(cont.name);

	VolumeManager volMgr = m_kernel.detectorDescription().volumeManager();
	for( auto& dep : cont )    {
	  if ( predicate(dep) )   {
	    CellID cell = dep.first;
	    EnergyDeposit& depo = dep.second;
	    auto*     ctxt = volMgr.lookupContext(cell);
#ifdef DDDIGI_INPLACE_HISTORY
	    Direction part_momentum = depo.history.average_particle_momentum(ev);
#else
	    Direction part_momentum = depo.momentum;
#endif
	    Position  local_pos = ctxt->worldToLocal(depo.position);
	    Position  local_dir = ctxt->worldToLocal(part_momentum).unit();
	    double    cos_u   = local_dir.Dot(Position(1,0,0));
	    double    sin_u   = std::sqrt(1e0 - cos_u*cos_u);
	    double    tan_u   = sin_u/(std::abs(cos_u)>eps ? cos_u : eps);
	    double    delta_u = tan_u * m_resolution_u * random.gaussian();

	    double    cos_v   = local_dir.Dot(Position(0,1,0));
	    double    sin_v   = std::sqrt(1e0 - cos_v*cos_v);
	    double    tan_v   = sin_v/(std::abs(cos_v)>eps ? cos_v : eps);
	    double    delta_v = tan_v * m_resolution_v * random.gaussian();

	    Position  delta_pos(delta_u, delta_v, 0e0);
	    Position  oldpos = depo.position;
	    Position  newpos = ctxt->localToWorld(local_pos + delta_pos);

	    if ( m_local_monitor ) m_local_monitor->position_shift(dep, local_pos, delta_pos);
	    delta_pos = newpos - oldpos;
	    if ( m_monitor ) m_monitor->position_shift(dep, oldpos, delta_pos);

	    info("%s+++ %016lX Smeared position [%9.2e %9.2e %9.2e] by [%9.2e %9.2e %9.2e] to [%9.2e %9.2e %9.2e] [mm]",
		 ev.id(), oldpos.X(), oldpos.Y(), oldpos.Z(), delta_pos.X(), delta_pos.Y(), delta_pos.Z(),
		 newpos.X(), newpos.Y(), newpos.Z());

	    depo.position = newpos;
	    depo.flag |= EnergyDeposit::POSITION_SMEARED;
	    ++updated;
	  }
	}
	info("%s+++ %-32s Smear position(track): updated %6ld out of %6ld entries from mask: %04X",
	     context.event->id(), cont.name.c_str(), updated, cont.size(), cont.key.mask());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSmearPositionTrack)
