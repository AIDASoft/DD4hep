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

// Framework includes
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"
#include "DD4hep/World.h"

using namespace dd4hep;
using namespace dd4hep::detail;

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
  
  template <typename T> class base_1: virtual public base_0   {
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
  
  class payload : virtual public base_1<int>, virtual public base_2<double>  {
  public:
    payload() = default;
    payload(payload&& copy) = default;
    payload(const payload& copy) = default;
    virtual ~payload() = default;
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
static int test_example (Detector& /* description */, int /* argc */, char** /* argv */)  {
  using namespace std;
  
  payload p;
  p.data_0 = 12345;
  p.data_1 = 54321;
  p.data_2 = 2e1;

  payload* pl = &p;
  base_0*  b0 = pl;
  base_1<int>* b1 = pl;
  base_2<double>* b2 = pl;
  cout << "Payload: " << (void*)pl
       << " b0: " << (void*)b0
       << " b1: " << (void*)b1
       << " b2: " << (void*)b2
       << endl;

  b0 = (base_0*)(Cast::instance<payload>().apply_dynCast(Cast::instance<base_0>(), pl));
  cout << "Payload(payload -> base_0): " << (void*)b0 << "  data: " << b0->data_0 << endl;

#if 0
  pl = (payload*)(Cast::instance<base_0>().apply_dynCast(Cast::instance<payload>(), b0));
  if ( pl )   {
    cout << "Payload(base_0 -> payload): " << (void*)pl << "  data: " << pl->data_0 << endl;
  }
  pl = &p;
#endif
  
  b1 = (base_1<int>*)(Cast::instance<payload>().apply_dynCast(Cast::instance<base_1<int> >(), pl));
  cout << "Payload(payload -> base_1): " << (void*)b1 << "  data: " << b1->data_1 << endl;

  b2 = (base_2<double>*)(Cast::instance<payload>().apply_dynCast(Cast::instance<base_2<double> >(), pl));
  cout << "Payload(payload -> base_2): " << (void*)b2 << "  data: " << b2->data_2 << endl;

  try   {
    b0 = (base_0*)(Cast::instance<base_2<double> >().apply_dynCast(Cast::instance<base_0>(), b2));
    cout << "Payload(base_2 -> base_0): " << (void*)b2 << " -> " << (void*)b0 << "  data: " << b0->data_0 << endl;

    b2 = (base_2<double>*)(Cast::instance<base_0>().apply_dynCast(Cast::instance<base_2<double> >(), b0));
    cout << "Payload(base_0 -> base_2): " << (void*)b0 << " -> " << (void*)b2 << "  data: " << b2->data_2 << endl;

    b0 = (base_0*)(Cast::instance<base_1<int> >().apply_dynCast(Cast::instance<base_0>(), b1));
    cout << "Payload(base_1 -> base_0): " << (void*)b2 << " -> " << (void*)b0 << "  data: " << b0->data_0 << endl;

    b1 = (base_1<int>*)(Cast::instance<base_0>().apply_dynCast(Cast::instance<base_1<int> >(), b0));
    cout << "Payload(base_0 -> base_1): " << (void*)b0 << " -> " << (void*)b1 << "  data: " << b1->data_1 << endl;

    /// Cross cast fails:
    //b1 = (base_1<int>*)(Cast::instance<base_2<double> >().apply_dynCast(Cast::instance<base_1<int> >(), b2));
    //cout << "Payload(base_2 -> base_1): " << (void*)b1 << "  data: " << b1->data_1 << endl;
  }
  catch(const exception& e)   {
    cout << "bad cast: " << e.what() << endl;
  }

  cout << "Test PASSED" << endl;
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_test_ABI_cast,test_example)
