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
#include <DDDigi/DigiContainerCombine.h>

/// C/C++ include files
#include <set>

using namespace dd4hep::digi;

class DigiContainerCombine::work_definition_t  {
public:
  const DigiContainerCombine* combine;
  std::size_t cnt_conts = 0;
  std::size_t cnt_depos = 0;
  std::size_t cnt_parts = 0;
  /// Printout format string
  char        format[128];

  std::vector<Key>            keys;
  std::vector<std::any*>      work;
  std::set<Key::itemkey_type> items;

  DigiEvent&                  event;
  DataSegment&                inputs;
  DataSegment&                outputs;

  /// Initializing constructor
  work_definition_t(const DigiContainerCombine* c, DigiEvent& ev, DataSegment& in, DataSegment& out)
    : combine(c), event(ev), inputs(in), outputs(out)
  {
    keys.reserve(inputs.size());
    work.reserve(inputs.size());
    for( auto& i : inputs )   {
      Key  key = i.first;
      if ( combine->use_key(key) )   {
	keys.emplace_back(key);
	work.emplace_back(&i.second);
	items.insert(key.item());
      }
    }
    ::snprintf(format, sizeof(format),
	       "%s+++%%2d++ %%-32s Mask: $%04X Input: $%%04X Merged %%6ld %%s",
	       event.id(), combine->m_deposit_mask);
    format[sizeof(format)-1] = 0;
  }

  template<typename T> void merge_depos(DepositMapping& output, T& input, int thr)  {
    std::size_t cnt = 0;
    if ( combine->m_erase_combined )
      cnt = output.merge(std::move(input));
    else
      cnt = output.insert(input);
    combine->info(this->format, thr, input.name.c_str(), input.key.values.mask, cnt, "deposits"); 
    this->cnt_depos += cnt;
    this->cnt_conts++;
  }

  void merge(const std::string& nam, size_t start, int thr)  {
    Key key = keys[start];
    DepositMapping out(nam, combine->m_deposit_mask);
    for( std::size_t j = start; j < keys.size(); ++j )   {
      if ( keys[j].item() == key.item() )   {
	if ( DepositMapping* m = std::any_cast<DepositMapping>(work[j]) )
	  merge_depos(out, *m, thr);
	else if ( DepositVector* v = std::any_cast<DepositVector>(work[j]) )
	  merge_depos(out, *v, thr);
	else 
	  continue;
	this->work[j]->reset();
      }
    }
    key.set_mask(combine->m_deposit_mask);
    outputs.emplace(key, std::move(out));
  }

  void merge_parts(const std::string& nam, size_t start, int thr)  {
    Key key = keys[start];
    ParticleMapping out(nam, combine->m_deposit_mask);
    for( std::size_t j=start; j < keys.size(); ++j )   {
      if ( keys[j].item() == key.item() )   {
	ParticleMapping* next = std::any_cast<ParticleMapping>(work[j]);
	std::size_t cnt = out.merge(std::move(*next));
	combine->info(format, thr, next->name.c_str(), keys[j].mask(), cnt, "particles"); 
	cnt_parts += cnt;
	cnt_conts++;
	work[j]->reset();
      }
    }
    key.set_mask(combine->m_deposit_mask);
    outputs.emplace(key, std::move(out));
  }

  void merge_one(Key::itemkey_type itm, int thr)   {
    const std::string& opt = combine->m_output_name_flag;
    for( std::size_t i=0; i < keys.size(); ++i )   {
      if ( keys[i].values.item != itm )
	continue;
      /// Merge deposit mapping
      if ( DepositMapping* depom = std::any_cast<DepositMapping>(work[i]) )   {
	merge(depom->name+opt, i, thr);
	break;
      }
      /// Merge deposit vector
      else if ( DepositVector* depov = std::any_cast<DepositVector>(work[i]) )   {
	merge(depov->name+opt, i, thr);
	break;
      }
      /// Merge particle container
      else if ( ParticleMapping* parts = std::any_cast<ParticleMapping>(work[i]) )   {
	merge_parts(parts->name+opt, i, thr);
	break;
      }
    }
  }

  void merge_all()   {
    for( auto itm : items )
      merge_one(itm, 0);
  }
};

template <> void DigiParallelWorker<DigiContainerCombine,
				    DigiContainerCombine::work_definition_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  std::size_t cnt = 0;
  for( auto itm : args->items )  {
    if ( cnt == this->options )   {
      args->merge_one(itm, this->options);
      return;
    }
    ++cnt;
  }
}

/// Standard constructor
DigiContainerCombine::DigiContainerCombine(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("containers",       m_containers);
  declareProperty("input_masks",      m_input_masks);
  declareProperty("input_segment",    m_input  = "inputs");
  declareProperty("output_segment",   m_output = "deposits");
  declareProperty("deposit_mask",     m_deposit_mask);
  declareProperty("output_name_flag", m_output_name_flag);
  declareProperty("erase_combined",   m_erase_combined);
  m_kernel.register_initialize(Callback(this).make(&DigiContainerCombine::initialize));
  InstanceCount::increment(this);
}

/// Default destructor
DigiContainerCombine::~DigiContainerCombine() {
  InstanceCount::decrement(this);
}

/// Initializing function: compute values which depend on properties
void DigiContainerCombine::initialize()    {
  for ( const auto& cont : m_containers )   {
    Key key(cont, 0x0);
    m_cont_keys.emplace(key.key);
    if ( m_input_masks.empty() )   {
      m_keys.emplace(key.key);
      continue;
    }
    for ( int m : m_input_masks )    {
      key.values.mask = m;
      m_keys.emplace(key.key);
    }
  }
  if ( !m_output_name_flag.empty() )
    m_output_name_flag += '/';
}

/// Initializing function: compute values which depend on properties
void DigiContainerCombine::have_workers(size_t count)  const   {
  if ( m_workers.size() < count )   {
    auto lock = m_workers.can_modify();
    for(size_t i=m_workers.size(); i <= count; ++i)
      m_workers.insert(new Worker(nullptr, i));
  }
}

/// Decide if a continer is to merged based on the properties
bool DigiContainerCombine::use_key(Key key)  const   {
  const auto& m = m_input_masks;
  bool use = m.empty() || m_keys.empty();
  if ( !use )  {
    if ( !m_cont_keys.empty() )
      return m_cont_keys.find(key.item()) != m_cont_keys.end();
    return std::find(m.begin(), m.end(), key.mask()) != m.end();
  }
  return true;
}

/// Combine selected containers to one single deposit container
std::size_t DigiContainerCombine::combine_containers(DigiEvent&   event,
						     DataSegment& inputs,
						     DataSegment& outputs)  const
{
  work_definition_t def(this, event, inputs, outputs);
  if ( m_parallel )  {
    have_workers(def.items.size());
    m_kernel.submit(m_workers.get_group(), def.items.size(), &def);
  }
  else  {
    def.merge_all();
  }
  if ( m_erase_combined )   {
    inputs.erase(def.keys);
  }
  info("%s+++ Merged %ld particles and %ld deposits from segment '%s' to segment '%s'",
       event.id(), def.cnt_parts, def.cnt_depos, m_input.c_str(), m_output.c_str());
  return def.cnt_depos;
}

/// Main functional callback
void DigiContainerCombine::execute(DigiContext& context)  const    {
  auto& event    = *context.event;
  auto& inputs   = event.get_segment(m_input);
  auto& outputs  = event.get_segment(m_output);
  combine_containers(event, inputs, outputs);
}
