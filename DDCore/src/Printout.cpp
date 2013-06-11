// $Id: Geant4Converter.cpp 588 2013-06-03 11:41:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Printout.h"
#include <cstdarg>

static size_t _the_printer(void*, DD4hep::PrintLevel, const char* src, const char* text)  {
  size_t len = ::fputs(src,stdout);
  len += fputs(": ",stdout);
  len += fputs(text,stdout);
  ::fflush(stdout);
  return len;
}

static DD4hep::PrintLevel        print_lvl  = DD4hep::INFO;
static void*                     print_arg  = 0;
static DD4hep::output_function_t print_func = _the_printer;

/** Calls the display action
 *  @arg severity   [int,read-only]      Display severity flag
 *  @arg fmt        [string,read-only]   Format string for ellipsis args
 *  @return Status code indicating success or failure
 */
int DD4hep::printout(PrintLevel severity, const char* src, const char* fmt, ...)   {
  if ( severity >= print_lvl ) {                  // receives:
    va_list args;                                // - the log level
    va_start( args, fmt);                        // - a standard C formatted 
    char str[4096];                              //   string (like printf)
    size_t len = vsnprintf(str,sizeof(str)-2,fmt,args);
    va_end (args);
    str[len]   = '\n';
    str[len+1] = '\0';
    print_func(print_arg,severity,src,str);
  }
  return 1;
}


  /// Set new print level. Returns the old print level
DD4hep::PrintLevel DD4hep::setPrintLevel(PrintLevel new_level)    {
  PrintLevel old = print_lvl;
  print_lvl = new_level;
  return old;
}

/// Customize printer function
void DD4hep::setPrinter(void* arg, output_function_t fcn)   {
  print_arg = arg;
  print_func = fcn;
}

