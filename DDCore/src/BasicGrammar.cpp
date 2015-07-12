// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Exceptions.h"
#include "DD4hep/BasicGrammar.h"

// C/C++ include files
#include <stdexcept>

/// Default constructor
DD4hep::BasicGrammar::BasicGrammar()  {
}

/// Default destructor
DD4hep::BasicGrammar::~BasicGrammar()   {
}

/// Error callback on invalid conversion
void DD4hep::BasicGrammar::invalidConversion(const std::string& value, const std::type_info& to) {
  std::string to_name = typeName(to);
  throw unrelated_value_error(to,
                              "Data conversion of '" + value + "' to type '" + 
                              to_name + "' is not defined.");
}

/// Error callback on invalid conversion
void DD4hep::BasicGrammar::invalidConversion(const std::type_info& from, const std::type_info& to) {
  std::string to_name = typeName(to);
  std::string from_name = typeName(from);
  throw unrelated_type_error(from, to,
                             "Data conversion from '" + from_name + 
                             "' to '" + to_name + "' is not implemented.");
}
