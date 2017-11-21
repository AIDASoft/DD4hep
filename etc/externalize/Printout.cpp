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
#include "XML/Printout.h"
#include <stdexcept>

// Disable some diagnostics for ROOT dictionaries
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wvarargs"
#endif

using namespace std;

namespace {
  string print_fmt = "%-16s %5s %s";
  dd4hep::PrintLevel print_lvl = dd4hep::INFO;
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
  size_t print_out(dd4hep::PrintLevel lvl, const char* src, const char* fmt, va_list& args) {
    char text[4096];
    ::snprintf(text,sizeof(text),print_fmt.c_str(),src,print_level(lvl),fmt);
    size_t len = ::vfprintf(stdout, text, args);
    ::fputc('\n',stdout);
    return len;
  }
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
    print_out(severity, src, fmt, args);
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
    print_out(severity, src.c_str(), fmt, args);
    va_end(args);
  }
  return 1;
}

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int dd4hep::except(const char* src, const char* fmt, ...) {
  char str[4096];
  va_list args;
  va_start(args, fmt);
  size_t len1 = ::snprintf(str, sizeof(str), "%s: ", src);
  size_t len2 = ::vsnprintf(str + len1, sizeof(str) - len1, fmt, args);
  if ( len2 > sizeof(str) - len1 )  {
    len2 = sizeof(str) - len1 - 1;
    str[sizeof(str)-1] = 0;
  }
  printout(FATAL,src,fmt,args);
  va_end(args);
  throw runtime_error(str);
}
