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
#ifndef DDDIGI_DIGIOUTPUTACTION_H
#define DDDIGI_DIGIOUTPUTACTION_H

/// Framework include files
#include <DDDigi/DigiEventAction.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiOutputAction;

    /// Base class for output actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiOutputAction : public DigiEventAction {
    public:
      enum { OUTPUT_START = -1  };
      enum { NO_MASK     = 0x0 };

    protected:
      /// Array of data containers to be saved
      std::vector<std::string> m_containers { };
      /// Property: Output data specification
      std::string              m_output { };
      /// Property: Mask to flag output source items
      int                      m_mask   { NO_MASK };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiOutputAction);

    public:
      /// Standard constructor
      DigiOutputAction(const kernel_t& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiOutputAction();

      /// Callback to read event output
      virtual void execute(context_t& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIOUTPUTACTION_H
