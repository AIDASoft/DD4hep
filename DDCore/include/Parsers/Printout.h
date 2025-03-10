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
#ifndef PARSERS_PRINTOUT_H
#define PARSERS_PRINTOUT_H

// Framework include files
#include <Parsers/config.h>

// C/C++ include files
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <sstream>
//#include <iostream>

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
    ALWAYS   = 7,

    /// Forced printout levels if the output level is handled
    /// e.g. by a Geant4Action or DigiAction. These always pass
    /// The default DD4hep print level restrictions.
    FORCE_LEVEL    = 0x10,
    FORCE_VERBOSE  = FORCE_LEVEL + 1,
    FORCE_DEBUG    = FORCE_LEVEL + 2,
    FORCE_INFO     = FORCE_LEVEL + 3,
    FORCE_WARNING  = FORCE_LEVEL + 4,
    FORCE_ERROR    = FORCE_LEVEL + 5,
    FORCE_FATAL    = FORCE_LEVEL + 6,
    FORCE_ALWAYS   = FORCE_LEVEL + 7
  };

#ifndef __CINT__
  typedef size_t (*output_function1_t)(void*, PrintLevel severity, const char*, const char*);
  typedef size_t (*output_function2_t)(void*, PrintLevel severity, const char*, const char*, va_list& args);

  namespace detail {
    /// Multi-thread save, locked printout to stdout
    std::size_t printf(const char* fmt, ...);
    /// Multi-thread save, locked printout to stderr
    std::size_t errprintf(const char* fmt, ...);
  }
  
  /// Helper function to serialize argument list to a single string
  /**
   *  @arg argc       [int,read-only]      Number of arguments.
   *  @arg argv       [char**,read-only]   Argument strings
   *  @return String containing the concatenated arguments
   */
  std::string arguments(int argc, char** argv);

  /// Decode printlevel from string to value
  /**
   *  @arg level_as_string       [string,read-only]      String value of print level
   *  @return Print level as enumeration
   */
  PrintLevel decodePrintLevel(const std::string& level_as_string);
  
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

  /// Helper function to print booleans in format YES/NO
  inline const char* yes_no(bool value) {
    return value ? "YES" : "NO ";
  }
  /// Helper function to print booleans in format true/false
  inline const char* true_false(bool value) {
    return value ? "true " : "false";
  }

}         /* End namespace dd4hep              */
#endif // PARSERS_PRINTOUT_H
