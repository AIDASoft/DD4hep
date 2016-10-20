//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-01-29
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_PLUGINCREATORS_H 
#define DD4HEP_PLUGINCREATORS_H 1

// Framework include files
#include "DD4hep/Primitives.h"

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    // Forward declarations
    class LCDD;
  }

  /// Handler for factories of type: ConstructionFactory
  /** Signature: void* create(const char* arg);
   *
   *  An exception is thrown in the event the object cannot be created.
   *  The object is properly casted before given to the caller.
   */
  void* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, void* (*cast)(void*));
  void* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, const std::string& arg, void* (*cast)(void*));
  void* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, int argc, char** argv, void* (*cast)(void*));

  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createPlugin(const std::string& factory, Geometry::LCDD& lcdd)   {
    typedef T plugin_t;
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<plugin_t&>(*(plugin_t*)p); } };
    return (plugin_t*)createPlugin(factory, lcdd, __cast::cast);
  }
  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, const std::string& arg)   {
    typedef T plugin_t;
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<plugin_t&>(*(plugin_t*)p); } };
    return (plugin_t*)createPlugin(factory, lcdd, arg, __cast::cast);
  }
  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createPlugin(const std::string& factory, Geometry::LCDD& lcdd, int argc, const void** argv)   {
    typedef T plugin_t;
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<plugin_t&>(*(plugin_t*)p); } };
    return (plugin_t*)createPlugin(factory, lcdd, argc, (char**)argv, __cast::cast);
  }

} /* End namespace DD4hep      */

#endif // DD4HEP_PLUGINCREATORS_H
