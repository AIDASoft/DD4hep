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

template <> void DigiParallelWorker<DigiContainerProcessor, DigiMultiContainerProcessor::ProcessorOptions>::operator()() const  {
  processor->execute(*options.context, *options.work);
}

/// Standard constructor
DigiMultiContainerProcessor::DigiMultiContainerProcessor(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->declareProperty("input_masks",      m_input_masks);
  this->declareProperty("input_segment",    m_input_segment);
  this->declareProperty("allow_duplicates", m_allow_duplicates);
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
  const char* aname = action->name().c_str();
  std::stringstream str;
  std::vector<Key> keys;
  for(const auto& c : containers)    {
    Key key(0x0, c);
    if ( !m_allow_duplicates )    {
      for(const auto& w : m_workers)   {
	if ( std::find(m_work_items.begin(), m_work_items.end(), key.item()) != m_work_items.end() )   {
	  error("+++ Container %s has already a worker action attached: %s",
		c.c_str(), w->name());
	  except("+++ Need to set property allow_duplicates=True to allow such behavior.");
	}
      }
    }
    keys.push_back(key);
    m_work_items.insert(key.item());
    str << c << " ";
  }
  action->m_work_keys = keys;
  Worker* w = new Worker(action, {nullptr, nullptr, keys});
  m_callers.emplace_back(new DigiKernel::CallWrapper(w));
  m_workers.emplace_back(std::unique_ptr<Worker>(w));
  info("+++ Use processor: %-32s for processing: %s", aname, str.str().c_str());
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
    for(std::size_t i=0; i < m_workers.size(); ++i)   {
      m_workers[i]->options.context = &context;
      m_workers[i]->options.work = &work_items;
    }
    m_kernel.submit(m_callers);
  }
}

/// Standard constructor
DigiContainerProcessor::DigiContainerProcessor(const DigiKernel& kernel, const std::string& name)   
  : DigiAction(kernel, name)
{
  this->declareProperty("input_masks",      m_input_masks);
  this->declareProperty("input_segment",    m_input_segment);
}

/// Main functional callback if specific work is known
void DigiContainerProcessor::execute(DigiContext& context, WorkItems& data)  const    {
  info("Hello there %p", (void*)&data);
}
