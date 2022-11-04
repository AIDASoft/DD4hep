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
#include <DDDigi/DigiContainerProcessor.h>

/// C/C++ include files
#include <sstream>

using namespace dd4hep::digi;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <typename T> T* DigiContainerProcessor::work_t::get_input(bool exc)   {
      if ( input.data.has_value() )  {
	T* object = std::any_cast<T>(&input.data);
	if ( object )   {
	  return object;
	}
      }
      if ( exc )   {
	dd4hep::except("DigiContainerProcessor",
		       "+++ Cannot access input %s. Invalid data handle of type: %s",
		       Key::key_name(input.key).c_str(), input_type_name().c_str());
      }
      return nullptr;
    }

    template <typename T> const T* DigiContainerProcessor::work_t::get_input(bool exc)  const  {
      if ( input.data.has_value() )  {
	const T* object = std::any_cast<T>(&input.data);
	if ( object )   {
	  return object;
	}
      }
      if ( exc )   {
	dd4hep::except("DigiContainerProcessor",
		       "+++ Cannot access input %s. Invalid data handle of type: %s",
		       Key::key_name(input.key).c_str(), input_type_name().c_str());
      }
      return nullptr;
    }
  }    // End namespace digi
}      // End namespace dd4hep

template       DepositVector*   DigiContainerProcessor::work_t::get_input(bool exc);
template const DepositVector*   DigiContainerProcessor::work_t::get_input(bool exc)  const;
template       DepositMapping*  DigiContainerProcessor::work_t::get_input(bool exc);
template const DepositMapping*  DigiContainerProcessor::work_t::get_input(bool exc)  const;
template       ParticleMapping* DigiContainerProcessor::work_t::get_input(bool exc);
template const ParticleMapping* DigiContainerProcessor::work_t::get_input(bool exc)  const;

/// input data type
const std::type_info& DigiContainerProcessor::work_t::input_type()  const   {
  return input.data.type();
}

/// String form of the input data type
std::string DigiContainerProcessor::work_t::input_type_name()  const   {
  return typeName(input.data.type());
}

/// Standard constructor
DigiContainerProcessor::DigiContainerProcessor(const DigiKernel& kernel, const std::string& name)   
  : DigiAction(kernel, name)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiContainerProcessor::~DigiContainerProcessor() {
  InstanceCount::decrement(this);
}

/// Main functional callback 
void DigiContainerProcessor::execute(DigiContext& context, work_t& work)  const  {
  this->execute(context, work.input.key, work.input.data);
}

/// Main functional callback if specific work is known
void DigiContainerProcessor::execute(DigiContext& context, Key key, std::any& /* data */)  const    {
  info("%s+++ %p Using container: %016lX  --> %04X %08X %s",
       context.event->id(), (void*)this, key.value(), key.mask(), key.item(), Key::key_name(key).c_str());
}

/// Standard constructor
DigiContainerSequence::DigiContainerSequence(const DigiKernel& krnl, const std::string& nam)
  : DigiContainerProcessor(krnl, nam)
{
  declareProperty("parallel", m_parallel = false);
  InstanceCount::increment(this);
}

/// Default destructor
DigiContainerSequence::~DigiContainerSequence() {
  InstanceCount::decrement(this);
}

/// Adopt new parallel worker
void DigiContainerSequence::adopt_processor(DigiContainerProcessor* action)   {
  if ( !action )  {
    except("+++ FAILED: attempt to add invalid processor!");
    return;
  }
  m_workers.insert(new worker_t(action, m_workers.size()));
}

/// Main functional callback if specific work is known
void DigiContainerSequence::execute(DigiContext& context, work_t& work)  const   {
  m_kernel.submit(context, m_workers.get_group(), m_workers.size(), &work, m_parallel);
}

/// Worker adaptor for caller DigiContainerSequence
template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiContainerSequence::work_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* args  = reinterpret_cast<calldata_t*>(data);
  action->execute(args->context, *args);
}

/// Standard constructor
DigiContainerSequenceAction::DigiContainerSequenceAction(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("input_mask",     m_input_mask);
  declareProperty("input_segment",  m_input_segment);
  declareProperty("output_mask",    m_output_mask);
  declareProperty("output_segment", m_output_segment);
  m_kernel.register_initialize(Callback(this).make(&DigiContainerSequenceAction::initialize));
  InstanceCount::increment(this);
}

/// Default destructor
DigiContainerSequenceAction::~DigiContainerSequenceAction() {
  InstanceCount::decrement(this);
}

/// Initialization callback
void dd4hep::digi::DigiContainerSequenceAction::initialize()   {
  for( auto& ent : m_registered_processors )   {
    worker_t* w = new worker_t(ent.second, m_registered_workers.size());
    m_registered_workers.emplace(ent.first, w);
    m_workers.insert(w);
    ent.second->release();
  }
}

/// Adopt new parallel worker
void DigiContainerSequenceAction::adopt_processor(DigiContainerProcessor* action,
						  const std::string& container)
{
  Key key(container, 0x0);
  auto it = m_registered_processors.find(key);
  if ( it != m_registered_processors.end() )   {
    if ( action != it->second )   {
      except("+++ The action %s was already registered to mask:%04X container:%s!",
	     action->c_name(), m_input_mask, container.c_str());
    }
    else   {
      warning("+++ The action %s was already registered to mask:%04X container:%s!",
	      action->c_name(), m_input_mask, container.c_str());
    }
    return;
  }
  action->addRef();
  m_registered_processors.emplace(key, action);
  info("+++ Adding processor: %s for container: [%08X] %s",
       action->c_name(), key.item(), container.c_str());
}

/// Adopt new parallel worker acting on multiple containers
void DigiContainerSequenceAction::adopt_processor(DigiContainerProcessor* action,
						  const std::vector<std::string>& containers)
{
  info("+++ Adding bulk processor: %s for %ld container", action->c_name(), containers.size());
  for( const auto& cont : containers )   {
    adopt_processor(action, cont);
  }
}

/// Get hold of the registered processor for a given container
DigiContainerSequenceAction::worker_t*
DigiContainerSequenceAction::need_registered_worker(Key item_key, bool exc)   const  {
  item_key.set_mask(0);
  auto it = m_registered_workers.find(item_key);
  if ( it != m_registered_workers.end() )  {
    return it->second;
  }
  if ( exc )   {
    except("No worker registered for input: %08X", item_key.item());
  }
  return nullptr;
}

/// Main functional callback if specific work is known
void DigiContainerSequenceAction::execute(DigiContext& context)  const   {
  auto& event   = *context.event;
  auto& inputs  = event.get_segment(m_input_segment);
  auto& outputs = event.get_segment(m_output_segment);
  std::vector<ParallelWorker*> event_workers;
  output_t output { m_output_mask, outputs };
  work_t   args   { context, { }, output, m_properties, *this };

  args.input_items.resize(m_workers.size(), { { }, nullptr });
  event_workers.reserve(inputs.size());
  for( auto& i : inputs )   {
    Key key(i.first);
    if ( key.mask() == m_input_mask )   {
      if ( worker_t* w = need_registered_worker(key, false) )  {
	event_workers.emplace_back(w);
	args.input_items[w->options] = { key, &i.second };
      }
    }
  }
  if ( !event_workers.empty() )   {
    m_kernel.submit(context, &event_workers.at(0), event_workers.size(), &args, m_parallel);
  }
}

/// Worker adaptor for caller DigiContainerSequenceAction
template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiContainerSequenceAction::work_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* args  = reinterpret_cast<calldata_t*>(data);
  auto& item = args->input_items[this->options];
  DigiContainerProcessor::work_t work {args->context, {item.key, *item.data}, args->output, args->properties};
  action->execute(args->context, work);
}

/// Standard constructor
DigiMultiContainerProcessor::DigiMultiContainerProcessor(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->declareProperty("input_masks",      m_input_masks);
  this->declareProperty("input_segment",    m_input_segment);
  this->declareProperty("output_mask",      m_output_mask);
  this->declareProperty("output_segment",   m_output_segment);
  m_kernel.register_initialize(Callback(this).make(&DigiMultiContainerProcessor::initialize));
  InstanceCount::increment(this);
}

/// Default destructor
DigiMultiContainerProcessor::~DigiMultiContainerProcessor() {
  InstanceCount::decrement(this);
}

/// Initialize action object
void DigiMultiContainerProcessor::initialize()   {
  std::map<processor_t*,worker_t*> action_workers;
  std::map<processor_t*,std::vector<Key> > keys;
  std::map<processor_t*,std::vector<std::string> > action_containers;

  m_worker_keys.clear();
  m_worker_map.clear();
  m_work_items.clear();
  m_workers.clear();
  for( auto& proc : m_processors )    {
    Key key(proc.first, 0x0);
    m_work_items.insert(key);
    for ( auto* action : proc.second )   {
      keys[action].push_back(key);
      action_containers[action].push_back(proc.first);
    }
  }
  /// We need to preserve the order the actions were submitted
  /// and remove later added duplicates (for different containers)
  for( auto* action : m_actions )   {
    auto worker_keys = keys[action];
    worker_t* w = nullptr;
    auto iw = action_workers.find(action);
    if ( iw != action_workers.end() )   {
      w = iw->second;
    }
    else   {
      w = new worker_t(action, m_workers.size());
      m_worker_keys.emplace_back(worker_keys);
      m_workers.insert(w);
      action_workers[action] = w;
    }
    for( const auto& key : worker_keys )
      m_worker_map[key.item()].push_back(w);
  }
  /// Add some printout about the configuration
  for(auto* action : m_actions )   {
    auto conts = action_containers[action];
    std::stringstream str;
    str << "[ ";
    for( const auto& cont : conts )
      str << cont << " ";
    str << "]";
    info("+++ Use processor: %-32s for processing: %s", action->c_name(), str.str().c_str());
  }
  /// All done: release reference count aquired in adopt_processor
  for(auto* action : m_actions )
    action->release();
}

/// Adopt new parallel worker
void DigiMultiContainerProcessor::adopt_processor(DigiContainerProcessor* action, const std::vector<std::string>& containers)    {
  if ( !action )   {
    except("+++ Attempt to use invalid processor. Request FAILED.");
  }
  else if ( containers.empty() )   {
    except("+++ Processor %s is defined, but no workload was assigned. Request FAILED.");
  }
  for(const auto& cont : containers)    {
    m_processors[cont].push_back(action);
  }
  if ( std::find(m_actions.begin(), m_actions.end(), action) == m_actions.end() )   {
    m_actions.emplace_back(action);
    action->addRef();
  }
}

/// Main functional callback
void DigiMultiContainerProcessor::execute(DigiContext& context)  const  {
  work_items_t work_items;
  auto& msk    = m_input_masks;
  auto& event  = *context.event;
  auto& inputs = event.get_segment(m_input_segment);

  work_items.reserve(inputs.size());
  for( auto& i : inputs )   {
    Key  key(i.first);
    key.set_mask(0);
    bool use = msk.empty() || std::find(msk.begin(), msk.end(), key.mask()) != msk.end();
    if ( use )   {
      use = m_work_items.empty() || m_work_items.find(key) != m_work_items.end();
      if ( use )   {
	work_items.emplace_back(std::make_pair(i.first, &i.second));
      }
    }
  }
  if ( !work_items.empty() )   {
    auto& outputs = event.get_segment(m_output_segment);
    output_t   output { m_output_mask, outputs };
    work_t     args   { context, work_items, output, properties(), *this };
    m_kernel.submit(context, m_workers.get_group(), m_workers.size(), &args, m_parallel);
  }
}

/// Worker adaptor for caller DigiMultiContainerProcessor
template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiMultiContainerProcessor::work_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* arg   = reinterpret_cast<calldata_t*>(data);
  const auto& par   = arg->parent;
  const auto& keys  = par.worker_keys(this->options);
  const auto& masks = par.input_masks();
  for( const auto& item : arg->items )  {
    Key key(item.first);
    key.set_mask(0);
    const char* tag = "";
    if ( masks.empty() || std::find(masks.begin(), masks.end(), key.mask()) != masks.end() )  {
      tag = "mask accepted";
      if ( keys.empty() )  {
	DigiContainerProcessor::work_t  work {arg->context, {key, *item.second }, arg->output, arg->properties };
	action->execute(work.context, work);
	continue;
      }
      else if ( std::find(keys.begin(), keys.end(), key) != keys.end() )    {
	DigiContainerProcessor::work_t  work {arg->context, {key, *item.second }, arg->output, arg->properties };
	action->execute(work.context, work);
	continue;
      }
      tag = "no keys matching";
    }
    if ( tag )  {}
#if 0
    par.info("%s+++ Ignore container: %016lX  --> %04X %08X %s [%s]",
	     arg->context.event->id(), key.value(), key.mask(), key.item(), Key::key_name(key).c_str(), tag);
#endif
  }
}

