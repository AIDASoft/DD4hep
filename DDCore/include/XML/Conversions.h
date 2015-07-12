// $Id$
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
  template <typename T> struct Converter {
    typedef T to_type;
    typedef void* user_param;
    /// Reference to the detector description object
    Geometry::LCDD& lcdd;
    /// Reference to optional user defined parameter
    user_param param;
    /// Initializing constructor of the functor
    Converter(Geometry::LCDD& l)
      : lcdd(l), param(0) {
    }
    /// Initializing constructor of the functor with initialization of the user parameter
    Converter(Geometry::LCDD& l, user_param p)
      : lcdd(l), param(p) {
    }
    /// Callback operator to be specialized depending on the element type
    void operator()(XML::Handle_t xml) const;
    /// Typed access to the user parameter (unchecked)
    template <typename TYPE> TYPE* _param() const {
      return (TYPE*) param;
    }
    /// Typed object access to the user parameter (unchecked)
    template <typename TYPE> TYPE& _object() const {
      return *(TYPE*) param;
    }
  };
} /* End namespace DD4hep           */
#endif    /* DD4hep_COMPACT_CONVERSION_H    */
