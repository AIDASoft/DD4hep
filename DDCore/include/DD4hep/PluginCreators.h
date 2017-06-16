//==========================================================================
//  AIDA Detector description implementation 
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
namespace dd4hep {
  
  // Forward declarations
  class Detector;

  /// Handler for factories of type: ConstructionFactory
  /** Signature: void* create(const char* arg);
   *
   *  An exception is thrown in the event the object cannot be created.
   *  The object is properly casted before given to the caller.
   */
  void* createPlugin(const std::string& factory, Detector& description, void* (*cast)(void*));
  void* createPlugin(const std::string& factory, Detector& description, const std::string& arg, void* (*cast)(void*));
  void* createPlugin(const std::string& factory, Detector& description, int argc, char** argv, void* (*cast)(void*));
  void* createProcessor(Detector& description, int argc, char** argv, void* (*cast)(void*));

  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createPlugin(const std::string& factory, Detector& description)   {
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<T&>(*(T*)p); } };
    return (T*)createPlugin(factory, description, __cast::cast);
  }
  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createPlugin(const std::string& factory, Detector& description, const std::string& arg)   {
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<T&>(*(T*)p); } };
    return (T*)createPlugin(factory, description, arg, __cast::cast);
  }
  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createPlugin(const std::string& factory, Detector& description, int argc, const void** argv)   {
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<T&>(*(T*)p); } };
    return (T*)createPlugin(factory, description, argc, (char**)argv, __cast::cast);
  }

  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createProcessor(Detector& description, int argc, char** argv)   {
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<T&>(*(T*)p); } };
    return (T*)createProcessor(description, argc, argv, __cast::cast);
  }

  /// Handler for factories of type: ConstructionFactory with casted return type
  template <typename T> T* createProcessor(Detector& description, int argc, const void** argv)   {
    struct __cast{ static void* cast(void* p) { return &dynamic_cast<T&>(*(T*)p); } };
    return (T*)createProcessor(description, argc, (char**)argv, __cast::cast);
  }

} /* End namespace dd4hep      */

#endif // DD4HEP_PLUGINCREATORS_H
