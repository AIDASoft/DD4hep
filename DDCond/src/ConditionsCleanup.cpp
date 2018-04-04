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

/// Request cleanup operation of IOV POOL
bool ConditionsCleanup::operator()(const ConditionsIOVPool & /* iov_pool */) const
{
  return false;
}

/// Request cleanup operation of regular conditiions pool
bool ConditionsCleanup::operator()(const ConditionsPool & /* iov_pool */) const
{
  return true;
}

/// Request cleanup operation of IOV POOL
bool ConditionsFullCleanup::operator()(const ConditionsIOVPool & /* iov_pool */) const
{
  return true;
}

/// Request cleanup operation of regular conditiions pool
bool ConditionsFullCleanup::operator()(const ConditionsPool & /* iov_pool */) const
{
  return true;
}
