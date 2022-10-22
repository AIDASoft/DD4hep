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
#if 0
    template <typename T> T* DigiContainerProcessor::work_t::get_output()   {
      if ( !output.data.has_value() )  {
	T data;
	output.data = std::move(data);
      }
      T* v = std::any_cast<T>(&output.data);
      if ( v )   {
	return v;
      }
      dd4hep::except("DigiContainerProcessor",
		     "+++ Cannot access output. Invalid data handle of type: %s",
		     output_type_name().c_str());
      return nullptr;
    }
#endif

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

#if 0
template       DepositVector*   DigiContainerProcessor::work_t::get_output();
template       DepositMapping*  DigiContainerProcessor::work_t::get_output();
template       ParticleMapping* DigiContainerProcessor::work_t::get_output();

/// output data type
const std::type_info& DigiContainerProcessor::work_t::output_type()  const   {
  return output.data.type();
}

/// String form of the output data type
std::string DigiContainerProcessor::work_t::output_type_name()  const   {
  return typeName(input.data.type());
}

/// Merge output data
void DigiContainerProcessor::work_t::merge_output(DepositVector&& data)   {
  std::lock_guard<std::mutex> lock(output.lock);
  this->get_output<DepositVector>()->merge(std::move(data));
}

void DigiContainerProcessor::work_t::emplace_output(CellID cell, EnergyDeposit&& data)   {
  std::lock_guard<std::mutex> lock(output.lock);
  this->get_output<DepositVector>()->emplace(cell, std::move(data));
}
#endif

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
       context.event->id(), (void*)this, key.key, key.mask(), key.item(), Key::key_name(key).c_str());
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
void DigiContainerSequence::execute(DigiContext& /* context */, work_t& work)  const   {
  m_kernel.submit(m_workers.get_calls(), m_workers.size(), &work, m_parallel);
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
  declareProperty("input_mask",    m_input_mask);
  declareProperty("input_segment", m_input_segment);
  declareProperty("output_mask",      m_output_mask);
  declareProperty("output_segment",   m_output_segment);
  m_kernel.register_initialize(Callback(this).make(&DigiContainerSequenceAction::initialize));
  InstanceCount::increment(this);
}

/// Default destructor
DigiContainerSequenceAction::~DigiContainerSequenceAction() {
  InstanceCount::decrement(this);
}

/// Initialization callback
void dd4hep::digi::DigiContainerSequenceAction::initialize()   {
  size_t count = 0;
  for( auto& ent : m_registered_processors )   {
    Key key = ent.first;
    key.set_mask(m_input_mask);
    worker_t* w = new worker_t(ent.second, count++);
    m_registered_workers.emplace(key, w);
    m_workers.insert(w);
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
  info("+++ Adding processor: %s for container: [%016lX] %s",
       action->c_name(), key.key, container.c_str());
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

/// Main functional callback if specific work is known
void DigiContainerSequenceAction::execute(DigiContext& context)  const   {
  auto& event   = *context.event;
  auto& inputs  = event.get_segment(m_input_segment);
  auto& outputs = event.get_segment(m_output_segment);
  std::vector<ParallelWorker*> event_workers;
  output_t output { m_output_mask, outputs };
  work_t   args   { context, {}, output, this };

  args.input_items.resize(m_workers.size(), {0, nullptr});
  event_workers.reserve(inputs.size());
  for( auto& i : inputs )   {
    Key key(i.first);
    if ( key.mask() == m_input_mask )   {
      auto it = m_registered_workers.find(key);
      if ( it != m_registered_workers.end() )  {
	worker_t* w = it->second;
	event_workers.emplace_back(w);
	args.input_items[w->options] = { key, &(i.second) };
      }
    }
  }
  if ( !event_workers.empty() )   {
    m_kernel.submit(&event_workers.at(0), event_workers.size(), &args, m_parallel);
  }
}

/// Worker adaptor for caller DigiContainerSequenceAction
template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiContainerSequenceAction::work_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* args  = reinterpret_cast<calldata_t*>(data);
  auto& item = args->input_items[this->options];
  DigiContainerProcessor::work_t  work { args->context, { item.key, *item.data }, args->output };
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
  InstanceCount::increment(this);
}

/// Default destructor
DigiMultiContainerProcessor::~DigiMultiContainerProcessor() {
  InstanceCount::decrement(this);
}

void DigiMultiContainerProcessor::adopt_processor(DigiContainerProcessor* action, const std::vector<std::string>& containers)    {
  if ( !action )   {
    except("+++ Attempt to use invalid processor. Request FAILED.");
  }
  else if ( containers.empty() )   {
    except("+++ Processor %s is defined, but no workload was assigned. Request FAILED.");
  }
  std::stringstream str;
  std::vector<Key> keys;
  str << "[ ";
  for(const auto& cont : containers)    {
    Key key(cont, 0x0);
    keys.push_back(key);
    m_work_items.insert(key.item());
    str << cont << " ";
  }
  str << " ";
  m_workers.insert(new worker_t(action, m_workers.size()));
  m_worker_keys.emplace_back(std::move(keys));
  info("+++ Use processor: %-32s for processing: %s", action->c_name(), str.str().c_str());
}

/// Main functional callback
void DigiMultiContainerProcessor::execute(DigiContext& context)  const  {
  work_items_t work_items;
  auto& msk    = m_input_masks;
  auto& event  = *context.event;
  auto& inputs = event.get_segment(m_input_segment);

  work_items.reserve(inputs.size());
  for( auto& i : inputs )   {
    Key in_key(i.first);
    bool use = msk.empty() || std::find(msk.begin(), msk.end(), in_key.mask()) != msk.end();
    if ( use )   {
      use = m_work_items.empty() || m_work_items.find(in_key.item()) != m_work_items.end();
      if ( use )   {
	work_items.emplace_back(std::make_pair(i.first, &i.second));
      }
    }
  }
  if ( !work_items.empty() )   {
    auto&    outputs = event.get_segment(m_output_segment);
    output_t output { m_output_mask, outputs };
    work_t   args   { context, work_items, output, *this };
    m_kernel.submit(m_workers.get_calls(), m_workers.size(), &args, m_parallel);
  }
}

/// Worker adaptor for caller DigiMultiContainerProcessor
template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiMultiContainerProcessor::work_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* arg   = reinterpret_cast<calldata_t*>(data);
  const auto& keys  = arg->parent.worker_keys(this->options);
  const auto& masks = arg->parent.input_masks();
  for( const auto& item : arg->items )  {
    Key key = item.first;
    if ( masks.empty() || std::find(masks.begin(), masks.end(), key.mask()) != masks.end() )  {
      if ( keys.empty() )  {
	DigiContainerProcessor::work_t  work {arg->context, {key, *item.second }, arg->output };
	action->execute(work.context, work);
      }
      else if ( std::find(keys.begin(), keys.end(), Key(key.item())) != keys.end() )    {
	DigiContainerProcessor::work_t  work {arg->context, {key, *item.second }, arg->output };
	action->execute(work.context, work);
      }
    }
  }
}

