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
#include "Parsers/Exceptions.h"
#include "Parsers/Primitives.h"

using namespace std;
using namespace dd4hep;

string unrelated_type_error::msg(const type_info& typ1, const type_info& typ2, const string& text) {
  string m = "The types " + typeName(typ1) + " and " + typeName(typ2) + " are not related. " + text;
  return m;
}

string unrelated_value_error::msg(const type_info& typ, const string& text) {
  string m = "The type " + typeName(typ) + " cannot be converted: " + text;
  return m;
}
