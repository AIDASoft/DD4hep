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
#ifndef DDDIGI_DIGIMONITORHANDLER_H
#define DDDIGI_DIGIMONITORHANDLER_H

/// Framework include files
#include <DDDigi/DigiAction.h>

/// C/C++ include files
class TNamed;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiMonitorHandler;

    /// Class to execute non-reentrant subhandler in wrapped mode
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiMonitorHandler : public DigiAction {
    protected:
      /// Property: output file name if monitor saving is desired (delegated to kernel)
      std::string m_output_file;

      /// Map of monitoring items
      std::map<DigiAction*, std::set<TNamed*> > m_monitors;

    public:
      /// Standard constructor
      DigiMonitorHandler(const kernel_t& kernel, const std::string& nam);
      /// Standard destructor
      virtual ~DigiMonitorHandler();
      /// Adopt monitor and keep reference for saving
      void adopt(DigiAction* source, TNamed* object);
      /// Save monitors
      void save();
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIMONITORHANDLER_H
