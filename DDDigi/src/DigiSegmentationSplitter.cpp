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
#include <DDDigi/DigiSegmentationSplitter.h>

using namespace dd4hep::digi;

class DigiSegmentAction::internals_t  {
public:
  DigiSegmentContext split { };
  std::vector<std::pair<Key::key_type, std::any> > output;
  const DepositMapping*    input { nullptr };
};

class DigiSegmentationSplitter::internals_t   {
public:
  /// Reference to master
  DigiSegmentationSplitter* split { nullptr };
  /// Flag to check the initialization
  bool inited  { false };
  /// Default constructor
  internals_t(DigiSegmentationSplitter* s) : split(s)  {}
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

/// Standard constructor
DigiSegmentationSplitter::DigiSegmentationSplitter(const DigiKernel& krnl, const std::string& nam)
  : DigiActionSequence(krnl, nam), m_split_tool(krnl.detectorDescription())
{
  this->internals = std::make_unique<internals_t>(this);
  declareProperty("detector",        this->m_detector_name);
  declareProperty("split_by",        this->m_split_by);
  declareProperty("input",           this->m_input_id = "deposits");
  declareProperty("mask",            this->m_input_mask);
  declareProperty("processor_type",  this->m_processor_type);
  declareProperty("share_processor", this->m_share_processor = false);
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentationSplitter::~DigiSegmentationSplitter() {
  InstanceCount::decrement(this);
}


/// Initialization function
void DigiSegmentationSplitter::initialize()   {
  char text[256];
  std::size_t count = 0;
  DigiSegmentAction* proc;

  this->m_split_tool.set_detector(this->m_detector_name);
  this->m_split_context = this->m_split_tool.split_context(this->m_split_by);
  this->m_data_keys = this->m_split_tool.collection_keys(this->m_input_mask);
  this->m_splits = this->m_split_tool.split_segmentation(this->m_split_by);

  /// Create the processors:
  for( auto& p : this->m_splits )   {
    ::snprintf(text, sizeof(text), "_%05X", m_split_context.split_id(p.first));
    std::string nam = this->name() + text;
    proc = PluginService::Create<DigiSegmentAction*>(m_processor_type, &m_kernel, nam);
    proc->internals = std::make_unique<DigiSegmentAction::internals_t>();
    proc->internals->split          = this->m_split_context;
    proc->internals->split.detector = p.second.first;
    proc->internals->split.id       = p.second.second;
    this->DigiActionSequence::adopt(proc);
    ++count;
  }
  info("+++ Detector splitter is now fully initialized!");
}

/// Main functional callback
void DigiSegmentationSplitter::execute(DigiContext& context)  const    {
  auto& input = context.event->get_segment(this->m_input_id);
  this->internals->initialize(context);
  for( auto k : this->m_data_keys )   {
    auto* hits = input.pointer<DepositMapping>(k);
    if ( hits )    {
      /// prepare processors for execution
      for ( auto* a : this->m_actors )   {
	auto* proc  = (DigiSegmentAction*)a;
	proc->internals->input = hits;
      }
      /// Now submit them
      this->DigiActionSequence::execute(context);
    }
  }
}

/// Standard constructor
DigiSegmentAction::DigiSegmentAction(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->internals = std::make_unique<internals_t>();
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentAction::~DigiSegmentAction() {
  InstanceCount::decrement(this);
}

void DigiSegmentAction::execute(DigiContext& context)  const  {
  auto& imp = *this->internals;
  auto ret = this->handleSegment(context, imp.split, *imp.input);
  imp.output = std::move(ret);
}

/// Main functional callback
std::vector<std::pair<Key::key_type, std::any> >
DigiSegmentAction::handleSegment(DigiContext&              context,
				 const DigiSegmentContext& segment,
				 const DepositMapping&     deposits)  const   {
  for( const auto& depo : deposits )   {
    if ( segment.split_id(depo.first) == segment.id )   {
      auto cell = depo.first;
      const auto& d = depo.second;
      info("%s[%s] %s-id: %d [processor:%d] Cell: %016lX mask: %016lX  hist:%4ld entries deposit: %f", 
	   context.event->id(), segment.idspec.name(), segment.cname(), 
	   segment.split_id(cell), segment.id, cell, segment.split_mask, d.history.size(), d.deposit);
    }
  }
  return {};
}
