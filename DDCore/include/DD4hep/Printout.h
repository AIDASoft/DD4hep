// $Id: $
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
#ifndef DD4HEP_PRINTOUT_H
#define DD4HEP_PRINTOUT_H

// Framework include files

// C/C++ include files
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <map>
#include <string>
#include <iostream>

/// Forward declarations
class TNamed;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class NamedObject;
  template <typename T> class Handle;
  typedef Handle<NamedObject> Ref_t;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class VisAttr;
    class DetElement;
    class PlacedVolume;
  }

  enum PrintLevel {
    NOLOG = 0, VERBOSE=1, DEBUG=2, INFO=3, WARNING=4, ERROR=5, FATAL=6, ALWAYS
  };

#ifndef __CINT__
  typedef size_t (*output_function_t)(void*, PrintLevel severity, const char*, const char*);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const char* fmt, ...);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const char* fmt, ...);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const std::string& fmt, ...);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const std::string& fmt, ...);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const char* fmt, va_list& args);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const char* fmt, va_list& args);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const std::string& src, const std::string& fmt, va_list& args);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const std::string& fmt, va_list& args);

  /** Calls the display action with ERROR and throws an std::runtime_error exception
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  void except(const std::string& src, const std::string& fmt, ...);

  /** Calls the display action with ERROR and throws an std::runtime_error exception
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  void except(const char* src, const char* fmt, ...);

  /** Calls the display action with ERROR and throws an std::runtime_error exception
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  void except(const std::string& src, const std::string& fmt, va_list& args);

  /** Calls the display action with ERROR and throws an std::runtime_error exception
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  void except(const char* src, const char* fmt, va_list& args);

  /** Build formatted string
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  std::string format(const std::string& src, const std::string& fmt, ...);

  /** Build exception string
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  std::string format(const char* src, const char* fmt, ...);

  /** Build formatted string
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  std::string format(const std::string& src, const std::string& fmt, va_list& args);

  /** Build exception string and throw std::runtime_error
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
   *  @return Status code indicating success or failure
   */
  std::string format(const char* src, const char* fmt, va_list& args);

  /// Customize printer function
  void setPrinter(void* print_arg, output_function_t fcn);

#endif // __CINT__

  /// Set new printout format for the 3 fields: source-level-message. All 3 are strings
  std::string setPrintFormat(const std::string& new_format);

  /// Set new print level. Returns the old print level
  PrintLevel setPrintLevel(PrintLevel new_level);

  /// Access the current printer level
  PrintLevel printLevel();

  /// Helper class template to implement ASCII object dumps
  /** @class Printer Conversions.h  DD4hep/compact/Conversions.h
   *
   *  Small helper class to print objects
   *
   *  @author   M.Frank
   *  @version  1.0
   */
  template <typename T> struct Printer {
    /// Reference to the detector description object
    const Geometry::LCDD* lcdd;
    /// Reference to the output stream object, the Printer object should write
    std::ostream& os;
    /// Optional text prefix when formatting the output
    std::string prefix;
    /// Initializing constructor of the functor
    Printer(const Geometry::LCDD* l, std::ostream& stream, const std::string& p = "")
      : lcdd(l), os(stream), prefix(p) {
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
  /** @class PrintMap Conversions.h  DD4hep/compact/Conversions.h
   *
   *  Small helper class to print maps of objects
   *
   *  @author   M.Frank
   *  @version  1.0
   */
  template <typename T> struct PrintMap {
    typedef T item_type;
    typedef const std::map<std::string, Ref_t> cont_type;

    /// Reference to the detector description object
    const Geometry::LCDD* lcdd;
    /// Reference to the output stream object, the Printer object should write
    std::ostream& os;
    /// Optional text prefix when formatting the output
    std::string text;
    /// Reference to the container data of the map.
    cont_type& cont;
    /// Initializing constructor of the functor
    PrintMap(const Geometry::LCDD* l, std::ostream& stream, cont_type& c, const std::string& t = "")
      : lcdd(l), os(stream), text(t), cont(c) {
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

} /* End namespace DD4hep      */
#endif    /* DD4HEP_PRINTOUT_H         */
