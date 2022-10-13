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
#include <DDDigi/DigiContainerCombine.h>

/// C/C++ include files
#include <set>

using dd4hep::digi::DigiContainerCombine;

class DigiContainerCombine::internals_t   {
public:
  /// Property: Container names to be loaded
  std::vector<std::string>       containers { };
  /// Property: event masks to be handled
  std::vector<int>               masks  { };
  /// Property: Output container dressing
  std::string                    output_name_flag;
  /// Property: Input data segment name
  std::string                    input;
  /// Property: Output data segment name
  std::string                    output;
  /// Property: mask of the deposit
  int                            deposit_mask { 0 };
  /// Property: Flag to erase already combined containers (not thread-safe!!!)
  bool                           erase_combined { false };

  /// Fully qualified keys of all containers to be manipulated
  std::set<Key::key_type>        keys  { };
  /// Container keys of all containers to be manipulated
  std::set<Key::key_type>        cont_keys  { };
  /// Predicate function to select containers for merging
  std::function<bool(Key::key_type)>  container_selector;
  /// Flag to check the initialization
  bool inited  { false };
  /// Initializing function: compute values which depend on properties
  void initialize(DigiContext& context)   {
    if ( !this->inited )   {
      std::lock_guard<std::mutex> lock(context.initializer_lock());
      if ( !this->inited )   {
	this->inited = true;

	this->container_selector = [this](Key::key_type k)  {
	  const auto& m = this->masks;
	  bool use = m.empty() || this->keys.empty();
	  if ( !use )  {
	    use = std::find(m.begin(), m.end(), Key::mask(k)) != m.end();
	    if ( !use )   {
	      return this->cont_keys.find(Key::item(k)) != this->cont_keys.end();
	    }
	  }
	  return true;
	};
	for ( const auto& c : this->containers )   {
	  Key key(0x0, c);
	  this->cont_keys.emplace(key.key);
	  if ( this->masks.empty() )   {
	    this->keys.emplace(key.key);
	    continue;
	  }
	  for ( int m : this->masks )    {
	    key.values.mask = m;
	    this->keys.emplace(key.key);
	  }
	}
      }
    }
  }
};

/// Standard constructor
dd4hep::digi::DigiContainerCombine::DigiContainerCombine(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->internals = std::make_unique<internals_t>();
  this->declareProperty("containers",       this->internals->containers);
  this->declareProperty("masks",            this->internals->masks);
  this->declareProperty("input_segment",    this->internals->input  = "inputs");
  this->declareProperty("output_segment",   this->internals->output = "deposits");
  this->declareProperty("deposit_mask",     this->internals->deposit_mask);
  this->declareProperty("output_name_flag", this->internals->output_name_flag);
  this->declareProperty("erase_combined",   this->internals->erase_combined);
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiContainerCombine::~DigiContainerCombine() {
  InstanceCount::decrement(this);
}

/// Combine selected containers to one single deposit container
template <typename PREDICATE> std::size_t 
dd4hep::digi::DigiContainerCombine::combine_containers(DigiEvent& event,
						       DataSegment& inputs,
						       DataSegment& outputs,
						       const PREDICATE& predicate)  const
{
  std::size_t cnt_depos = 0;
  std::size_t cnt_parts = 0;
  std::size_t cnt_conts = 0;
  std::vector<Key::key_type> to_erase;

  for( auto& i : inputs )   {
    if ( predicate(i.first) )   {
      Key depo_key(i.first);
      Key input_key(i.first);
      DepositMapping* din = std::any_cast<DepositMapping>(&i.second);
      if ( din )   {
	depo_key.values.mask = internals->deposit_mask;
	cnt_depos += din->size();
	cnt_conts++;
	auto iter = outputs.find(depo_key.key);
	if ( iter == outputs.end() )   {
	  this->info("%s+++ %-32s Mask: %06X Input: %06X Merged %6ld deposits",
		     event.id(), din->name.c_str(), depo_key.values.mask, 
		     input_key.values.mask, din->size()); 
	  if ( !internals->output_name_flag.empty() )   {
	    din->name = din->name+"/"+internals->output_name_flag;
	  }
	  outputs.emplace(depo_key.key, std::move(i.second));
	  to_erase.emplace_back(i.first);
	  continue;
	}
	DepositMapping* out = std::any_cast<DepositMapping>(&iter->second);
	std::size_t cnt = out->merge(std::move(*din));
	this->info("%s+++ %-32s Mask: %06X Input: %06X Merged %6ld deposits",
		   event.id(), out->name.c_str(), depo_key.values.mask, 
		   input_key.values.mask, cnt); 
	i.second.reset();
	to_erase.emplace_back(i.first);
	continue;
      }
      ParticleMapping* pin = std::any_cast<ParticleMapping>(&i.second);
      if ( pin )   {
	depo_key.values.mask = internals->deposit_mask;
	cnt_parts += pin->size();
	cnt_conts++;
	auto iter = outputs.find(depo_key.key);
	if ( iter == outputs.end() )   {
	  this->info("%s+++ %-32s Mask: %06X Input: %06X Merged %6ld particles",
		     event.id(), pin->name.c_str(), depo_key.values.mask, 
		     input_key.values.mask, pin->size()); 
	  if ( !internals->output_name_flag.empty() )   {
	    pin->name = pin->name+"/"+internals->output_name_flag;
	  }
	  outputs.emplace(depo_key.key, std::move(i.second));
	  to_erase.emplace_back(i.first);
	  continue;
	}
	ParticleMapping* out = std::any_cast<ParticleMapping>(&iter->second);
	std::size_t cnt = out->merge(std::move(*pin));
	this->info("%s+++ %-32s Mask: %06X Input: %06X Merged %6ld particles",
		   event.id(), out->name.c_str(), depo_key.values.mask, 
		   input_key.values.mask, cnt); 
	i.second.reset();
	to_erase.emplace_back(i.first);
	continue;
      }      
    }
  }
  if ( this->internals->erase_combined )   {
    inputs.erase(to_erase);
  }
  this->info("%s+++ Merged %ld particles and %ld deposits from %ld containers",
	     event.id(), cnt_parts, cnt_depos, cnt_conts);
  return cnt_depos;
}

/// Main functional callback
void dd4hep::digi::DigiContainerCombine::execute(DigiContext& context)  const    {
  this->internals->initialize(context);
  auto& event    = *context.event;
  auto& inputs   = event.get_segment(this->internals->input);
  auto& outputs  = event.get_segment(this->internals->output);
  auto& selector = this->internals->container_selector;
  this->combine_containers(event, inputs, outputs, selector);
}
