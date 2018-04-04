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
#include "DDCond/ConditionsCleanup.h"

using namespace dd4hep::cond;

/// Cleanup operation
void ConditionsFullCleanup::operator()(ConditionsManager mgr) const    {
  mgr.clear();
}
