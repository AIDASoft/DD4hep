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
#include "DD4hep/ExtensionEntry.h"
#include "DD4hep/Printout.h"

/// Callback on invalid call invokation
void dd4hep::ExtensionEntry::invalidCall(const char* tag)  const {
  except("ExtensionEntry","Invalid call invocation from %s to method %s(...).",
         typeName(typeid(*this)).c_str(), tag);    
}
