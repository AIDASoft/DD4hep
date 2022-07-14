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
#include <deque>

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
#include <sstream>
namespace   {
  template <typename T> int _test_prop(const string& tag, const string& data)    {
    T value;
    stringstream log;
    Property prop(value);
    try  {
      prop.str(data);
      log << "| " << setw(32) << left << tag << " " << setw(6) << left << "" << "   " << setw(10) << left << value << "  ";
      cout << log.str() << endl;
    }
    catch(const exception& e)   {
      cout << "Test FAILED: " << tag << " Exception: " << e.what() << endl;
      return 1;
    }
    return 0;
  }
  template <typename T> int _test_cont(const string& tag, const string& data)    {
    T value;
    stringstream log;
    Property prop(value);
    try  {
      prop.str(data);
      log << "| " << setw(32) << left << tag << " ";
      for(const auto& p : value)
	log << setw(6) << left << "" << "   " << setw(10) << left << p << "  ";
      cout << log.str() << endl;
    }
    catch(const exception& e)   {
      cout << "Test FAILED: " << tag << " Exception: " << e.what() << endl;
      return 1;
    }
    return 0;
  }
  template <typename T> int _test_map(const string& tag, const string& data)    {
    T value;
    stringstream log;
    Property prop(value);
    try  {
      prop.str(data);
      log << "| " << setw(32) << left << tag << " ";
      for(const auto& p : value)
	log << setw(6) << left << p.first << " = " << setw(10) << left << p.second << "  ";
      cout << log.str() << endl;
    }
    catch(const exception& e)   {
      cout << "Test FAILED: " << tag << " Exception: " << e.what() << endl;
      return 1;
    }
    return 0;
  }
}

static int property_test(Detector& /* description */, int /* argc */, char** /* argv */)  {
  using XYZPoint = ROOT::Math::XYZPoint;
  using XYZVector = ROOT::Math::XYZVector;
  using PxPyPzE = ROOT::Math::PxPyPzEVector;

  int result = 0;
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_prop<string>                   ("prop_str",                "'a'");
  result += _test_prop<bool>                     ("prop_bool",               "true");
  result += _test_prop<int>                      ("prop_int",                "11");
  result += _test_prop<int>                      ("prop_int_eval",           "1*11");
  result += _test_prop<float>                    ("prop_float",              "1.11");
  result += _test_prop<float>                    ("prop_float_eval",         "1.11*GeV");
  result += _test_prop<double>                   ("prop_double",             "1.11");
  result += _test_prop<double>                   ("prop_double_eval",        "1.11*GeV");
  result += _test_prop<XYZPoint>                 ("prop_XYZPoint",           "(1, 2, 3)");
  result += _test_prop<XYZPoint>                 ("prop_XYZPoint_eval",      "(1*m, 2*m, 3*m)");
  result += _test_prop<XYZVector>                ("prop_XYZVector",          "(1, 2, 3)");
  result += _test_prop<XYZVector>                ("prop_XYZVector_eval",     "(1*GeV, 2*GeV, 3*GeV)");
  result += _test_prop<PxPyPzE>                  ("prop_PxPyPzEVector",      "(1, 2, 3, 4)");
  result += _test_prop<PxPyPzE>                  ("prop_PxPyPzEVector_eval", "(1*GeV, 2*GeV, 3*GeV, 4*GeV)");
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_map <map<string, string> >     ("map_str_str",             "{'a':   'a',      'b': 'bb',        'c': 'ccc'}");
  result += _test_map <map<string, bool> >       ("map_str_bool",            "{'a':   true,     'b':  false,      'c':  true}");
  result += _test_map <map<string, int> >        ("map_str_int",             "{'a':   11,       'b':  200,        'c':  3000}");
  result += _test_map <map<string, int> >        ("map_str_int_eval",        "{'a':   1*11,     'b': 2*100,       'c': 3*1000}");
  result += _test_map <map<string, float> >      ("map_str_float",           "{'a':   1.11,     'b': 22.22,       'c': 333.33}");
  result += _test_map <map<string, float> >      ("map_str_float_eval",      "{'a':   1.11*GeV, 'b': 22.22*MeV,   'c': 333.3*TeV}");
  result += _test_map <map<string, double> >     ("map_str_double",          "{'a':   1.11,     'b': 22.22,       'c': 333.33}");
  result += _test_map <map<string, double> >     ("map_str_double_eval",     "{'a':   1.11*GeV, 'b': 22.22*MeV,   'c': 333.3*TeV}");
  //result += _test_map <map<string, XYZPoint> >   ("map_str_XYZPoint",        "{['a', (1, 2, 3)]}");//, 'b': (10,20,30), 'c': (100,200,300)}");
  //result += _test_map <map<string, XYZVector> >  ("map_str_XYZVector",       "{'a': (1, 2, 3), 'b': (10,20,30), 'c': (100,200,300)}");
  //result += _test_map <map<string, PxPyPzE> >    ("map_str_PxPyPzEVector",   "{'a': (1, 2, 3, 4), 'b': (10,20,30,40), 'c': (100,200,300,400)}");
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_map <map<int, string> >        ("map_int_str",             "{ 10:   'a',        200:   'bb',        3000: '    ccc'}");
  result += _test_map <map<int, bool> >          ("map_int_bool",            "{ 10:   true,       200:  false,        3000:      true}");
  result += _test_map <map<int, int> >           ("map_int_int",             "{ 10:   11,         200:  200,          3000:      3000}");
  result += _test_map <map<int, int> >           ("map_int_int_eval",        "{ 10:   1*11,       200: 2*100,         3000:    3*1000}");
  result += _test_map <map<int, float> >         ("map_int_float",           "{ 10:   1.11,       200:   22.22,       3000:    333.33}");
  result += _test_map <map<int, float> >         ("map_int_float_eval",      "{ 10:   1.11*GeV,   200:   22.22*MeV,   3000: 333.3*TeV}");
  result += _test_map <map<int, double> >        ("map_int_double",          "{ 10:   1.11,       200:   22.22,       3000:    333.33}");
  result += _test_map <map<int, double> >        ("map_int_double_eval",     "{ 10:   1.11*GeV,   200:   22.22*MeV,   3000: 333.3*TeV}");
  //result += _test_map <map<int, string> >        ("map_eval_int_str",        "{ 1*10: 'a',      2*100:   'bb',      3*1000:     'ccc'}");
  result += _test_map <map<int, double> >        ("map_eval_int_double",     "{ 1*10: 1.11,     2*100: 22.22,       3*1000:    333.33}");
  //result += _test_map <map<int, XYZPoint> >      ("map_int_XYZPoint",        "{ 10: (1, 2, 3),    200: (10,20,30),    3000: (100,200,300)}");
  //result += _test_map <map<int, XYZVector> >     ("map_int_XYZVector",       "{ 10: (1, 2, 3),    200: (10,20,30),    3000: (100,200,300)}");
  //result += _test_map <map<int, PxPyPzE> >       ("map_int_PxPyPzEVector",   "{ 10: (1, 2, 3, 4), 200: (10,20,30,40), 3000: (100,200,300,400)}");
#if defined(DD4HEP_HAVE_ALL_PARSERS)
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_map <map<float, string> >      ("map_float_str",           "{ 10:   'a',      200:   'bb',        3000: 'ccc'}");
  result += _test_map <map<float, bool> >        ("map_float_bool",          "{ 10:   true,     200:  false,        3000:  true}");
  result += _test_map <map<float, int> >         ("map_float_int",           "{ 10:   11,       200:  200,          3000:  3000}");
  result += _test_map <map<float, int> >         ("map_float_int_eval",      "{ 10:   1*11,     200: 2*100,         3000: 3*1000}");
  result += _test_map <map<float, float> >       ("map_float_float",         "{ 10:   1.11,     200:   22.22,       3000: 333.33}");
  result += _test_map <map<float, float> >       ("map_float_float_eval",    "{ 10:   1.11*GeV, 200:   22.22*MeV,   3000: 333.3*TeV}");
  result += _test_map <map<float, double> >      ("map_float_double",        "{ 10:   1.11,     200:   22.22,       3000: 333.33}");
  result += _test_map <map<float, double> >      ("map_float_double_eval",   "{ 10:   1.11*GeV, 200:   22.22*MeV,   3000: 333.3*TeV}");
#endif
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_cont<set<string> >             ("set_str",                 "{'a',                    'bb',              'ccc'}");
  result += _test_cont<set<bool> >               ("set_bool",                "{true,                   false,              true}");
  result += _test_cont<set<int> >                ("set_int",                 "{11,                     222,               3333}");
  result += _test_cont<set<int> >                ("set_int_eval",            "{1*11,                   2*111,           3*1111}");
  result += _test_cont<set<float> >              ("set_float",               "{1.11,                   22.22,             333.33}");
  result += _test_cont<set<float> >              ("set_float_eval",          "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<set<double> >             ("set_double",              "{1.11,                   22.22,             333.33}");
  result += _test_cont<set<double> >             ("set_double_eval",         "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<set<XYZPoint> >           ("set_XYZPoint",            "{(1, 2, 3),              (10,20,30),       (100,200,300)}");
  result += _test_cont<set<XYZPoint> >           ("set_XYZPoint_eval",       "{(1*m, 2*m, 3*m),        (10*m,20*m,30*m), (100*m,200*m,300*m)}");
  result += _test_cont<set<XYZVector> >          ("set_XYZVector",           "{(1, 2, 3),              (10,20,30),       (100,200,300)}");
  result += _test_cont<set<XYZVector> >          ("set_XYZVector_eval",      "{(1*m, 2*m, 3*m),        (10*m,20*m,30*m), (100*m,200*m,300*m)}");
  result += _test_cont<set<PxPyPzE> >            ("set_PxPyPzEVector",       "{(1, 2, 3,4),            (10,20,30,40),    (100,200,300,400)}");
  result += _test_cont<set<PxPyPzE> >            ("set_PxPyPzEVector_eval",  "{(1*m, 2*m, 3*m, 4*m),   (10*m,20*m,30*m,40*m), (100*m,200*m,300*m,400*m)}");
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_cont<vector<string> >          ("vector_str",               "{'a',                    'bb',              'ccc'}");
  result += _test_cont<vector<bool> >            ("vector_bool",              "{true,                   false,              true}");
  result += _test_cont<vector<int> >             ("vector_int",               "{11,                     222,               3333}");
  result += _test_cont<vector<int> >             ("vector_int_eval",          "{1*11,                   2*111,           3*1111}");
  result += _test_cont<vector<float> >           ("vector_float",             "{1.11,                   22.22,             333.33}");
  result += _test_cont<vector<float> >           ("vector_float_eval",        "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<vector<double> >          ("vector_double",            "{1.11,                   22.22,             333.33}");
  result += _test_cont<vector<double> >          ("vector_double_eval",       "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<vector<XYZPoint> >        ("vector_XYZPoint",          "{(1, 2, 3),              (10,20,30), (100,200,300)}");
  result += _test_cont<vector<XYZPoint> >        ("vector_XYZPoint_eval",     "{(1*m, 2*m, 3*m),        (10*m,20*m,30*m), (100*m,200*m,300*m)}");
  result += _test_cont<vector<XYZVector> >       ("vector_XYZVector",         "{(1, 2, 3),              (10,20,30), (100,200,300)}");
  result += _test_cont<vector<XYZVector> >       ("vector_XYZVector_eval",    "{(1*m, 2*m, 3*m),        (10*m,20*m,30*m), (100*m,200*m,300*m)}");
  result += _test_cont<vector<PxPyPzE> >         ("vector_PxPyPzEVector",     "{(1, 2, 3,4),            (10,20,30,40),    (100,200,300,400)}");
  result += _test_cont<vector<PxPyPzE> >         ("vector_PxPyPzEVector_eval","{(1*m, 2*m, 3*m, 4*m),   (10*m,20*m,30*m,40*m), (100*m,200*m,300*m,400*m)}");
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
  result += _test_cont<list<string> >            ("list_str",                 "{'a',                    'bb',              'ccc'}");
  result += _test_cont<list<bool> >              ("list_bool",                "{true,                   false,              true}");
  result += _test_cont<list<int> >               ("list_int",                 "{11,                     222,               3333}");
  result += _test_cont<list<int> >               ("list_int_eval",            "{1*11,                   2*111,           3*1111}");
  result += _test_cont<list<float> >             ("list_float",               "{1.11,                   22.22,             333.33}");
  result += _test_cont<list<float> >             ("list_float_eval",          "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<list<double> >            ("list_double",              "{1.11,                   22.22,             333.33}");
  result += _test_cont<list<double> >            ("list_double_eval",         "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<list<XYZPoint> >          ("list_XYZPoint",            "{(1, 2, 3),              (10,20,30), (100,200,300)}");
  result += _test_cont<list<XYZVector> >         ("list_XYZVector",           "{(1, 2, 3),              (10,20,30), (100,200,300)}");
  result += _test_cont<list<PxPyPzE> >           ("list_PxPyPzEVector",       "{(1, 2, 3,4),            (10,20,30,40), (100,200,300,400)}");
  result += _test_cont<list<XYZVector> >         ("list_XYZVector_eval",      "{(1*m, 2*m, 3*m),        (10*m,20*m,30*m), (100*m,200*m,300*m)}");
  result += _test_cont<list<PxPyPzE> >           ("list_PxPyPzEVector",       "{(1, 2, 3,4),            (10,20,30,40),    (100,200,300,400)}");
  result += _test_cont<list<PxPyPzE> >           ("list_PxPyPzEVector_eval",  "{(1*m, 2*m, 3*m, 4*m),   (10*m,20*m,30*m,40*m), (100*m,200*m,300*m,400*m)}");
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
#if defined(DD4HEP_HAVE_ALL_PARSERS)
  result += _test_cont<deque<string> >           ("deque_str",                "{'a',                    'bb',              'ccc'}");
  result += _test_cont<deque<bool> >             ("deque_bool",               "{true,                   false,              true}");
  result += _test_cont<deque<int> >              ("deque_int",                "{11,                     222,               3333}");
  result += _test_cont<deque<int> >              ("deque_int_eval",           "{1*11,                   2*111,           3*1111}");
  result += _test_cont<deque<float> >            ("deque_float",              "{1.11,                   22.22,             333.33}");
  result += _test_cont<deque<float> >            ("deque_float_eval",         "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  result += _test_cont<deque<double> >           ("deque_double",             "{1.11,                   22.22,             333.33}");
  result += _test_cont<deque<double> >           ("deque_double_eval",        "{1.11*GeV,               22.22*MeV,         333.3*TeV}");
  cout << "+----------------------------------------------------------------------------------------------------------" << endl;
#endif
  if ( 0 == result )
    cout << endl << "Test PASSED" << endl << endl;
  else
    cout << endl << "Test FAILED" << endl << "===> " << result << " Subtests FAILED" << endl;
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_property_test,property_test)
