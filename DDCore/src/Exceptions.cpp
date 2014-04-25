// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Exceptions.h"
#include "DD4hep/Primitives.h"

using namespace std;
using namespace DD4hep;

string unrelated_type_error::msg(const type_info& typ1, const type_info& typ2, const string& text) {
  string m = "The types " + typeName(typ1) + " and " + typeName(typ2) + " are not related. " + text;
  return m;
}

string unrelated_value_error::msg(const type_info& typ, const string& text) {
  string m = "The type " + typeName(typ) + " cannot be converted: " + text;
  return m;
}
