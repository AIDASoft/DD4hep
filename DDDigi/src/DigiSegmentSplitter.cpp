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
#include <DD4hep/Plugins.h>
#include <DD4hep/InstanceCount.h>

#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiSegmentSplitter.h>

using namespace dd4hep::digi;

class DigiSegmentSplitter::internals_t   {
public:
  /// Reference to master
  DigiSegmentSplitter* split { nullptr };
  /// Flag to check the initialization
  bool inited  { false };
  /// Default constructor
  internals_t(DigiSegmentSplitter* s) : split(s)  {}
  /// Initializing function: compute values which depend on properties
  void initialize(DigiContext& context)   {
    if ( !this->inited )   {
      std::lock_guard<std::mutex> lock(context.initializer_lock());
      if ( !this->inited )   {
	this->split->initialize();
	this->inited = true;
      }
    }
  }
};

template <> void DigiParallelWorker<DigiSegmentAction,
				    DigiSegmentSplitter::CallData,
				    int>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  auto res = action->handleSegment(args->context, args->input);
  if ( args->output && !res.empty() )   {
    args->parent->register_output(*args->output, std::move(res));
  }
}

/// Standard constructor
DigiSegmentSplitter::DigiSegmentSplitter(const DigiKernel& kernel, const std::string& nam)
  : DigiEventAction(kernel, nam),
    m_split_tool(kernel.detectorDescription())
{
  this->internals = std::make_unique<internals_t>(this);
  declareProperty("detector",        m_detector_name);
  declareProperty("split_by",        m_split_by);
  declareProperty("processor_type",  m_processor_type);
  declareProperty("share_processor", m_share_processor = false);

  declareProperty("input_segment",   m_input_id = "deposits");
  declareProperty("input_mask",      m_input_mask);
  declareProperty("output_segment",  m_output_id);
  declareProperty("output_mask",     m_output_mask);
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentSplitter::~DigiSegmentSplitter() {
  for(auto* w : m_workers ) detail::deletePtr(w);
  m_workers.clear();
  InstanceCount::decrement(this);
}


/// Initialization function
void DigiSegmentSplitter::initialize()   {
  char text[256];
  std::size_t count = 0;

  m_split_tool.set_detector(m_detector_name);
  m_split_context = m_split_tool.split_context(m_split_by);
  m_data_keys = m_split_tool.collection_keys(m_input_mask);
  m_splits = m_split_tool.split_segmentation(m_split_by);

  /// Create the processors:
  for( auto& p : m_splits )   {
    ::snprintf(text, sizeof(text), "_%05X", m_split_context.split_id(p.first));
    std::string nam = this->name() + text;
    auto* proc = PluginService::Create<DigiSegmentAction*>(m_processor_type, &m_kernel, nam);
    if ( !proc )   {
      except("+++ Failed to create split worker: %s/%s", m_processor_type.c_str(), nam.c_str());
    }
    proc->segment          = m_split_context;
    proc->segment.detector = p.second.first;
    proc->segment.id       = p.second.second;
    m_workers.emplace_back(new Worker(proc, 0));
    ++count;
  }
  info("+++ Detector splitter is now fully initialized!");
}

/// Main functional callback
void DigiSegmentSplitter::execute(DigiContext& context)  const    {
  auto& input = context.event->get_segment(m_input_id);
  this->internals->initialize(context);
  for( auto k : m_data_keys )   {
    auto* hits = input.pointer<DepositMapping>(k);
    if ( hits )    {
      /// prepare processors for execution
      info("+++ Got hit collection %04X %08X. Prepare processors.", Key::mask(k), Key::item(k));
      /// Now submit them
      if ( m_output_id.empty() )   {
	CallData data { context, *hits, nullptr, this };
	m_kernel.submit(m_workers, &data);
      }
      else   {
	DepositMapping result (m_name+"."+hits->name, m_output_mask);
	CallData data { context, *hits, &result, this };
	m_kernel.submit(m_workers, &data);
	auto& output = context.event->get_segment(m_output_id);
	output.emplace(result.key, std::move(result));
      }
    }
  }
}

/// Handle result from segment callbacks
void DigiSegmentSplitter::register_output(DepositMapping& result,
					       DepositVector&& output)  const  {
  std::lock_guard<std::mutex> lock(m_output_lock);
  result.merge(std::move(output));
}

