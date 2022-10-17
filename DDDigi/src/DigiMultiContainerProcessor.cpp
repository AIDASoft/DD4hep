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
#include <DDDigi/DigiMultiContainerProcessor.h>

/// C/C++ include files
#include <set>
#include <sstream>

using namespace dd4hep::digi;

template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiMultiContainerProcessor::CallData,
				    int>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  action->execute(args->context, args->work);
}

/// Standard constructor
DigiMultiContainerProcessor::DigiMultiContainerProcessor(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->declareProperty("input_masks",      m_input_masks);
  this->declareProperty("input_segment",    m_input_segment);
  InstanceCount::increment(this);
}

/// Default destructor
DigiMultiContainerProcessor::~DigiMultiContainerProcessor() {
  for(auto* w : m_workers ) detail::deletePtr(w);
  m_workers.clear();
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
  for(const auto& c : containers)    {
    Key key(0x0, c);
    keys.push_back(key);
    m_work_items.insert(key.item());
    str << c << " ";
  }
  action->m_container_keys = std::move(keys);
  m_workers.emplace_back(new Worker(action, 0));
  info("+++ Use processor: %-32s for processing: %s", action->c_name(), str.str().c_str());
}

/// Main functional callback
void DigiMultiContainerProcessor::execute(DigiContext& context)  const    {
  WorkItems work_items;
  auto& msk    = m_input_masks;
  auto& event  = *context.event;
  auto& inputs = event.get_segment(m_input_segment);

  work_items.reserve(inputs.size());
  for( auto& i : inputs )   {
    Key in_key(i.first);
    bool use = msk.empty() || std::find(msk.begin(), msk.end(), in_key.mask()) != msk.end();
    if ( use )   {
      use = m_work_items.find(in_key.item()) != m_work_items.end();
      if ( use )   {
	work_items.emplace_back(std::make_pair(i.first, &i.second));
      }
    }
  }
  if ( !work_items.empty() )   {
    CallData data { context, work_items };
    m_kernel.submit(m_workers, &data);
  }
}

/// Standard constructor
DigiContainerProcessor::DigiContainerProcessor(const DigiKernel& kernel, const std::string& name)   
  : DigiAction(kernel, name)
{
  this->declareProperty("input_masks",      m_input_masks);
  this->declareProperty("input_segment",    m_input_segment);
}

/// Check if the work item is for us
bool DigiContainerProcessor::use_container(Key key)   const    {
  auto key_iter = std::find(m_container_keys.begin(), m_container_keys.end(), Key(key.item()));
  if ( m_container_keys.empty() || key_iter != m_container_keys.end() )    {
    auto mask_iter = std::find(m_input_masks.begin(), m_input_masks.end(), key.mask());
    return m_input_masks.empty() || mask_iter != m_input_masks.end();
  }
  return false;
}

/// Main functional callback if specific work is known
void DigiContainerProcessor::execute(DigiContext& context, WorkItems& work)  const    {
  for( const auto& item : work )  {
    if ( use_container(item.first) )   {
      Key key = item.first;
      info("%s+++ %p Using container: %016lX  --> %04X %08X %s",
	   context.event->id(), (void*)this, key.key, key.mask(), key.item(), Key::key_name(key).c_str());
    }
  }
}
