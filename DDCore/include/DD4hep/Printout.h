// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_PRINTOUT_H
#define DD4HEP_PRINTOUT_H

// C/C++ include files
#include <cstdio>
#include <cstdlib>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  
  enum PrintLevel {
    NOLOG=0,
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    ALWAYS
  };

  typedef size_t (*output_function_t)(void*, PrintLevel severity, const char*, const char*);

  /** Calls the display action
   *  @arg severity   [int,read-only]      Display severity flag (see enum)
   *  @arg src        [string,read-only]   Information source (component, etc.)
   *  @arg fmt        [string,read-only]   Format string for ellipsis args
   *  @return Status code indicating success or failure
   */
  int printout(PrintLevel severity, const char* src, const char* fmt, ...);

  /// Set new print level. Returns the old print level
  PrintLevel setPrintLevel(PrintLevel new_level);

  /// Customize printer function
  void setPrinter(void* print_arg, output_function_t fcn);

}         /* End namespace DD4hep    */
#endif    /* DD4HEP_PRINTOUT_H         */
