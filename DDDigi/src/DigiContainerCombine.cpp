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

	this->container_selector = [this](Key::key_type k)  {
	  const auto& m = this->masks;
	  bool use = m.empty() || this->keys.empty();
	  if ( !use )  {
	    if ( !this->cont_keys.empty() )
	      return this->cont_keys.find(Key::item(k)) != this->cont_keys.end();
	    return std::find(m.begin(), m.end(), Key::mask(k)) != m.end();
	  }
	  return true;
	};
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
dd4hep::digi::DigiContainerCombine::combine_containers(DigiEvent&   event,
						       DataSegment& inputs,
						       DataSegment& outputs,
						       const PREDICATE& predicate)  const
{
  std::size_t cnt_depos = 0;
  std::size_t cnt_parts = 0;
  std::size_t cnt_conts = 0;
  std::vector<Key> keys;
  std::vector<std::any*> work;
  std::set<Key::itemkey_type> items;

  keys.reserve(inputs.size());
  work.reserve(inputs.size());
  for( auto& i : inputs )   {
    if ( predicate(i.first) )   {
      keys.emplace_back(i.first);
      work.emplace_back(&i.second);
      items.insert(Key(i.first).values.item);
    }
  }
  Key depo_key;
  depo_key.values.mask = internals->deposit_mask;
  for(auto itm : items)   {
    for( std::size_t i=0; i < keys.size(); ++i )   {
      if ( keys[i].values.item != itm )
	continue;
      auto* output = work[i];
      depo_key.values.item = itm;

      /// Merge deposit mapping
      if ( DepositMapping* depos = std::any_cast<DepositMapping>(output) )   {
	if ( !internals->output_name_flag.empty() )
	  depos->name = depos->name+"/"+internals->output_name_flag;
	cnt_depos += depos->size();
	cnt_conts++;
	this->info("%s+++ %-32s Mask: $%04X Input: $%04X Merged %6ld deposits",
		   event.id(), depos->name.c_str(), depo_key.values.mask, 
		   keys[i].values.mask, depos->size()); 
	for( std::size_t j=i+1; j < keys.size(); ++j )   {
	  if ( keys[j].values.item == itm )   {
	    DepositMapping* next = std::any_cast<DepositMapping>(work[j]);
	    cnt_depos += next->size();
	    cnt_conts++;
	    std::size_t cnt = depos->merge(std::move(*next));
	    this->info("%s+++ %-32s Mask: $%04X Input: $%04X Merged %6ld deposits",
		       event.id(), depos->name.c_str(), depo_key.values.mask, 
		       keys[j].values.mask, cnt); 
	    cnt_depos += cnt;
	    cnt_conts++;
	    work[j]->reset();
	  }
	}
	outputs.emplace(depo_key, std::move(*output));
	break;
      }
      else if ( ParticleMapping* parts = std::any_cast<ParticleMapping>(output) )   {
	if ( !internals->output_name_flag.empty() )
	  parts->name = parts->name+"/"+internals->output_name_flag;
	cnt_parts += parts->size();
	cnt_conts++;

	this->info("%s+++ %-32s Mask: $%04X Input: $%04X Merged %6ld particles",
		   event.id(), parts->name.c_str(), depo_key.values.mask, 
		   keys[i].values.mask, parts->size()); 
	for( std::size_t j=i+1; j < keys.size(); ++j )   {
	  if ( keys[j].values.item == itm )   {
	    ParticleMapping* next = std::any_cast<ParticleMapping>(work[j]);
	    std::size_t cnt = parts->merge(std::move(*next));
	    this->info("%s+++ %-32s Mask: $%04X Input: $%04X Merged %6ld particles",
		       event.id(), parts->name.c_str(), depo_key.values.mask, 
		       keys[j].values.mask, cnt); 
	    cnt_parts += cnt;
	    cnt_conts++;
	    work[j]->reset();
	  }
	}
	outputs.emplace(depo_key, std::move(*output));
	break;
      }
    }
  }
  if ( this->internals->erase_combined )   {
    inputs.erase(keys);
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
