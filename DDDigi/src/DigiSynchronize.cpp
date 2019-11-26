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
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiKernel.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiSynchronize.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiSynchronize::DigiSynchronize(const DigiKernel& kernel, const string& nam)
  : DigiEventAction(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSynchronize::~DigiSynchronize() {
  m_actors(&DigiEventAction::release);
  m_actors.clear();
  InstanceCount::decrement(this);
}

/// Get an action sequence member by name
DigiEventAction* DigiSynchronize::get(const string& nam) const   {
  return m_actors.get(FindByName(TypeName::split(nam).second));
}

/// Pre-track action callback
void DigiSynchronize::execute(DigiContext& context)  const   {
  auto start = chrono::high_resolution_clock::now();
  if ( m_parallel )
    m_kernel.submit(m_actors, context);
  else
    m_kernel.execute(m_actors, context);
  chrono::duration<double> secs = chrono::high_resolution_clock::now() - start;
  debug("+++ Event: %8d (DigiSynchronize) Parallel: %-4s  %3ld actions [%8.3g sec]",
        context.event().eventNumber, yes_no(m_parallel), m_actors.size(),
        secs.count());
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void DigiSynchronize::adopt(DigiEventAction* action) {
  if (action)    {
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("DigiSynchronize","++ Attempt to add invalid actor!");
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void DigiSynchronize::analyze() {
  info("+++ Analyzing the algorithm sequence. Parallel: %s",yes_no(m_parallel));
}
