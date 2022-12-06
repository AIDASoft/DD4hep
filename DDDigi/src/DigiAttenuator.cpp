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
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiPlugins.h>
#include <DDDigi/DigiAttenuator.h>

/// C/C++ include files
#include <cmath>

using namespace dd4hep::digi;

/// Exponential decay depending on time offset(t0) and mean signal decay time
double DigiAttenuationTool::exponential(double t0, double decay_time)  const   {
  return std::exp(-1e0 * std::abs(t0)/decay_time);
}

/// Standard constructor
DigiAttenuator::DigiAttenuator(const DigiKernel& krnl, const std::string& nam)
  : DigiContainerProcessor(krnl, nam)
{
  declareProperty("factor", m_factor);
  InstanceCount::increment(this);
}

/// Default destructor
DigiAttenuator::~DigiAttenuator() {
  InstanceCount::decrement(this);
}

/// Attenuator callback for single container
template <typename T> std::size_t
DigiAttenuator::attenuate(T& cont, const predicate_t& predicate) const {
  for( auto& dep : cont )   {
    if ( predicate(dep) )   {
      dep.second.deposit *= m_factor;
      if ( !m_attenuate_history ) 
	continue;
      auto& e = dep.second.history;
      for( auto& h : e.hits ) h.weight *= m_factor;
      for( auto& h : e.particles ) h.weight *= m_factor;
    }
  }
  return cont.size();
}

/// Attenuator callback for single container
template <> std::size_t
DigiAttenuator::attenuate<DetectorHistory>(DetectorHistory& cont, const predicate_t& /* predicate */) const {
  for( auto& history : cont )   {
    auto& entry = history.second;
    for( auto& h : entry.hits ) h.weight *= m_factor;
    for( auto& h : entry.particles ) h.weight *= m_factor;
  }
  return cont.size();
}

/// Main functional callback adapter
void DigiAttenuator::execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const   {
  std::size_t count = 0;
  if ( auto* m = work.get_input<DepositMapping>() )
    count = this->attenuate(*m, predicate);
  else if ( auto* v = work.get_input<DepositVector>() )
    count = this->attenuate(*v, predicate);
  else if ( auto* h = work.get_input<DetectorHistory>() )
    count = this->attenuate(*h, predicate);
  Key key { work.input.key };
  std::string nam = Key::key_name(key)+":";
  info("%s+++ %-32s mask:%04X item: %08X Attenuated %6ld hits by %8.5f",
	context.event->id(), nam.c_str(), key.mask(), key.item(), count, m_factor); 
}

/// Standard constructor
DigiAttenuatorSequence::DigiAttenuatorSequence(const DigiKernel& krnl, const std::string& nam)
  : DigiContainerSequenceAction(krnl, nam)
{
  declareProperty("processor_type",    m_processor_type = "DigiAttenuator");
  declareProperty("containers",        m_container_attenuation);
  declareProperty("signal_decay",      m_signal_decay = "exponential");
  declareProperty("t0",                m_t0);
  InstanceCount::increment(this);
}

/// Default destructor
DigiAttenuatorSequence::~DigiAttenuatorSequence() {
  InstanceCount::decrement(this);
}

/// Initialization callback
void DigiAttenuatorSequence::initialize()   {
  DigiAttenuationTool tool;
  if ( m_container_attenuation.empty() )   {
    warning("+++ No input containers given for attenuation action -- no action taken");
    return;
  }
  for ( const auto& c : m_container_attenuation )   {
    double factor = 0e0;
    switch( ::toupper(m_signal_decay[0]) )   {
    case 'E':
      factor = tool.exponential(m_t0, c.second);
      break;
    default:
      except("+++ The attenuation function '%s' is not supported ---> STOP", 
	     m_signal_decay.c_str());
      break;
    }
    // Attenuate the signal
    std::string nam = name() + ".D." + c.first;
    auto* att = createAction<DigiAttenuator>(m_processor_type, m_kernel, nam);
    if ( !att )   {
      except("+++ Failed to create signal attenuator: %s of type: %s",
	     nam.c_str(), m_processor_type.c_str());
    }
    att->property("factor").set(factor);
    att->property("OutputLevel").set(int(outputLevel()));
    adopt_processor(att, c.first);
  }
  this->DigiContainerSequenceAction::initialize();
}

/// Main functional callback
void DigiAttenuatorSequence::execute(DigiContext& context)  const    {
  this->DigiContainerSequenceAction::execute(context);
}
