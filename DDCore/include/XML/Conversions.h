// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4hep_COMPACT_CONVERSION_H
#define DD4hep_COMPACT_CONVERSION_H

// C/C++ include files
#include <map>
#include <iostream>

// Framework include files
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {
    class Handle_t;
  }

  /// Basic conversion objects for handling DD4hep XML files.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \ingroup DD4HEP_XML
   */
  template <typename T, typename ARG=XML::Handle_t> struct Converter {
    typedef T to_type;
    typedef void* user_param;
    /// Reference to the detector description object
    Geometry::LCDD& lcdd;
    /// Reference to optional user defined parameter
    user_param param;
    user_param optional;
    /// Initializing constructor of the functor
    Converter(Geometry::LCDD& l) : lcdd(l), param(0), optional(0) { }
    /// Initializing constructor of the functor with initialization of the user parameter
    Converter(Geometry::LCDD& l, user_param p) : lcdd(l), param(p), optional(0) { }
    /// Initializing constructor of the functor with initialization of the user parameter
    Converter(Geometry::LCDD& l, user_param p, user_param o) : lcdd(l), param(p), optional(o)  { }
    /// Callback operator to be specialized depending on the element type
    void operator()(ARG handle) const;
    /// Typed access to the 1rst. user parameter (unchecked)
    template <typename TYPE> TYPE* _param() const  {    return (TYPE*) param;     }
    /// Typed object access to the 1rst. user parameter (unchecked)
    template <typename TYPE> TYPE& _object() const {    return *(TYPE*) param;    }
    /// Typed access to the 2nd. user parameter (unchecked)
    template <typename TYPE> TYPE* _option() const {    return (TYPE*) optional;  }
  };
} /* End namespace DD4hep           */
#endif    /* DD4hep_COMPACT_CONVERSION_H    */
