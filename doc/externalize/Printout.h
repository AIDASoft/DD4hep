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
#ifndef DD4HEP_DDCORE_XML_PRINTOUT_H
#define DD4HEP_DDCORE_XML_PRINTOUT_H

// C/C++ include files
#include <cstdarg>
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
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
  /// Access the current printer level
  PrintLevel printLevel();

  /// Set new print level. Returns the old print level
  PrintLevel setPrintLevel(PrintLevel new_level);

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
  /// Calls the display action with ERROR and throws an std::runtime_error exception
  /**
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int except(const char* src, const char* fmt, ...);
}

#endif   /* DD4HEP_DDCORE_XML_PRINTOUT_H  */
