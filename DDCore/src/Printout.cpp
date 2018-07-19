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

// Framework include files
#include "DD4hep/Printout.h"

// C/C++ include files
#include <cstring>
#include <cstdarg>
#include <sstream>
#include <stdexcept>
// Disable some diagnostics for ROOT dictionaries
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wvarargs"
#endif

using namespace std;

namespace {
  size_t _the_printer_1(void*, dd4hep::PrintLevel lvl, const char* src, const char* text);
  size_t _the_printer_2(void* par, dd4hep::PrintLevel lvl, const char* src, const char* fmt, va_list& args);

  std::string print_fmt = "%-16s %5s %s";
  dd4hep::PrintLevel print_lvl = dd4hep::INFO;
  void* print_arg = 0;
  dd4hep::output_function1_t print_func_1 = 0;
  dd4hep::output_function2_t print_func_2 = _the_printer_2;

  const char* print_level(dd4hep::PrintLevel lvl)   {
    switch(lvl)   {
    case dd4hep::NOLOG:     return "NOLOG";
    case dd4hep::VERBOSE:   return "VERB ";
    case dd4hep::DEBUG:     return "DEBUG";
    case dd4hep::INFO:      return "INFO ";
    case dd4hep::WARNING:   return "WARN ";
    case dd4hep::ERROR:     return "ERROR";
    case dd4hep::FATAL:     return "FATAL";
    case dd4hep::ALWAYS:    return "     ";
    default:
      if ( lvl> dd4hep::ALWAYS )
        return print_level(dd4hep::ALWAYS);
      return print_level(dd4hep::NOLOG);
    }
  }

  size_t _the_printer_1(void*, dd4hep::PrintLevel lvl, const char* src, const char* text) {
    ::fflush(stdout);
    ::fflush(stderr);
    cout << flush;
    cerr << flush;
    size_t len = ::fprintf(stdout, print_fmt.c_str(), src, print_level(lvl), text);
    ::fputc('\n',stdout);
    return len;
  }

  size_t _the_printer_2(void* par, dd4hep::PrintLevel lvl, const char* src, const char* fmt, va_list& args) {
    if ( !print_func_1 )  {
      char text[4096];
      ::fflush(stdout);
      ::fflush(stderr);
      cout << flush;
      cerr << flush;
      ::snprintf(text,sizeof(text),print_fmt.c_str(),src,print_level(lvl),fmt);
      size_t len = ::vfprintf(stdout, text, args);
      ::fputc('\n',stdout);
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

dd4hep::PrintLevel dd4hep::decodePrintLevel(const std::string& val)   {
  switch(::toupper(val[0]))  {
  case '1':
  case 'V':
    return dd4hep::VERBOSE;
  case '2':
  case 'D':
    return dd4hep::DEBUG;
  case '3':
  case 'I':
    return dd4hep::INFO;
  case '4':
  case 'W':
    return dd4hep::WARNING;
  case '5':
  case 'E':
    return dd4hep::ERROR;
  case '6':
  case 'F':
    return dd4hep::FATAL;
  case '7':
  case 'A':
    return dd4hep::FATAL;
  default:
    std::cout << "Unknown print level supplied:'" << val << "'. Argument ignored." << std::endl;
    throw std::runtime_error("Invalid printLevel:"+val);
  }
}

/// Helper function to serialize argument list to a single string
/**
 *  \arg argc       [int,read-only]      Number of arguments.
 *  \arg argv       [char**,read-only]   Argument strings
 *  \return String containing the concatenated arguments
 */
string dd4hep::arguments(int argc, char** argv)   {
  stringstream str;
  for(int i=0; i<argc;)  {
    str << argv[i];
    if ( ++i < argc ) str << " ";
  }
  return str.str();
}  

/// Calls the display action with a given severity level
/**
 *  @arg severity   [int,read-only]      Display severity flag (see enum)
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg str        [stringstream, RW]   string stream containing data to be printed.
 *                                       Object is reset after use.
 *  @return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const char* src, std::stringstream& str)   {
  int ret = 1;
  if (severity >= print_lvl) {
    ret = printout(severity, src, str.str().c_str());
  }
  str.str("");
  return ret;
}

/// Calls the display action with a given severity level
/**
 *  @arg severity   [int,read-only]      Display severity flag (see enum)
 *  @arg src        [string,read-only]   Information source (component, etc.)
 *  @arg str        [stringstream, RW]   string stream containing data to be printed.
 *                                       Object is reset after use.
 *  @return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const std::string& src, std::stringstream& str)   {
  int ret = 1;
  if (severity >= print_lvl) {
    ret = printout(severity, src, str.str().c_str());
  }
  str.str("");
  return ret;
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const char* src, const char* fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, fmt);
    printout(severity, src, fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const string& src, const char* fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, fmt);
    printout(severity, src.c_str(), fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const char* src, const string& fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, &fmt);
    printout(severity, src, fmt.c_str(), args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const string& src, const string& fmt, ...) {
  if (severity >= print_lvl) {
    va_list args;
    va_start(args, &fmt);
    printout(severity, src.c_str(), fmt.c_str(), args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const char* src, const char* fmt, va_list& args) {
  if (severity >= print_lvl) {
    print_func_2(print_arg, severity,src,fmt,args);
  }
  return 1;
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const string& src, const char* fmt, va_list& args) {
  return printout(severity, src.c_str(), fmt, args);
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const char* src, const string& fmt, va_list& args) {
  return printout(severity, src, fmt.c_str(), args);
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::printout(PrintLevel severity, const string& src, const string& fmt, va_list& args) {
  return printout(severity, src.c_str(), fmt.c_str(), args);
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::except(const string& src, const string& fmt, ...) {
  va_list args;
  va_start(args, &fmt);
  return except(src.c_str(),fmt.c_str(), args);
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::except(const char* src, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  return except(src, fmt, args);
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  \return Status code indicating success or failure
 */
int dd4hep::except(const string& src, const string& fmt, va_list& args) {
  string msg = __format(fmt.c_str(), args);
  va_end(args);
  printout(ERROR, src.c_str(), "%s", msg.c_str());
  // No return. Must call va_end here!
  throw runtime_error((src+": "+msg).c_str());
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  \return Status code indicating success or failure
 */
int dd4hep::except(const char* src, const char* fmt, va_list& args) {
  string msg = __format(fmt, args);
  va_end(args);
  printout(ERROR, src, "%s", msg.c_str());
  // No return. Must call va_end here!
  throw runtime_error((string(src)+": "+msg).c_str());
}

/** Build exception string
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
string dd4hep::format(const string& src, const string& fmt, ...) {
  va_list args;
  va_start(args, &fmt);
  string str = format(src, fmt, args);
  va_end(args);
  return str;
}

/** Build exception string
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
string dd4hep::format(const char* src, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  string str = format(src, fmt, args);
  va_end(args);
  return str;
}

/** Build exception string and throw std::runtime_error
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  \return Status code indicating success or failure
 */
string dd4hep::format(const string& src, const string& fmt, va_list& args) {
  return format(src.c_str(), fmt.c_str(), args);
}

/** Build exception string and throw std::runtime_error
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \arg args       [ap_list,read-only]  List with variable number of arguments to fill format string.
 *  \return Status code indicating success or failure
 */
string dd4hep::format(const char* src, const char* fmt, va_list& args) {
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
dd4hep::PrintLevel dd4hep::setPrintLevel(PrintLevel new_level) {
  PrintLevel old = print_lvl;
  print_lvl = new_level;
  return old;
}

/// Access the current printer level
dd4hep::PrintLevel dd4hep::printLevel()  {
  return print_lvl;
}

/// Translate the printer level from string to value
dd4hep::PrintLevel dd4hep::printLevel(const char* value)  {
  if ( !value ) except("Printout","Invalid printlevel requested [EINVAL: Null-pointer argument]");
  // Explicit values
  if ( strcmp(value,"NOLOG")   == 0 ) return dd4hep::NOLOG;
  if ( strcmp(value,"VERBOSE") == 0 ) return dd4hep::VERBOSE;
  if ( strcmp(value,"DEBUG")   == 0 ) return dd4hep::DEBUG;
  if ( strcmp(value,"INFO")    == 0 ) return dd4hep::INFO;
  if ( strcmp(value,"WARNING") == 0 ) return dd4hep::WARNING;
  if ( strcmp(value,"ERROR")   == 0 ) return dd4hep::ERROR;
  if ( strcmp(value,"FATAL")   == 0 ) return dd4hep::FATAL;
  if ( strcmp(value,"ALWAYS")  == 0 ) return dd4hep::ALWAYS;
  // Numeric values
  if ( strcmp(value,"0")       == 0 ) return dd4hep::NOLOG;
  if ( strcmp(value,"1")       == 0 ) return dd4hep::VERBOSE;
  if ( strcmp(value,"2")       == 0 ) return dd4hep::DEBUG;
  if ( strcmp(value,"3")       == 0 ) return dd4hep::INFO;
  if ( strcmp(value,"4")       == 0 ) return dd4hep::WARNING;
  if ( strcmp(value,"5")       == 0 ) return dd4hep::ERROR;
  if ( strcmp(value,"6")       == 0 ) return dd4hep::FATAL;
  if ( strcmp(value,"7")       == 0 ) return dd4hep::ALWAYS;
  except("Printout","Unknown printlevel requested:%s",value);
  return dd4hep::ALWAYS;
}

/// Translate the printer level from string to value
dd4hep::PrintLevel dd4hep::printLevel(const std::string& value)  {
  return printLevel(value.c_str());
}

/// Check if this print level would result in some output
bool dd4hep::isActivePrintLevel(int severity)   {
  return severity >= print_lvl;
}

/// Set new printout format for the 3 fields: source-level-message. All 3 are strings
string dd4hep::setPrintFormat(const string& new_format) {
  string old = print_fmt;
  print_fmt  = new_format;
  return old;
}

/// Customize printer function
void dd4hep::setPrinter(void* arg, output_function1_t fcn) {
  print_arg  = arg;
  print_func_1 = fcn ? fcn : _the_printer_1;
}

/// Customize printer function
void dd4hep::setPrinter2(void* arg, output_function2_t fcn) {
  print_arg = arg;
  print_func_2 = fcn ? fcn : _the_printer_2;
}
