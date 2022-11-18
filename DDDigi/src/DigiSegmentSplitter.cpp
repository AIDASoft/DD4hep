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
#include <DDDigi/DigiPlugins.h>
#include <DDDigi/DigiSegmentSplitter.h>

using namespace dd4hep::digi;

/// Default copy constructor
DigiSegmentProcessContext::DigiSegmentProcessContext(const DigiSegmentContext& copy)
  : DigiSegmentContext(copy)
{
}

/// Full identifier (field + id)
std::string DigiSegmentProcessContext::identifier()  const   {
  return this->DigiSegmentContext::identifier(this->predicate.id);
}

/// Default move assignment
DigiSegmentProcessContext&
DigiSegmentProcessContext::operator=(const DigiSegmentContext& copy)   {
  this->DigiSegmentContext::operator=(copy);
  return *this;
}

void DigiSegmentProcessContext::enable(uint32_t split_id)  {
  this->predicate.id = split_id;
  this->predicate.segmentation = this;
  this->predicate.callback = std::bind(&DigiSegmentProcessContext::use_depo, this, std::placeholders::_1);
}

/// Worker adaptor for caller DigiContainerSequence
template <> void DigiParallelWorker<DigiContainerProcessor,
				    DigiContainerProcessor::work_t,
				    DigiSegmentProcessContext>::execute(void* ptr) const  {
  calldata_t* args  = reinterpret_cast<calldata_t*>(ptr);
  action->execute(args->environ.context, *args, this->options.predicate);
}

/// Standard constructor
DigiSegmentSplitter::DigiSegmentSplitter(const kernel_t& kernel, const std::string& nam)
  : DigiContainerProcessor(kernel, nam),
    m_split_tool(kernel.detectorDescription())
{
  declareProperty("parallel",        m_parallel = false);
  declareProperty("detector",        m_detector_name);
  declareProperty("split_by",        m_split_by);
  declareProperty("processor_type",  m_processor_type);
  declareProperty("share_processor", m_share_processor = false);
  m_kernel.register_initialize(std::bind(&DigiSegmentSplitter::initialize,this));
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentSplitter::~DigiSegmentSplitter() {
  InstanceCount::decrement(this);
}

/// Retrieve the names of all collection to be accessed
std::vector<std::string> DigiSegmentSplitter::collection_names()  const   {
  if ( !m_detector_name.empty() )   {
    if ( m_splits.empty() )  {
      m_split_tool.set_detector(m_detector_name);
    }
    return m_split_tool.collection_names();
  }
  except("+++ collection_names: The detector name is not set. Unable to access readout properties.");
  return {};
}

/// Initialization function
void DigiSegmentSplitter::initialize()   {
  char text[256];
  std::size_t count = 0;

  m_split_tool.set_detector(m_detector_name);
  m_keys          = m_split_tool.collection_keys();
  m_split_context = m_split_tool.split_context(m_split_by);
  m_splits        = m_split_tool.split_segmentation(m_split_by);

  /// 1) Check if the workers were pre-configured
  if ( !m_workers.empty() )    {
    bool bad = false;
    auto group = m_workers.get_group();
    const auto& workers = group.actors();
    /// Create the processors:
    for( auto split_id : m_splits )   {
      bool ok = false;
      for( auto* w : workers )   {
	if ( w->options.predicate.id == split_id )  {
	  w->options = m_split_context;
	  w->options.enable(split_id);
	  ok = true;
	  break;
	}
      }
      if ( !ok )   {
	error("+++ Missing processor for split ID: %08ld", split_id);
	bad = true;
      }
    }
    if ( bad )   {
      except("+++ If you add processors by hand, do it properly! "
	     "Otherwise use the property 'processor_type'. "
	     "This setup is invalid.");
    }
    return;
  }
  /// IF NOT:
  /// 2) Create the processors using the 'processor_type' option
  for( auto id : m_splits )   {
    ::snprintf(text, sizeof(text), "_%05X", id);
    std::string nam = this->name() + text;
    auto* proc = createAction<DigiContainerProcessor>(m_processor_type, m_kernel, nam);
    if ( !proc )   {
      except("+++ Failed to create split worker: %s/%s", m_processor_type.c_str(), nam.c_str());
    }
    info("+++ Created worker: %s layer: %d", nam.c_str(), id);
    auto* w = new worker_t(proc, m_split_context);
    w->options.enable(id);
    m_workers.insert(w);
    ++count;
  }
  info("+++ Detector splitter is now fully initialized!");
}

/// Adopt new parallel worker handling a single split identifier
void DigiSegmentSplitter::adopt_segment_processor(DigiContainerProcessor* action, int split_id)   {
  if ( !action )  {
    except("+++ adopt_segment_processor: FAILED attempt to add invalid processor!");
  }
  auto* w = new worker_t(action, m_split_context);
  w->options.enable(split_id);
  m_workers.insert(w);
}

/// Adopt new parallel worker handling multiple split-identifiers
void DigiSegmentSplitter::adopt_segment_processor(DigiContainerProcessor* action, const std::vector<int>&  ids)   {
  for( int split_id : ids )
    adopt_segment_processor(action, split_id);
}

/// Main functional callback
void DigiSegmentSplitter::execute(context_t& context, work_t& work, const predicate_t& /* predicate */)  const    {
  Key key = work.input_key();
  Key unmasked_key;
  unmasked_key.set_item(key.item());
  if ( std::find(m_keys.begin(), m_keys.end(), unmasked_key) != m_keys.end() )   {
    if ( work.has_input() )   {
      info("%s+++ Got hit collection %04X %08X. Prepare processors for %sparallel execution.",
	   context.event->id(), key.mask(), key.item(), m_parallel ? "" : "NON-");
      m_kernel.submit(context, m_workers.get_group(), m_workers.size(), &work, m_parallel);
    }
  }
}
