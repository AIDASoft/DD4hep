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
#include <DD4hep/ComponentProperties.h>
#include <DD4hep/Factories.h>
#include <iostream>

using namespace std;
using namespace dd4hep;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_Conditions_dynamic
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */

template <typename T> T _test_read(const std::string& tag, const std::string& data)    {
  T value;
  Property prop(value);
  prop.str(data);
  for(const auto& p : value)
    std::cout << std::setw(32) << std::left << tag << " " << p.first << " = " << p.second << std::endl;
  return value;
}

static int property_test(Detector& /* description */, int /* argc */, char** /* argv */)  {

  _test_read<std::map<std::string, int> >        ("map_str_int",    "{'a': 10, 'b': 100, 'c': 1000}");
  _test_read<std::map<std::string, double> >     ("map_str_double", "{'a': 1.0, 'b': 200.5, 'c': 3000.9}");
  _test_read<std::map<std::string, std::string> >("map_str_str",    "{'a': '10', 'b': '100', 'c': '1000'}");
  _test_read<std::map<std::string, std::string> >("map_str_strdouble", "{'a': 1.0, 'b': 200.5, 'c': 3000.9}");

  //_test_read<std::map<std::string, double> >     ("map_str_eval",   "{'a': 1.0*GeV, 'b': 200.5*MeV, 'c': 3000.9*TeV}");

  cout << endl << "Test PASSED" << endl << endl;
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_property_test,property_test)
