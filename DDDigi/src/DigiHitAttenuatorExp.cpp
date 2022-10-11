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
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiHitAttenuatorExp.h>

class dd4hep::digi::DigiHitAttenuatorExp::internals_t   {
public:
  /// Property: Input data segment name
  std::string                    input  { };
  /// Property: Container names to be loaded
  std::map<std::string, double>  container_attenuation  { };
  /// Property: event masks to be handled
  std::vector<int>               masks  { };
  /// Property: T0 with respect to central crossing
  double                         t0     { 0e0 };

  /// Keys of all containers to be manipulated
  std::map<unsigned long, double> attenuation  { };

  void initialize(DigiContext& context)   {
    if ( this->attenuation.empty() )   {
      std::lock_guard<std::mutex> lock(context.initializer_lock());
      if ( this->attenuation.empty() )   {
	for ( const auto& c : this->container_attenuation )   {
	  Key key(0x0, c.first);
	  for ( int m : this->masks )    {
	    double factor = std::exp(-1e0 * this->t0/c.second);
	    key.values.mask = m;
	    this->attenuation.emplace(key.key, factor);
	  }
	}
      }
    }
  }
};

/// Standard constructor
dd4hep::digi::DigiHitAttenuatorExp::DigiHitAttenuatorExp(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  internals = std::make_unique<internals_t>();
  declareProperty("input",      internals->input = "inputs");
  declareProperty("containers", internals->container_attenuation);
  declareProperty("masks",      internals->masks);
  declareProperty("t0",         internals->t0);
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiHitAttenuatorExp::~DigiHitAttenuatorExp() {
  InstanceCount::decrement(this);
}

/// Main functional callback
void dd4hep::digi::DigiHitAttenuatorExp::execute(DigiContext& context)  const    {
  internals->initialize(context);
  std::size_t count = 0;
  auto& event  = context.event;
  auto& inputs = event->get_segment(internals->input);
  for ( const auto& k : internals->attenuation )     {
    auto iter = inputs.find(k.first);
    if ( iter != inputs.end() )   {
      std::any& obj = (*iter).second;
      DepositMapping* m = std::any_cast<DepositMapping>(&obj);
      double factor = k.second;
      if ( m )    {
	for( auto& c : *m )    {
	  c.second.deposit *= factor;
	}
	count += m->size();
	std::string nam = Key::key_name(k.first)+":";
	debug("%s+++ %-32s Mask: %06X Attenuated exponentially %6ld hits by %8.5f",
	      event->id(), nam.c_str(), m->mask, m->size(), factor); 
	continue;
      }
      error("Invalid data type in container");
    }
  }
  info("%s+++ Attenuated exponentially %6ld hits", event->id(), count);
}

