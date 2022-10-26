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
#include <DDDigi/DigiSegmentation.h>
#include <DDDigi/noise/DigiSignalProcessorSequence.h>

// C/C++ include files
#include <stdexcept>

using namespace dd4hep::digi;

template <> void 
DigiParallelWorker<DigiSignalProcessor, 
		   DigiSignalProcessorSequence::CallData,
		   int>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  args->value += (*action)(args->context);
}

/// Standard constructor
DigiSignalProcessorSequence::DigiSignalProcessorSequence(const DigiKernel& kernel, const std::string& nam)
  : DigiSignalProcessor(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSignalProcessorSequence::~DigiSignalProcessorSequence() {
  InstanceCount::decrement(this);
}

/// Adopt a new action as part of the sequence. Sequence takes ownership.
void DigiSignalProcessorSequence::adopt(DigiSignalProcessor* action)    {
  if (action)    {
    m_actors.insert(new Worker(action, 0));
    return;
  }
  except("DigiSignalProcessorSequence","++ Attempt to add invalid actor!");
}

/// Pre-track action callback
double DigiSignalProcessorSequence::operator()(DigiCellContext& context)  const   {
  CallData args { context, 0e0 };
  double result = context.data.signal;
  auto group = m_actors.get_group();
  for ( const auto* p : group.actors() )  {
    args.value = 0e0;
    p->execute(&args);
    result += args.value;
  }
  return context.data.kill ? 0e0 : result;
}
