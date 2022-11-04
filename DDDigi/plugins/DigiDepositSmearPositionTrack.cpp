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

#include <DD4hep/InstanceCount.h>
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
    class DigiDepositSmearPositionTrack : public DigiContainerProcessor   {
    protected:
      /// Property: Energy cutoff. No hits will be merged with a deposit smaller
      double m_deposit_cutoff   { detail::numeric_epsilon };
      /// Property: Resolution in loacl sensor's U coordinates along (1, 0, 0)
      double m_resolution_u     { 0e0 * dd4hep::mm };
      /// Property: Resolution in loacl sensor's V coordinates along (0, 1, 0)
      double m_resolution_v     { 0e0 * dd4hep::mm };
      /// Property: Flag to update existing container in-place or create a new container
      bool   m_update_in_place  { true  };

    public:
      /// Standard constructor
      DigiDepositSmearPositionTrack(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("deposit_cutoff",             m_deposit_cutoff);
	declareProperty("update_in_place",            m_update_in_place = true);
	declareProperty("resolution_u",               m_resolution_u);
	declareProperty("resolution_v",               m_resolution_v);
	InstanceCount::increment(this);
      }

      /// Default destructor
      virtual ~DigiDepositSmearPositionTrack() {
	InstanceCount::decrement(this);
      }

      /// Create deposit mapping with updates on same cellIDs
      template <typename T> void smear_deposit_position(DigiContext& context, T& cont, work_t& work)  const  {
	constexpr double eps = detail::numeric_epsilon;
	T output_cont(cont.name, work.output.mask);
	auto& random = context.randomGenerator();
	const auto& ev = *(context.event);
	std::size_t killed = 0UL;
	std::size_t updated = 0UL;
	std::size_t created = 0UL;

	if ( dd4hep::isActivePrintLevel(outputLevel()) )   {
	  print("%s+++ Smearing container: %s %6ld entries",
		context.event->id(), cont.name.c_str(), cont.size());
	}
	VolumeManager volMgr = m_kernel.detectorDescription().volumeManager();
	for( auto& dep : cont )    {
	  CellID cell = dep.first;
	  const EnergyDeposit& depo = dep.second;
	  double deposit = depo.deposit;
	  if ( deposit >= m_deposit_cutoff )   {
	    auto*     ctxt = volMgr.lookupContext(cell);
	    Direction part_momentum = depo.history.average_particle_momentum(ev);
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

	    delta_pos = newpos - oldpos;
	    info("+++ %016lX Smeared position [%9.2e %9.2e %9.2e] by [%9.2e %9.2e %9.2e] to [%9.2e %9.2e %9.2e] [mm]",
		 oldpos.X(), oldpos.Y(), oldpos.Z(), delta_pos.X(), delta_pos.Y(), delta_pos.Z(),
		 newpos.X(), newpos.Y(), newpos.Z());

	    /// Update / create deposit with smeared position
	    if ( m_update_in_place )   {
	      EnergyDeposit& d = dep.second;
	      d.position = newpos;
	      d.flag |= EnergyDeposit::POSITION_SMEARED;
	      ++updated;
	    }
	    else   {
	      EnergyDeposit d(depo);
	      d.position = newpos;
	      d.flag |= EnergyDeposit::POSITION_SMEARED;
	      output_cont.emplace(cell, std::move(d));
	      ++created;
	    }
	  }
	  else if ( !m_update_in_place )   {
	    EnergyDeposit& d = dep.second;
	    d.flag |= EnergyDeposit::KILLED;
	    ++killed;
	  }
	}

	if ( !m_update_in_place )   {
	  work.output.data.put(output_cont.key, std::move(output_cont));
	}
	info("%s+++ %-32s Smearing: created %6ld updated %6ld killed %6ld entries from mask: %04X",
	     context.event->id(), cont.name.c_str(), created, updated, killed, cont.key.mask());
      }

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work)  const override final  {
	if ( auto* v = work.get_input<DepositVector>() )
	  smear_deposit_position(context, *v, work);
	else if ( auto* m = work.get_input<DepositMapping>() )
	  smear_deposit_position(context, *m, work);
	else
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositSmearPositionTrack)
