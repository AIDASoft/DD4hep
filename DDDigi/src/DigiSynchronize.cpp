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
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiSynchronize.h>

// C/C++ include files
#include <stdexcept>

using namespace dd4hep::digi;


template <> void 
DigiParallelWorker<DigiEventAction, DigiSynchronize::work_t, std::size_t, DigiSynchronize&>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  action->execute(*args);
}

/// Standard constructor
DigiSynchronize::DigiSynchronize(const DigiKernel& kernel, const std::string& nam)
  : DigiEventAction(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSynchronize::~DigiSynchronize() {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiSynchronize::execute(DigiContext& context)  const   {
  auto start = std::chrono::high_resolution_clock::now();
  if ( !m_actors.empty() )   {
    m_kernel.submit(context, m_actors.get_group(), m_actors.size(), &context, m_parallel);
  }
  std::chrono::duration<double> secs = std::chrono::high_resolution_clock::now() - start;
  const DigiEvent& ev = *context.event;
  debug("%s+++ Event: %8d (DigiSynchronize) Parallel: %-4s  %3ld actions [%8.3g sec]",
        ev.id(), ev.eventNumber, yes_no(m_parallel), m_actors.size(),
        secs.count());
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void DigiSynchronize::adopt(DigiEventAction* action) {
  if (action)    {
    m_actors.insert(new worker_t(action, m_actors.size(), *this));
    return;
  }
  except("DigiSynchronize","++ Attempt to add invalid actor!");
}
