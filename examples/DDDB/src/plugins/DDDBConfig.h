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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DDDBCONFIG_H
#define DD4HEP_DDDB_DDDBCONFIG_H

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  
  /// Namespace for the DDDB2Objects configuration setup
  namespace CondDB2Objects {

    /// Printout steering for debugging
    class PrintConfig  {
    public:
      bool              xml           = false;
      bool              file_load     = false;
      bool              docs          = false;
      bool              materials     = false;
      bool              logvol        = false;
      bool              shapes        = false;
      bool              physvol       = false;
      bool              params        = false;
      bool              detelem       = false;
      bool              detelem_ref   = false;
      bool              detelem_xml   = false;
      bool              condition     = false;
      bool              condition_ref = false;
      bool              catalog       = false;
      bool              catalog_ref   = false;
      bool              tabprop       = false;
      bool              tree_on_error = true;
      bool              eval_error    = true;
      /// Access global instance for xml configuration
      static PrintConfig& instance();
    };
  }
  
  /// Namespace for the DDDB2Objects configuration setup
  namespace DDDB2Objects {

    /// Printout steering for debugging
    class PrintConfig  {
    public:
      bool              materials  = false;
      bool              volumes    = false;
      bool              logvol     = false;
      bool              shapes     = false;
      bool              physvol    = false;
      bool              params     = false;
      bool              detelem    = false;
      bool              condition  = false;
      bool              vis        = false;
      int               max_volume_depth = 12;
      /// Access global instance for xml configuration
      static PrintConfig& instance();
    };
  }
}
#endif /*  DD4HEP_DDDB_DDDBCONFIG_H  */
  
