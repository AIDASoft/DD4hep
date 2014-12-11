// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_PLUGINS_H
#define DD4HEP_PLUGINS_H

// ROOT include files
#ifndef __CINT__
#include "Reflex/PluginService.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  typedef ROOT::Reflex::PluginService PluginService;

  /// Helper to debug plugin manager calls
  /** 
   *  Small helper class to adjust the plugin service debug level
   *  for a limited code scope. Automatically back-adjusts the debug
   *  level at object destruction.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct PluginDebug {
    int m_debug;
    /// Default constructor
    PluginDebug(int dbg = 2);
    /// Default destructor
    ~PluginDebug();
    /// Helper to check factory existence
    std::string missingFactory(const std::string& name) const;
  };

} /* End namespace DD4hep      */

#endif    /* __CINT__          */
#endif    /* DD4HEP_PLUGINS_H  */
