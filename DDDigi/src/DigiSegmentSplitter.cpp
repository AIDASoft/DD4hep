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

/// Standard constructor
DigiSegmentSplitter::DigiSegmentSplitter(const DigiKernel& kernel, const std::string& nam)
  : DigiContainerSequence(kernel, nam),
    m_split_tool(kernel.detectorDescription())
{
  declareProperty("detector",        m_detector_name);
  declareProperty("split_by",        m_split_by);
  declareProperty("processor_type",  m_processor_type);
  declareProperty("share_processor", m_share_processor = false);
  m_kernel.register_initialize(Callback(this).make(&DigiSegmentSplitter::initialize));
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
    for( auto& p : m_splits )   {
      auto split_id = p.second.second;
      bool ok = false;
      for( const auto* w : workers )   {
	if ( VolumeID(w->options) == split_id )  { ok = true; break; }
      }
      if ( !ok )   {
	error("+++ Missing processor for plit ID: %08ld", split_id);
	bad = true;
      }
    }
    if ( bad )    {
      except("+++ If you add processors by hand, do it properly! "
	     "Otherwise use the property 'processor_type'. "
	     "This setup is invalid.");
    }
    return;
  }
  /// IF NOT:
  /// 2) Create the processors using the 'processor_type' option
  for( auto& p : m_splits )   {
    ::snprintf(text, sizeof(text), "_%05X", m_split_context.split_id(p.first));
    std::string nam = this->name() + text;
    auto* proc = createAction<DigiSegmentProcessor>(m_processor_type, m_kernel, nam);
    if ( !proc )   {
      except("+++ Failed to create split worker: %s/%s", m_processor_type.c_str(), nam.c_str());
    }
    proc->segment          = m_split_context;
    proc->segment.detector = p.second.first;
    proc->segment.id       = p.second.second;
    m_workers.insert(new worker_t(proc, proc->segment.id));
    ++count;
  }
  info("+++ Detector splitter is now fully initialized!");
}

/// Adopt new parallel worker
void DigiSegmentSplitter::adopt_processor(DigiContainerProcessor* action)   {
  if ( !action )  {
    except("+++ FAILED: attempt to add invalid processor!");
  }
  auto* proc = dynamic_cast<DigiSegmentProcessor*>(action);
  if ( !proc )   {
    error("+++ FAILED: Attempt to add processor %s of type %s",
	  action->c_name(), typeName(typeid(*action)).c_str());
    except("+++ DigiSegmentSplitter do ONLY accept processors of type DigiSegmentProcessor!");
  }
  m_workers.insert(new worker_t(proc, m_workers.size()));
}

/// Main functional callback
void DigiSegmentSplitter::execute(DigiContext& context, work_t& work)  const    {
  Key key = work.input_key();
  Key unmasked_key;
  unmasked_key.set_item(key.item());
  if ( std::find(m_keys.begin(), m_keys.end(), unmasked_key) != m_keys.end() )   {
    if ( work.has_input() )   {
      info("+++ Got hit collection %04X %08X. Prepare processors for %sparallel execution.",
	   key.mask(), key.item(), m_parallel ? "" : "NON-");
      m_kernel.submit(context, m_workers.get_group(), m_workers.size(), &work, m_parallel);
    }
  }
}
