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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiLockedAction.h"

// C/C++ include files

using namespace std;
using namespace dd4hep::digi;


/// Standard constructor
DigiLockedAction::DigiLockedAction(const DigiKernel& kernel, const string& nam)
  : DigiAction(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiLockedAction::~DigiLockedAction()   {
  dd4hep::detail::releasePtr(m_action);
  InstanceCount::decrement(this);
}

/// Underlying object to be used during the locked execution
void DigiLockedAction::use(DigiAction* action)   {
  if (action) {
    action->addRef();
    m_properties.adopt(action->properties());
    m_action = action;
    return;
  }
  fatal("DigiLockedAction: Attempt to use invalid actor!");
}

/// Pre-track action callback
void DigiLockedAction::execute(DigiContext& context)  const   {
  if (m_action) {
    m_action->execute(context);
    return;
  }
  fatal("DigiLockedAction: Attempt to use invalid actor! "
        "Did you call DigiLockedAction::use?");
}
