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
#ifndef DD4HEP_DDG4_PROPERTYTESTACTION_H
#define DD4HEP_DDG4_PROPERTYTESTACTION_H

/// Framework include files
#include <DDG4/Geant4Action.h>
#include <DD4hep/ComponentProperties.h>

/// ROOT include files
#include <Math/Point3D.h>
#include <Math/Vector4D.h>
#include <Math/Vector3D.h>

/// C/C++ include files
#include <map>
#include <set>
#include <list>
#include <vector>

/// Namespace example name of the user
namespace DD4hepTests   {

  using namespace std;

  /// Class to measure the energy of escaping tracks
  /** Class to measure the energy of escaping tracks of a detector using Geant 4
   * Measure escaping energy....
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class PropertyTestAction : public dd4hep::sim::Geant4Action {
  public:
    using XYZPoint  = ROOT::Math::XYZPoint;
    using XYZVector = ROOT::Math::XYZVector;
    using PxPyPzE   = ROOT::Math::PxPyPzEVector;
    typedef unsigned long ulong;

    string                   prop_str;               // 'a'
    bool                     prop_bool;              // true
    int                      prop_int;               // 11
    long                     prop_long;              // 11
    unsigned long            prop_ulong;             // 11
    float                    prop_float;             // 1.11
    double                   prop_double;            // 1.11
    XYZPoint                 prop_XYZPoint;          // (1, 2, 3)
    XYZVector                prop_XYZVector;         // (1, 2, 3)
    PxPyPzE                  prop_PxPyPzEVector;     // (1, 2, 3, 4)

    map<string, string>      map_str_str;            // {'a':   'a',      'b': 'bb',        'c': 'ccc'}
    map<string, bool>        map_str_bool;           // {'a':   true,     'b':  false,      'c':  true}
    map<string, int>         map_str_int;            // {'a':   11,       'b':  200,        'c':  3000}
    map<string, long>        map_str_long;           // {'a':   11,       'b':  200,        'c':  3000}
    //map<string, ulong>       map_str_ulong;          // {'a':   11,       'b':  200,        'c':  3000}
    map<string, float>       map_str_float;          // {'a':   1.11,     'b': 22.22,       'c': 333.33}
    map<string, double>      map_str_double;         // {'a':   1.11,     'b': 22.22,       'c': 333.33}

    map<int, string>         map_int_str;            // { 10:   'a',        200:   'bb',        3000: '    ccc'}
    map<int, bool>           map_int_bool;           // { 10:   true,       200:  false,        3000:      true}
    map<int, int>            map_int_int;            // { 10:   11,         200:  200,          3000:      3000}
    map<int, long>           map_int_long;           // { 10:   11,         200:  200,          3000:      3000}
    //map<int, ulong>          map_int_ulong;          // { 10:   11,         200:  200,          3000:      3000}
    map<int, float>          map_int_float;          // { 10:   1.11,       200:   22.22,       3000:    333.33}
    map<int, double>         map_int_double;         // { 10:   1.11,       200:   22.22,       3000:    333.33}

    set<string>              set_str;                // {'a',                    'bb',              'ccc'}
    set<bool>                set_bool;               // {true,                   false,              true}
    set<int>                 set_int;                // {11,                     222,               3333}
    set<long>                set_long;               // {11,                     222,               3333}
    //set<ulong>               set_ulong;              // {11,                     222,               3333}
    set<float>               set_float;              // {1.11,                   22.22,             333.33}
    set<double>              set_double;             // {1.11,                   22.22,             333.33}
    set<XYZPoint>            set_XYZPoint;           // {(1, 2, 3),              (10,20,30),       (100,200,300)}
    set<XYZVector>           set_XYZVector;          // {(1, 2, 3),              (10,20,30),       (100,200,300)}
    set<PxPyPzE>             set_PxPyPzEVector;      // {(1, 2, 3,4),            (10,20,30,40),    (100,200,300,400)}

    vector<string>           vector_str;              // {'a',                    'bb',              'ccc'}
    vector<bool>             vector_bool;             // {true,                   false,              true}
    vector<int>              vector_int;              // {11,                     222,               3333}
    vector<long>             vector_long;             // {11,                     222,               3333}
    vector<ulong>            vector_ulong;            // {11,                     222,               3333}
    vector<float>            vector_float;            // {1.11,                   22.22,             333.33}
    vector<double>           vector_double;           // {1.11,                   22.22,             333.33}
    vector<XYZPoint>         vector_XYZPoint;         // {(1, 2, 3),              (10,20,30), (100,200,300)}
    vector<XYZVector>        vector_XYZVector;        // {(1, 2, 3),              (10,20,30), (100,200,300)}
    vector<PxPyPzE>          vector_PxPyPzEVector;    // {(1, 2, 3,4),            (10,20,30,40),    (100,200,300,400)}

    list<string>             list_str;                // {'a',                    'bb',              'ccc'}
    list<bool>               list_bool;               // {true,                   false,              true}
    list<int>                list_int;                // {11,                     222,               3333}
    list<long>               list_long;               // {11,                     222,               3333}
    list<ulong>              list_ulong;              // {11,                     222,               3333}
    list<float>              list_float;              // {1.11,                   22.22,             333.33}
    list<double>             list_double;             // {1.11,                   22.22,             333.33}
    list<XYZPoint>           list_XYZPoint;           // {(1, 2, 3),              (10,20,30), (100,200,300)}
    list<XYZVector>          list_XYZVector;          // {(1, 2, 3),              (10,20,30), (100,200,300)}
    list<PxPyPzE>            list_PxPyPzEVector;      // {(1, 2, 3,4),            (10,20,30,40),    (100,200,300,400)}

  public:
    /// Standard constructor
    PropertyTestAction(dd4hep::sim::Geant4Context* context, const std::string& nam);
    /// Default destructor
    virtual ~PropertyTestAction() = default;
    /// Dump properties to screen
    void dumpProperties()  const;
    /// Install command control messenger
    virtual void installCommandMessenger()  override;
  };
}      // End namespace dd4hep
#endif /* DD4HEP_DDG4_PROPERTYTESTACTION_H */

//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//====================================================================

// Framework include files
#include <DDG4/Geant4UIMessenger.h>

#include <iostream>
#include <sstream>

using namespace std;

namespace {
  string str_rep(string in, const string& pat, const string& rep)   {
    size_t idx = string::npos;
    string res = in;
    while( (idx=res.find(pat)) != string::npos )
      res.replace(idx, pat.length(), rep);
    return res;
  }
  void _print(stringstream& log)   {
    auto res = str_rep(log.str(), "( ", "(");
    cout << res << endl;
  }
  /// Dump properties to screen
  template <typename T> int dump_item(const string& tag, const T& value)    {
    try  {
      stringstream log;
      log << "| " << setw(32) << left << tag << " " << setw(6) << left << value;
      _print(log);
    }
    catch(const exception& e)   {
      cout << "Test FAILED: " << tag << " Exception: " << e.what() << endl;
      return 1;
    }
    return 0;
  }
  template <typename T> int dump_cont(const string& tag, const T& value)    {
    try  {
      stringstream log;
      log << "| " << setw(32) << left << tag << " ";
      for(const auto& p : value)
	log << setw(6) << left << p << "  ";
      _print(log);
    }
    catch(const exception& e)   {
      cout << "Test FAILED: " << tag << " Exception: " << e.what() << endl;
      return 1;
    }
    return 0;
  }
  template <typename T> int dump_map(const string& tag, const T& value)    {
    try  {
      stringstream log;
      log << "| " << setw(32) << left << tag << " ";
      for(const auto& p : value)
	log << setw(6) << left << p.first << " = " << setw(10) << left << p.second << "  ";
      _print(log);
    }
    catch(const exception& e)   {
      cout << "Test FAILED: " << tag << " Exception: " << e.what() << endl;
      return 1;
    }
    return 0;
  }
}

/// Standard constructor
DD4hepTests::PropertyTestAction::PropertyTestAction(dd4hep::sim::Geant4Context* ctxt, const string& nam)
  : dd4hep::sim::Geant4Action(ctxt, nam)
{
  declareProperty("prop_str",   prop_str); 
  declareProperty("prop_bool",  prop_bool);
  declareProperty("prop_int",   prop_int);
  declareProperty("prop_long",  prop_long);
  declareProperty("prop_ulong",  prop_ulong);
  declareProperty("prop_float", prop_float);           
  declareProperty("prop_double", prop_double);          
  declareProperty("prop_XYZPoint", prop_XYZPoint);        
  declareProperty("prop_XYZVector", prop_XYZVector);         
  declareProperty("prop_PxPyPzEVector", prop_PxPyPzEVector);

  declareProperty("map_str_str", map_str_str);
  declareProperty("map_str_bool", map_str_bool);
  declareProperty("map_str_int", map_str_int);
  declareProperty("map_str_long", map_str_long);
  //declareProperty("map_str_ulong", map_str_ulong);
  declareProperty("map_str_float", map_str_float);
  declareProperty("map_str_double", map_str_double);

  declareProperty("map_int_str", map_int_str);
  declareProperty("map_int_bool", map_int_bool);
  declareProperty("map_int_int", map_int_int);
  declareProperty("map_int_long", map_int_long);
  //declareProperty("map_int_ulong", map_int_ulong);
  declareProperty("map_int_float", map_int_float);
  declareProperty("map_int_double", map_int_double);

  declareProperty("set_str", set_str);
  declareProperty("set_bool", set_bool);
  declareProperty("set_int", set_int);
  declareProperty("set_long", set_long);
  //declareProperty("set_ulong", set_ulong);
  declareProperty("set_float", set_float);
  declareProperty("set_double", set_double);
  declareProperty("set_XYZPoint", set_XYZPoint);
  declareProperty("set_XYZVector", set_XYZVector);
  declareProperty("set_PxPyPzEVector", set_PxPyPzEVector);

  declareProperty("vector_str", vector_str);
  declareProperty("vector_bool", vector_bool);
  declareProperty("vector_int", vector_int);
  declareProperty("vector_long", vector_long);
  declareProperty("vector_ulong", vector_ulong);
  declareProperty("vector_float", vector_float);
  declareProperty("vector_double", vector_double);
  declareProperty("vector_XYZPoint", vector_XYZPoint);
  declareProperty("vector_XYZVector", vector_XYZVector);
  declareProperty("vector_PxPyPzEVector", vector_PxPyPzEVector);

  declareProperty("list_str", list_str);
  declareProperty("list_bool", list_bool);
  declareProperty("list_int", list_int);
  declareProperty("list_long", list_long);
  declareProperty("list_ulong", list_ulong);
  declareProperty("list_float", list_float);
  declareProperty("list_double", list_double);
  declareProperty("list_XYZPoint", list_XYZPoint);
  declareProperty("list_XYZVector", list_XYZVector);
  declareProperty("list_PxPyPzEVector", list_PxPyPzEVector);
}

/// Install command control messenger
void DD4hepTests::PropertyTestAction::installCommandMessenger()   {
  this->Geant4Action::installCommandMessenger();
  m_control->addCall("dumpProperties", "Execute property check",
                     dd4hep::Callback(this).make(&PropertyTestAction::dumpProperties),0);
}

/// Dump properties to screen
void DD4hepTests::PropertyTestAction::dumpProperties()  const    {
  int result = 0;
  const char* line = "+----------------------------------------------------------------------------------------------------------";
  cout << line << endl;
  result += dump_item("prop_str", prop_str); 
  result += dump_item("prop_bool", prop_bool);
  result += dump_item("prop_int", prop_int);
  result += dump_item("prop_long", prop_long);
  result += dump_item("prop_ulong", prop_ulong);
  result += dump_item("prop_float", prop_float);           
  result += dump_item("prop_double", prop_double);          
  result += dump_item("prop_XYZPoint", prop_XYZPoint);        
  result += dump_item("prop_XYZVector", prop_XYZVector);         
  result += dump_item("prop_PxPyPzEVector", prop_PxPyPzEVector);
  cout << line << endl;
  result += dump_map("map_str_str", map_str_str);
  result += dump_map("map_str_bool", map_str_bool);
  result += dump_map("map_str_int", map_str_int);
  result += dump_map("map_str_long", map_str_long);
  //result += dump_map("map_str_ulong", map_str_ulong);
  result += dump_map("map_str_float", map_str_float);
  result += dump_map("map_str_double", map_str_double);
  cout << line << endl;
  result += dump_map("map_int_str", map_int_str);
  result += dump_map("map_int_bool", map_int_bool);
  result += dump_map("map_int_int", map_int_int);
  result += dump_map("map_int_long", map_int_long);
  //result += dump_map("map_int_ulong", map_int_ulong);
  result += dump_map("map_int_float", map_int_float);
  result += dump_map("map_int_double", map_int_double);
  cout << line << endl;
  result += dump_cont("set_str", set_str);
  result += dump_cont("set_bool", set_bool);
  result += dump_cont("set_int", set_int);
  result += dump_cont("set_long", set_long);
  //result += dump_cont("set_ulong", set_ulong);
  result += dump_cont("set_float", set_float);
  result += dump_cont("set_double", set_double);
  result += dump_cont("set_XYZPoint", set_XYZPoint);
  result += dump_cont("set_XYZVector", set_XYZVector);
  result += dump_cont("set_PxPyPzEVector", set_PxPyPzEVector);
  cout << line << endl;
  result += dump_cont("vector_str", vector_str);
  result += dump_cont("vector_bool", vector_bool);
  result += dump_cont("vector_int", vector_int);
  result += dump_cont("vector_long", vector_long);
  result += dump_cont("vector_ulong", vector_ulong);
  result += dump_cont("vector_float", vector_float);
  result += dump_cont("vector_double", vector_double);
  result += dump_cont("vector_XYZPoint", vector_XYZPoint);
  result += dump_cont("vector_XYZVector", vector_XYZVector);
  result += dump_cont("vector_PxPyPzEVector", vector_PxPyPzEVector);
  cout << line << endl;
  result += dump_cont("list_str", list_str);
  result += dump_cont("list_bool", list_bool);
  result += dump_cont("list_int", list_int);
  result += dump_cont("list_long", list_long);
  result += dump_cont("list_ulong", list_ulong);
  result += dump_cont("list_float", list_float);
  result += dump_cont("list_double", list_double);
  result += dump_cont("list_XYZPoint", list_XYZPoint);
  result += dump_cont("list_XYZVector", list_XYZVector);
  result += dump_cont("list_PxPyPzEVector", list_PxPyPzEVector);
  cout << line << endl;

  if ( 0 == result )
    cout << endl << "Test PASSED" << endl << endl;
  else
    cout << endl << "Test FAILED" << endl << "===> " << result << " Subtests FAILED" << endl;
  cout << line << endl;
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(DD4hepTests,PropertyTestAction)
