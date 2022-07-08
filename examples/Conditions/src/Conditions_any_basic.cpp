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
#include <DD4hep/ConditionAny.h>
#include <DD4hep/Factories.h>
#include <iostream>

using namespace std;
using namespace dd4hep;

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
    virtual ~payload()  { cout << "payload destroyed..." << endl; }
    payload& operator=(payload&& copy) = default;
    payload& operator=(const payload& copy) = default;
  };
}

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_Conditions_dynamic
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_any_basic (Detector& /* description */, int /* argc */, char** /* argv */)  {
  ConditionAny c1("any_cond","payload");
  Condition    c2("vector_cond","std::vector<int>");

  cout << endl << endl;
  cout << "Size std::any:      " << sizeof(std::any) << endl;
  cout << "Size std::vector:   " << sizeof(std::vector<int>) << endl;
  if ( sizeof(std::any) > sizeof(std::vector<int>) )  {
    cout << endl << "Test FAILED" << endl << endl;
    return EINVAL;
  }
  cout << "Payload test PASSED" << endl << endl;

  c1.get() = std::make_any<payload>();
  auto& v2 = c2.bind<std::vector<int> >();
  v2.push_back(1);
  v2.push_back(2);

  cout << "c1 : " << c1.any_type().name() << endl;

  // Assign incompatible exception to any condition:
  try  {
    c1 = c2;
    cout << "Assigned:  ConditionAny = Condition(vector) . " << endl;
  }
  catch(const std::exception& e)   {
    cout << "Expected exception: ConditionAny = Condition(vector) : " << e.what() << endl;
  }

  // Assign any to condition:
  Condition cc(c2);
  c2 = c1;
  cout << "Assigned: Condition(vector) = ConditionAny ." << endl;
  c2 = cc; // Restore value!

  Condition c3(c1);
  cout << "Construct c3: Condition( Condition(any) )   Pointer: "
       << (void*)c3.ptr() << endl;

  ConditionAny c4(c3);
  cout << "Construct c4: Condition(any)( ConditionAny ) Pointer: "
       << (void*)c4.ptr() << " type:" << c4.any_type().name() << endl;

  try  {
    ConditionAny c5(c2);
    cout << "Construct c5: ConditionAny( Condition(vector) ) Pointer: " 
	 << (void*)c5.ptr() << " type:" << c5.any_type().name() << endl;
  }
  catch(const std::exception& e)   {
    cout << "Expected exception: Construct c5: ConditionAny( Condition(vector) ) : " << e.what() << endl;
  }

  c1.destroy();
  c2.destroy();

  cout << endl << "Test PASSED" << endl << endl;
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_Conditions_any_basic,condition_any_basic)
