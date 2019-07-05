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
#ifndef DD4HEP_ANY_H
#define DD4HEP_ANY_H

#if __cplusplus >= 201703L
// C/C++ include files
#include <any>
/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {
  using std::any;
  using std::any_cast;
  using std::make_any;
}

#else

#include "boost/any.hpp"
/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  using boost::any;
  using boost::any_cast;
  /// Create an any holding a @c _Tp constructed from @c __args.
  template <typename T, typename... _Args>
    any make_any(_Args&&... __args)  {
      return any(T(std::forward<_Args>(__args)...));
    }
}
#endif

#endif  // DD4HEP_ANY_H
