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
  DigiSegmentContext    split  { };
  DepositVector         output { };
  const DepositMapping* input  { nullptr };
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
DigiSegmentationSplitter::DigiSegmentationSplitter(const DigiKernel& kernel, const std::string& nam)
  : DigiActionSequence(kernel, nam),
    m_split_tool(kernel.detectorDescription())
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

  this->m_split_tool.set_detector(this->m_detector_name);
  this->m_split_context = this->m_split_tool.split_context(this->m_split_by);
  this->m_data_keys = this->m_split_tool.collection_keys(this->m_input_mask);
  this->m_splits = this->m_split_tool.split_segmentation(this->m_split_by);

  /// Create the processors:
  for( auto& p : this->m_splits )   {
    ::snprintf(text, sizeof(text), "_%05X", m_split_context.split_id(p.first));
    std::string nam = this->name() + text;
    auto* eproc = PluginService::Create<DigiEventAction*>(m_processor_type, &m_kernel, nam);
    if ( !eproc )   {
      except("+++ Failed to create split worker: %s/%s", m_processor_type.c_str(), nam.c_str());
    }
    auto* proc = dynamic_cast<DigiSegmentAction*>(eproc);
    if ( !proc )   {
      except("+++ Split worker: %s/%s is not of type DigiSegmentAction!",
	     m_processor_type.c_str(), nam.c_str());
    }
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
      info("+++ Got hit collection %04X %08X. Prepare processors.",
	   Key::mask(k), Key::item(k));
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
  imp.output = this->handleSegment(context, imp.split, *imp.input);
}

/// Main functional callback
DepositVector
DigiSegmentAction::handleSegment(DigiContext&              /* context */,
				 const DigiSegmentContext& /* segment */,
				 const DepositMapping&     /* depos   */) const {
  return {};
}
