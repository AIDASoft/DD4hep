// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Callback.h"
#include "DD4hep/Exceptions.h"

using namespace DD4hep;

/// Check the compatibility of two typed objects. The test is the result of a dynamic_cast
void CallbackSequence::checkTypes(const std::type_info& typ1, const std::type_info& typ2, void* test) {
  if (!test) {
    throw unrelated_type_error(typ1, typ2, "Cannot install a callback for these 2 types.");
  }
}

