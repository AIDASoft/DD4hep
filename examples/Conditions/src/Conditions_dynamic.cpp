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
/* 
   Plugin invocation:
   ==================
   This plugin behaves like a main program.
   Invoke the plugin with something like this:

   geoPluginRun -volmgr -destroy -plugin DD4hep_ConditionExample_populate \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml

   Populate the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/

namespace  {
  

  class base_0   {
  public:
    int data_0 {0};
    base_0() = default;
    base_0(base_0&& copy) = default;
    base_0(const base_0& copy) = default;
    virtual ~base_0() = default;
    base_0& operator=(base_0&& copy) = default;
    base_0& operator=(const base_0& copy) = default;
  };
  
  template <typename T> class base_1   {
  public:
    T data_1;
    base_1() = default;
    base_1(base_1&& copy) = default;
    base_1(const base_1& copy) = default;
    virtual ~base_1() = default;
    base_1& operator=(base_1&& copy) = default;
    base_1& operator=(const base_1& copy) = default;
  };
  template <typename T> class base_2: virtual public base_0   {
  public:
    T data_2;
    base_2() = default;
    base_2(base_2&& copy) = default;
    base_2(const base_2& copy) = default;
    virtual ~base_2() = default;
    base_2& operator=(base_2&& copy) = default;
    base_2& operator=(const base_2& copy) = default;
  };
  
  class payload : public base_1<int>, public base_2<double>  {
  public:
    payload() = default;
    payload(payload&& copy) = default;
    payload(const payload& copy) = default;
    virtual ~payload() = default;
    payload& operator=(payload&& copy) = default;
    payload& operator=(const payload& copy) = default;
  };
}

// Framework include files
#include "ConditionExampleObjects.h"
#include "DD4hep/GrammarUnparsed.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_Conditions_dynamic
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Detector& /* description */, int /* argc */, char** /* argv */)  {
#define DATA_0  12345
#define DATA_1  54321
#define DATA_2  2.2222e2

  Condition cond("Cond","payload");
  payload&  p = cond.bind<payload>();

  BasicGrammar::instance<payload>().setCast( &Cast::instance<payload>() );
  BasicGrammar::instance<base_0>().setCast(  &Cast::instance<base_0>() );
  BasicGrammar::instance<base_1<int> >().setCast( &Cast::instance<base_1<int> >() );
  BasicGrammar::instance<base_2<double> >().setCast( &Cast::instance<base_2<double> >() );
  
  p.data_0 = DATA_0;
  p.data_1 = DATA_1;
  p.data_2 = DATA_2;
  {
    payload* pl = &p;
    base_0*  b0 = pl;
    base_1<int>* b1 = pl;
    base_2<double>* b2 = pl;
    cout << "Payload: " << (void*)pl
	 << " b0: " << (void*)b0
	 << " b1: " << (void*)b1
	 << " b2: " << (void*)b2
	 << endl;
  }
  const auto& pl = cond.as<payload>();
  cout << "Payload: " << pl.data_1 << "  " << pl.data_2 << endl;
  if ( pl.data_0 != DATA_0 || pl.data_1 != DATA_1 || pl.data_2 != DATA_2 ) cout << "Test FAILED" << endl;

  const auto& b0 = cond.as<base_0>();
  cout << "Payload(base_0): " << (void*)&b0 << "  data: " << b0.data_0 << endl;
  if ( b0.data_0 != DATA_0 ) cout << "Test FAILED" << endl;

  const auto& b1 = cond.as<base_1<int> >();
  cout << "Payload(base_1): " << (void*)&b1 << "  data: " << b1.data_1 << endl;
  if ( b1.data_1 != DATA_1 ) cout << "Test FAILED" << endl;

  const auto& b2 = cond.as<base_2<double> >();
  cout << "Payload(base_2): " << (void*)&b2 << "  data: " << b2.data_2 << endl;
  if ( b2.data_2 != DATA_2 ) cout << "Test FAILED" << endl;

  delete cond.ptr();
  cout << "Test PASSED" << endl;

  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_Conditions_dynamic,condition_example)
