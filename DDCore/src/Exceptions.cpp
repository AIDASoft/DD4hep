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
#include <DD4hep/Exceptions.h>
#include <DD4hep/Primitives.h>

using namespace dd4hep;

std::string unrelated_type_error::msg(const std::type_info& typ1, const std::type_info& typ2, const std::string& text) {
  std::string m = "The types " + typeName(typ1) + " and " + typeName(typ2) + " are not related. " + text;
  return m;
}

std::string unrelated_value_error::msg(const std::type_info& typ, const std::string& text) {
  std::string m = "The type " + typeName(typ) + " cannot be converted: " + text;
  return m;
}
