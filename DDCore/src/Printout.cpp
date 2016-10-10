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

// Framework include files
#include "DD4hep/Printout.h"

// C/C++ include files
#include <cstdarg>
#include <stdexcept>

using namespace std;

namespace {
  size_t _the_printer_1(void*, DD4hep::PrintLevel lvl, const char* src, const char* text);
  size_t _the_printer_2(void* par, DD4hep::PrintLevel lvl, const char* src, const char* fmt, va_list& args);

  std::string print_fmt = "%-16s %5s %s";
  DD4hep::PrintLevel print_lvl = DD4hep::INFO;
  void* print_arg = 0;
  DD4hep::output_function1_t print_func_1 = 0;
  DD4hep::output_function2_t print_func_2 = _the_printer_2;

  const char* print_level(DD4hep::PrintLevel lvl)   {
    switch(lvl)   {
    case DD4hep::NOLOG:     return "NOLOG";
    case DD4hep::VERBOSE:   return "VERB ";
    case DD4hep::DEBUG:     return "DEBUG";
    case DD4hep::INFO:      return "INFO ";
    case DD4hep::WARNING:   return "WARN ";
    case DD4hep::ERROR:     return "ERROR";
    case DD4hep::FATAL:     return "FATAL";
    case DD4hep::ALWAYS:    return "     ";
    default:
      if ( lvl> DD4hep::ALWAYS )
	return print_level(DD4hep::ALWAYS);
      return print_level(DD4hep::NOLOG);
    }
  }

  size_t _the_printer_1(void*, DD4hep::PrintLevel lvl, const char* src, const char* text) {
    size_t len = ::fprintf(stdout, print_fmt.c_str(), src, print_level(lvl), text);
    ::fflush(stdout);
    return len;
  }

  size_t _the_printer_2(void* par, DD4hep::PrintLevel lvl, const char* src, const char* fmt, va_list& args) {
    if ( !print_func_1 )  {
      char text[4096];
      ::snprintf(text,sizeof(text),print_fmt.c_str(),src,print_level(lvl),fmt);
      size_t len = ::vfprintf(stdout, text, args);
      ::fflush(stdout);
      return len;
    }
    char str[4096];
    ::vsnprintf(str, sizeof(str), fmt, args);
    return print_func_1(par, lvl, src, str);
  }

  string __format(const char* fmt, va_list& args) {
    char str[4096];
    ::vsnprintf(str, sizeof(str), fmt, args);
    return string(str);
  }
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const char* fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, fmt);
    printout(severity, src, fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const string& src, const char* fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, fmt);
    printout(severity, src.c_str(), fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const string& fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, &fmt);
    printout(severity, src, fmt.c_str(), args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const string& src, const string& fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, &fmt);
    printout(severity, src.c_str(), fmt.c_str(), args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const char* fmt, va_list& args) {
  if (severity >= print_lvl) {
    print_func_2(print_arg, severity,src,fmt,args);
  }
  return 1;
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const string& src, const char* fmt, va_list& args) {
  return printout(severity, src.c_str(), fmt, args);
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const string& fmt, va_list& args) {
  return printout(severity, src, fmt.c_str(), args);
}

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const string& src, const string& fmt, va_list& args) {
  return printout(severity, src.c_str(), fmt.c_str(), args);
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
void DD4hep::except(const string& src, const string& fmt, ...) {
  va_list args;
  va_start(args, &fmt);
  except(src.c_str(),fmt.c_str(), args);
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
void DD4hep::except(const char* src, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  except(src, fmt, args);
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  @return Status code indicating success or failure
 */
void DD4hep::except(const string& src, const string& fmt, va_list& args) {
  string msg = __format(fmt.c_str(), args);
  va_end(args);
  printout(ERROR, src.c_str(), "%s", msg.c_str());
  // No return. Must call va_end here!
  throw runtime_error((src+": "+msg).c_str());
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  @return Status code indicating success or failure
 */
void DD4hep::except(const char* src, const char* fmt, va_list& args) {
  string msg = format(src, fmt, args);
  va_end(args);
  printout(ERROR, src, "%s", msg.c_str());
  // No return. Must call va_end here!
  throw runtime_error((string(src)+": "+msg).c_str());
}

/** Build exception string
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
string DD4hep::format(const string& src, const string& fmt, ...) {
  va_list args;
  va_start(args, &fmt);
  string str = format(src, fmt, args);
  va_end(args);
  return str;
}

/** Build exception string
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
string DD4hep::format(const char* src, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  string str = format(src, fmt, args);
  va_end(args);
  return str;
}

/** Build exception string and throw std::runtime_error
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  @return Status code indicating success or failure
 */
string DD4hep::format(const string& src, const string& fmt, va_list& args) {
  return format(src.c_str(), fmt.c_str(), args);
}

/** Build exception string and throw std::runtime_error
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  @return Status code indicating success or failure
 */
string DD4hep::format(const char* src, const char* fmt, va_list& args) {
  char str[4096];
  size_t len1 = ::snprintf(str, sizeof(str), "%s: ", src);
  size_t len2 = ::vsnprintf(str + len1, sizeof(str) - len1, fmt, args);
  if ( len2 > sizeof(str) - len1 )  {
    len2 = sizeof(str) - len1 - 1;
    str[sizeof(str)-1] = 0;
  }
  return string(str);
}

/// Set new print level. Returns the old print level
DD4hep::PrintLevel DD4hep::setPrintLevel(PrintLevel new_level) {
  PrintLevel old = print_lvl;
  print_lvl = new_level;
  return old;
}

/// Access the current printer level
DD4hep::PrintLevel DD4hep::printLevel()  {
  return print_lvl;
}

/// Set new printout format for the 3 fields: source-level-message. All 3 are strings
string DD4hep::setPrintFormat(const string& new_format) {
  string old = print_fmt;
  print_fmt  = new_format;
  return old;
}

/// Customize printer function
void DD4hep::setPrinter(void* arg, output_function1_t fcn) {
  print_arg  = arg;
  print_func_1 = fcn ? fcn : _the_printer_1;
}

/// Customize printer function
void DD4hep::setPrinter2(void* arg, output_function2_t fcn) {
  print_arg = arg;
  print_func_2 = fcn ? fcn : _the_printer_2;
}
