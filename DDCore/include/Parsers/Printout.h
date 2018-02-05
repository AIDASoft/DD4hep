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
#ifndef DD4HEP_PARSERS_PRINTOUT_H
#define DD4HEP_PARSERS_PRINTOUT_H

// Framework include files
#include "Parsers/config.h"

// C/C++ include files
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

/// Forward declarations
class TNamed;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;
  class NamedObject;
  template <typename T> class Handle;

  enum PrintLevel {
    NOLOG    = 0,
    VERBOSE  = 1,
    DEBUG    = 2,
    INFO     = 3,
    WARNING  = 4,
    ERROR    = 5,
    FATAL    = 6,
    ALWAYS   = 7
  };

#ifndef __CINT__
  typedef size_t (*output_function1_t)(void*, PrintLevel severity, const char*, const char*);
  typedef size_t (*output_function2_t)(void*, PrintLevel severity, const char*, const char*, va_list& args);

  /// Helper function to serialize argument list to a single string
  /**
   *  @arg argc       [int,read-only]      Number of arguments.
   *  @arg argv       [char**,read-only]   Argument strings
   *  @return String containing the concatenated arguments
   */
  std::string arguments(int argc, char** argv);
  
  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg str        [stringstream, RW]   string stream containing data to be printed.
   *                                       Object is reset after use.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, std::stringstream& str);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg str        [stringstream, RW]   string stream containing data to be printed.
   *                                       Object is reset after use.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, std::stringstream& str);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const char* fmt, ...);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const char* fmt, ...);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const std::string& fmt, ...);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const std::string& fmt, ...);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const char* fmt, va_list& args);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const char* fmt, va_list& args);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const std::string& fmt, va_list& args);

  /// Calls the display action with a given severity level
  /**
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const std::string& fmt, va_list& args);

  /// Calls the display action with ERROR and throws an std::runtime_error exception
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int except(const std::string& src, const std::string& fmt, ...);

  /// Calls the display action with ERROR and throws an std::runtime_error exception
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int except(const char* src, const char* fmt, ...);

  /// Calls the display action with ERROR and throws an std::runtime_error exception
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int except(const std::string& src, const std::string& fmt, va_list& args);

  /// Calls the display action with ERROR and throws an std::runtime_error exception
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int except(const char* src, const char* fmt, va_list& args);

  /// Build formatted string
  /*
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  std::string format(const std::string& src, const std::string& fmt, ...);

  /// Build exception string
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  std::string format(const char* src, const char* fmt, ...);

  /// Build formatted string
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  std::string format(const std::string& src, const std::string& fmt, va_list& args);

  /// Build formatted string
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  std::string format(const char* src, const char* fmt, va_list& args);

  /// Customize printer function
  void setPrinter(void* print_arg, output_function1_t fcn);

  /// Customize printer function
  void setPrinter2(void* print_arg, output_function2_t fcn);

#endif // __CINT__

  /// Set new printout format for the 3 fields: source-level-message. All 3 are strings
  std::string setPrintFormat(const std::string& new_format);

  /// Set new print level. Returns the old print level
  PrintLevel setPrintLevel(PrintLevel new_level);

  /// Access the current printer level
  PrintLevel printLevel();

  /// Translate the printer level from string to value
  PrintLevel printLevel(const char* value);

  /// Translate the printer level from string to value
  PrintLevel printLevel(const std::string& value);

  /// Check if this print level would result in some output
  bool isActivePrintLevel(int severity);

  /// Helper class template to implement ASCII object dumps
  /** @class Printer Conversions.h  dd4hep/compact/Conversions.h
   *
   *  Small helper class to print objects
   *
   *  @author   M.Frank
   *  @version  1.0
   */
  template <typename T> struct Printer {
    /// Reference to the detector description object
    const Detector* description;
    /// Reference to the output stream object, the Printer object should write
    std::ostream& os;
    /// Optional text prefix when formatting the output
    std::string prefix;
    /// Initializing constructor of the functor
    Printer(const Detector* l, std::ostream& stream, const std::string& p = "")
      : description(l), os(stream), prefix(p) {
    }
    /// Callback operator to be specialized depending on the element type
    void operator()(const T& value) const;
  };

  template <typename T> inline std::ostream& print(const T& object, std::ostream& os = std::cout,
                                                   const std::string& indent = "") {
    Printer<T>(0, os, indent)(object);
    return os;
  }

  /// Helper class template to implement ASCII dumps of named objects maps
  /** @class PrintMap Conversions.h  dd4hep/compact/Conversions.h
   *
   *  Small helper class to print maps of objects
   *
   *  @author   M.Frank
   *  @version  1.0
   */
  template <typename T> struct PrintMap {
    typedef T item_type;
    typedef const std::map<std::string, Handle<NamedObject> > cont_type;

    /// Reference to the detector description object
    const Detector* description;
    /// Reference to the output stream object, the Printer object should write
    std::ostream& os;
    /// Optional text prefix when formatting the output
    std::string text;
    /// Reference to the container data of the map.
    cont_type& cont;
    /// Initializing constructor of the functor
    PrintMap(const Detector* l, std::ostream& stream, cont_type& c, const std::string& t = "")
      : description(l), os(stream), text(t), cont(c) {
    }
    /// Callback operator to be specialized depending on the element type
    void operator()() const;
  };

  /// Helper function to print booleans in format YES/NO
  inline const char* yes_no(bool value) {
    return value ? "YES" : "NO ";
  }
  /// Helper function to print booleans in format true/false
  inline const char* true_false(bool value) {
    return value ? "true " : "false";
  }

}         /* End namespace dd4hep              */
#endif    /* DD4HEP_PARSERS_PRINTOUT_H         */
