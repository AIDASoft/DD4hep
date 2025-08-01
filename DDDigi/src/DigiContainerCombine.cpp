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

/// Worker class to combine items of identical types as given by the input definition
/**
 *  This is a utility class.
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_DIGITIZATION
 */
class DigiContainerCombine::work_definition_t  {
public:
  /// reference to parent
  const DigiContainerCombine* combine;
  /// Printout format string
  char        format[128];
  /// Local counters
  std::size_t cnt_conts    = 0;
  std::size_t cnt_depos    = 0;
  std::size_t cnt_parts    = 0;
  std::size_t cnt_hist     = 0;
  std::size_t cnt_response = 0;
  /// Work definition
  std::vector<Key>            keys;
  std::vector<std::any*>      work;
  std::set<Key::itemkey_type> items;
  /// Work done
  std::vector<Key>            used_keys;

  /// Input arguments
  DigiEvent&                  event;
  DataSegment&                inputs;
  DataSegment&                outputs;
  std::mutex&                 used_keys_lock;

  /// Initializing constructor
  work_definition_t(const DigiContainerCombine* c, DigiEvent& ev, DataSegment& in, DataSegment& out, std::mutex& used_lock)
    : combine(c), event(ev), inputs(in), outputs(out), used_keys_lock(used_lock)
  {
    keys.reserve(inputs.size());
    work.reserve(inputs.size());
    for( auto& i : inputs )   {
      Key  key(i.first);
      if ( combine->use_key(key) )   {
        keys.emplace_back(key);
        work.emplace_back(&i.second);
        items.insert(key.item());
      }
    }
    ::snprintf(format, sizeof(format),
               "%s Thread:%%2d+++ %%-32s Out-Mask: $%04X In-Mask: $%%04X Merged %%6ld %%s",
               event.id(), combine->m_deposit_mask);
    format[sizeof(format)-1] = 0;
  }

  void used_keys_insert(Key key)   {
    std::lock_guard<std::mutex> lock(used_keys_lock);
    used_keys.emplace_back(key);
  }

  /// Specialized deposit merger: implicitly assume identical item types are mapped sequentially
  template<typename OUT, typename IN> void merge_depos(OUT& output, IN& input, int thr)  {
    std::size_t cnt = 0;
    const auto& nam = input.name;
    Key::mask_type mask = input.key.mask();
    if ( output.data_type == SegmentEntry::UNKNOWN )
      output.data_type = input.data_type;
    else if ( output.data_type != input.data_type )
      combine->except("+++ Digitization does not allow to mix data of different type!");
    if ( combine->m_erase_combined )   {
      cnt = output.merge(std::move(input));
    }
    else   {
      cnt = output.insert(input);
    }
    combine->info(this->format, thr, nam.c_str(), mask, cnt, "deposits"); 
    this->cnt_depos += cnt;
    this->cnt_conts++;
  }

  /// Generic deposit merger: implicitly assume identical item types are mapped sequentially
  void merge(const std::string& nam, size_t start, int thr)  {
    Key key = keys[start];
    DepositVector out(nam, combine->m_deposit_mask, SegmentEntry::UNKNOWN);
    for( std::size_t j = start; j < keys.size(); ++j )   {
      if ( keys[j].item() == key.item() )   {
        if ( DepositMapping* m = std::any_cast<DepositMapping>(work[j]) )
          merge_depos(out, *m, thr);
        else if ( DepositVector* v = std::any_cast<DepositVector>(work[j]) )
          merge_depos(out, *v, thr);
        else
          break;
        used_keys_insert(keys[j]);
      }
    }
    key.set_mask(combine->m_deposit_mask);
    outputs.emplace(std::move(key), std::move(out));
  }

  /// Merge history records: implicitly assume identical item types are mapped sequentially
  void merge_hist(const std::string& nam, size_t start, int thr)  {
    std::size_t cnt;
    Key key = keys[start];
    DetectorHistory out(nam, combine->m_deposit_mask);
    for( std::size_t j=start; j < keys.size(); ++j )   {
      if ( keys[j].item() == key.item() )   {
        DetectorHistory* next = std::any_cast<DetectorHistory>(work[j]);
        if ( next )   {
          std::string next_name = next->name;
          cnt = (combine->m_erase_combined) ? out.merge(std::move(*next)) : out.insert(*next);
          combine->info(format, thr, next_name.c_str(), keys[j].mask(), cnt, "histories");
          used_keys_insert(keys[j]);
          cnt_hist += cnt;
          cnt_conts++;
        }
      }
    }
    key.set_mask(combine->m_deposit_mask);
    outputs.emplace(std::move(key), std::move(out));
  }

  /// Merge detector rsponse records: implicitly assume identical item types are mapped sequentially
  void merge_response(const std::string& nam, size_t start, int thr)  {
    std::size_t cnt;
    Key key = keys[start];
    DetectorResponse out(nam, combine->m_deposit_mask);
    for( std::size_t j=start; j < keys.size(); ++j )   {
      if ( keys[j].item() == key.item() )   {
        DetectorResponse* next = std::any_cast<DetectorResponse>(work[j]);
        if ( next )   {
          std::string next_name = next->name;
          cnt = (combine->m_erase_combined) ? out.merge(std::move(*next)) : out.insert(*next);
          combine->info(format, thr, next_name.c_str(), keys[j].mask(), cnt, "responses"); 
          used_keys_insert(keys[j]);
          cnt_response += cnt;
          cnt_conts++;
        }
      }
    }
    key.set_mask(combine->m_deposit_mask);
    outputs.emplace(std::move(key), std::move(out));
  }

  /// Merge particle objects: implicitly assume identical item types are mapped sequentially
  void merge_parts(const std::string& nam, size_t start, int thr)  {
    std::size_t cnt;
    Key key = keys[start];
    ParticleMapping out(nam, combine->m_deposit_mask);
    for( std::size_t j=start; j < keys.size(); ++j )   {
      if ( keys[j].item() == key.item() )   {
        ParticleMapping* next = std::any_cast<ParticleMapping>(work[j]);
        if ( next )   {
          std::string next_name = next->name;
          cnt = (combine->m_erase_combined) ? out.merge(std::move(*next)) : out.insert(*next);
          combine->info(format, thr, next_name.c_str(), keys[j].mask(), cnt, "particles"); 
          used_keys_insert(keys[j]);
          cnt_parts += cnt;
          cnt_conts++;
        }
      }
    }
    key.set_mask(combine->m_deposit_mask);
    outputs.emplace(std::move(key), std::move(out));
  }

  /// Merge single item type
  void merge_one(Key::itemkey_type itm, int thr)   {
    const std::string& opt = combine->m_output_name_flag;
    for( std::size_t i=0; i < keys.size(); ++i )   {
      if ( keys[i].item() != itm )
        continue;
      /// Merge deposit mapping
      if ( DepositMapping* depom = std::any_cast<DepositMapping>(work[i]) )   {
        if ( combine->m_merge_deposits  ) merge(depom->name+opt, i, thr);
      }
      /// Merge deposit vector
      else if ( DepositVector* depov = std::any_cast<DepositVector>(work[i]) )   {
        if ( combine->m_merge_deposits  ) merge(depov->name+opt, i, thr);
      }
      /// Merge detector response
      else if ( DetectorResponse* resp = std::any_cast<DetectorResponse>(work[i]) )   {
        if ( combine->m_merge_response  ) merge_response(resp->name+opt, i, thr);
      }
      /// Merge response history
      else if ( DetectorHistory* hist = std::any_cast<DetectorHistory>(work[i]) )   {
        if ( combine->m_merge_history   ) merge_hist(hist->name+opt, i, thr);
      }
      /// Merge particle container
      else if ( ParticleMapping* parts = std::any_cast<ParticleMapping>(work[i]) )   {
        if ( combine->m_merge_particles ) merge_parts(parts->name+opt, i, thr);
      }
      break;
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
  declareProperty("output_mask",      m_deposit_mask);
  declareProperty("output_name_flag", m_output_name_flag);
  declareProperty("erase_combined",   m_erase_combined  = false);
  declareProperty("merge_deposits",   m_merge_deposits  = true);
  declareProperty("merge_response",   m_merge_response  = true);
  declareProperty("merge_history",    m_merge_history   = true);
  declareProperty("merge_particles",  m_merge_particles = false);
  m_kernel.register_initialize(std::bind(&DigiContainerCombine::initialize,this));
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
    m_cont_keys.emplace(key.item());
    if ( m_input_masks.empty() )   {
      m_keys.emplace(key.value());
      continue;
    }
    for ( int m : m_input_masks )    {
      key.set_mask(m);
      m_keys.emplace(key.value());
    }
  }
  if ( !m_output_name_flag.empty() )
    m_output_name_flag += '/';
}

/// Initializing function: compute values which depend on properties
void DigiContainerCombine::have_workers(size_t count)  const   {
  if ( m_workers.size() < count )   {
    auto group = m_workers.get_group(); // Lock worker group
    for(size_t i=m_workers.size(); i <= count; ++i)
      m_workers.insert(new Worker(nullptr, i));
  }
}

/// Decide if a container is to merged based on the properties
bool DigiContainerCombine::use_key(Key key)  const   {
  const auto& m = m_input_masks;
  bool use = m.empty() || m_keys.empty();
  if ( !use )  {
    if ( !m_cont_keys.empty() )  {
      key.set_mask(0);
      return m_cont_keys.find(key.value()) != m_cont_keys.end();
    }
    return std::find(m.begin(), m.end(), key.mask()) != m.end();
  }
  return true;
}

/// Combine selected containers to one single deposit container
std::size_t DigiContainerCombine::combine_containers(DigiContext& context,
                                                     DigiEvent&   event,
                                                     DataSegment& inputs,
                                                     DataSegment& outputs)  const
{
  work_definition_t def(this, event, inputs, outputs, m_used_keys_lock);
  if ( m_parallel )  {
    have_workers(def.items.size());
    m_kernel.submit(context, m_workers.get_group(), def.items.size(), &def);
  }
  else  {
    def.merge_all();
  }
  if ( m_erase_combined )   {
    inputs.erase(def.used_keys);
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
  combine_containers(context, event, inputs, outputs);
}
