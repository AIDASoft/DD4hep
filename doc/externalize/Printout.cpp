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
#include "XML/Printout.h"
#include <stdexcept>

// Disable some diagnostics for ROOT dictionaries
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wvarargs"
#endif

using namespace std;

namespace {
  string print_fmt = "%-16s %5s %s";
  DD4hep::PrintLevel print_lvl = DD4hep::INFO;
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
}

/** Calls the display action
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
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
 *  \arg severity   [int,read-only]      Display severity flag
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
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

/** Calls the display action with ERROR and throws an std::runtime_error exception
 *  \arg src        [string,read-only]   Information source (component, etc.)
 *  \arg fmt        [string,read-only]   Format string for ellipsis args
 *  \return Status code indicating success or failure
 */
int DD4hep::except(const char* src, const char* fmt, ...) {
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
