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

/** 
 *  Note: Do NEVER include this file directly!
 *
 *  Use the specific include files in the XML or JSON directory!
 *  Also NO header guards!
 */


// C/C++ include files
#include <map>
#include <iostream>

// Framework include files


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declaration
  class Detector;
  
  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DD4HEP_CONVERSION_NS {
    class Handle_t;
  }

  /// Basic conversion objects for handling dd4hep XML files.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \ingroup DD4HEP_XML
   */
  template <typename T, typename ARG=DD4HEP_CONVERSION_NS::Handle_t> struct Converter {
    typedef T to_type;
    typedef void* user_param;
    /// Reference to the detector description object
    Detector& description;
    /// Reference to optional user defined parameter
    user_param param;
    user_param optional;
    /// Initializing constructor of the functor
    Converter(Detector& l) : description(l), param(0), optional(0) { }
    /// Initializing constructor of the functor with initialization of the user parameter
    Converter(Detector& l, user_param p) : description(l), param(p), optional(0) { }
    /// Initializing constructor of the functor with initialization of the user parameter
    Converter(Detector& l, user_param p, user_param o) : description(l), param(p), optional(o)  { }
    /// Callback operator to be specialized depending on the element type
    void operator()(ARG handle) const;
    /// Typed access to the 1rst. user parameter (unchecked)
    template <typename TYPE> TYPE* _param() const  {    return (TYPE*) param;     }
    /// Typed object access to the 1rst. user parameter (unchecked)
    template <typename TYPE> TYPE& _object() const {    return *(TYPE*) param;    }
    /// Typed access to the 2nd. user parameter (unchecked)
    template <typename TYPE> TYPE* _option() const {    return (TYPE*) optional;  }
  };
} /* End namespace dd4hep           */
