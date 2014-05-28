// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Exceptions.h"
#include "DD4hep/BasicGrammar.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;

/// Default constructor
BasicGrammar::BasicGrammar()  {
}

/// Default destructor
BasicGrammar::~BasicGrammar()   {
}

/// Error callback on invalid conversion
void BasicGrammar::invalidConversion(const string& value, const type_info& to) {
  string to_name = typeName(to);
  throw unrelated_value_error(to,
      "The Property data conversion of '" + value + "' to type " + to_name + " is not defined.");
}

/// Error callback on invalid conversion
void BasicGrammar::invalidConversion(const type_info& from, const type_info& to) {
  string to_name = typeName(to);
  string from_name = typeName(from);
  throw unrelated_type_error(from, to,
      "The Property data conversion from type " + from_name + " to " + to_name + " is not implemented.");
}
