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
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiSegmentProcessor.h>

using namespace dd4hep::digi;

/// Standard constructor
DigiSegmentProcessor::DigiSegmentProcessor(const DigiKernel& krnl, const std::string& nam)
  : DigiContainerProcessor(krnl, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentProcessor::~DigiSegmentProcessor() {
  InstanceCount::decrement(this);
}

/// Main functional callback if specific work is known
void DigiSegmentProcessor::handle_segment(DigiContext& /* context */, work_t& /* args */)  const   {
}

template <> void DigiParallelWorker<DigiSegmentProcessor,
				    DigiSegmentSequence::work_t,
				    dd4hep::VolumeID>::execute(void* args) const  {
  calldata_t* work = reinterpret_cast<calldata_t*>(args);
  action->handle_segment(work->context, *work);
}

/// Standard constructor
DigiSegmentSequence::DigiSegmentSequence(const DigiKernel& krnl, const std::string& nam)
  : DigiSegmentProcessor(krnl, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentSequence::~DigiSegmentSequence() {
  InstanceCount::decrement(this);
}

/// Adopt new parallel worker
void DigiSegmentSequence::adopt_processor(DigiContainerProcessor* action)   {
  if ( !action )  {
    except("+++ FAILED: attempt to add invalid processor!");
  }
  auto* proc = dynamic_cast<DigiSegmentProcessor*>(action);
  if ( !proc )   {
    error("+++ FAILED: Attempt to add processor %s of type %s",
	  action->c_name(), typeName(typeid(*action)).c_str());
    except("+++ DigiSegmentSplitter do ONLY accept processors of type DigiSegmentProcessor!");
  }
  proc->segment = segment;
  m_workers.insert(new worker_t(proc, proc->segment.id));
}

/// Main functional callback
void DigiSegmentSequence::handle_segment(DigiContext& /* context */, work_t& work) const {
  m_kernel.submit(m_workers.get_group(), m_workers.size(), &work, m_parallel);
}
