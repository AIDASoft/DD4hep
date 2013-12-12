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
#endif

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  typedef ROOT::Reflex::PluginService PluginService;

  /** @class PluginDebug. PluginDebug.h DD4hep/PluginDebug.h
   *
   *  Small helper class to adjust the plugin service debug level
   *  for a limited code scope. Automatically back-adjusts the debug
   *  level at object destruction.
   *
   *  @author  M.Frank
   *  @version 1.0
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

} /* End namespace DD4hep    */
#endif    /* DD4HEP_PLUGINS_H        */
