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


///             TODO !


// Framework include files
#include <DD4hep/InstanceCount.h>

#include <DDDigi/DigiData.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiAttenuatorSequenceAction.h>

/// C/C++ include files
#include <cmath>

/// Standard constructor
dd4hep::digi::DigiAttenuatorSequenceAction::DigiAttenuatorSequenceAction(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("input",      m_input_segment = "inputs");
  declareProperty("processor",  m_processor_name);
  declareProperty("containers", m_container_attenuation);
  declareProperty("mask",       m_mask);
  declareProperty("t0",         m_t0);
  m_kernel.register_initialize(Callback(this).make(&DigiAttenuatorSequenceAction::initialize));
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiAttenuatorSequenceAction::~DigiAttenuatorSequenceAction() {
  InstanceCount::decrement(this);
}

/// Initialization callback
void dd4hep::digi::DigiAttenuatorSequenceAction::initialize()   {
  for ( const auto& c : m_container_attenuation )   {
    double factor = std::exp(-1e0 * m_t0/c.second);
    m_attenuation.emplace(Key(c.first, m_mask), factor);
  }
}

/// Main functional callback
void dd4hep::digi::DigiAttenuatorSequenceAction::execute(DigiContext& context)  const    {
  std::size_t count = 0, cnt = 0, cont = 0;
  auto& event  = *context.event;
  auto& inputs = event.get_segment(m_input_segment);
  for ( const auto& k : m_attenuation )     {
    Key key = k.first;
    auto* data = inputs.entry(key);
    if ( auto* m = std::any_cast<DepositMapping>(data) )
      ++cont;
    else if ( auto* v = std::any_cast<DepositVector>(data) )
      cnt++, ++cont;
    count += cnt;
    std::string nam = Key::key_name(key)+":";
    debug("%s+++ %-32s mask:%04X item: %08X Attenuated exponentially %6ld hits by %8.5f",
	  event.id(), nam.c_str(), key.mask(), key.item(), cnt, k.second); 
  }
  info("%s+++ Attenuated exponentially %6ld hits from %4ld containers from segment %s",
       event.id(), count, cont, m_input_segment.c_str());
}

