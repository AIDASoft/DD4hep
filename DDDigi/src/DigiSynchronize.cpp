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

using namespace std;
using namespace dd4hep::digi;


template <> void 
DigiParallelWorker<DigiEventAction, DigiContext, int>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  action->execute(*args);
}

/// Standard constructor
DigiSynchronize::DigiSynchronize(const DigiKernel& kernel, const string& nam)
  : DigiEventAction(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSynchronize::~DigiSynchronize() {
  for(auto* w : m_actors ) detail::deletePtr(w);
  m_actors.clear();
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiSynchronize::execute(DigiContext& context)  const   {
  auto start = chrono::high_resolution_clock::now();
  if ( m_parallel )
    m_kernel.submit(m_actors, &context);
  else
    m_kernel.execute(m_actors, &context);
  chrono::duration<double> secs = chrono::high_resolution_clock::now() - start;
  debug("+++ Event: %8d (DigiSynchronize) Parallel: %-4s  %3ld actions [%8.3g sec]",
        context.event->eventNumber, yes_no(m_parallel), m_actors.size(),
        secs.count());
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void DigiSynchronize::adopt(DigiEventAction* action) {
  if (action)    {
    m_actors.emplace_back(new Worker(action, 0));
    return;
  }
  except("DigiSynchronize","++ Attempt to add invalid actor!");
}
