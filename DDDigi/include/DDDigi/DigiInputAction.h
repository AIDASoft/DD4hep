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
#ifndef DDDIGI_DIGIINPUTACTION_H
#define DDDIGI_DIGIINPUTACTION_H

/// Framework include files
#include <DDDigi/DigiEventAction.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiInputAction;

    /// Base class for input actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiInputAction : public DigiEventAction {
    public:
      enum { INPUT_START = -1  };
      enum { NO_MASK     = 0x0 };

    protected:
      /// Property: Input data specification
      std::vector<std::string> m_input_sources { };
      /// Property: Input data segment name
      std::string              m_input_segment { "inputs" };
      /// Property: Mask to flag input source items
      int                      m_input_mask    { NO_MASK };
      /// Property: Loop on inputs and restart at end
      bool                     m_input_rescan { true };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiInputAction);

    public:
      /// Standard constructor
      DigiInputAction(const kernel_t& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiInputAction();

      /// Access to container names containing data
      const std::vector<std::string>& inputs()  const   {
	return m_input_sources;
      }
      /// Access to container names containing data
      const std::string& input_segment()  const   {
	return m_input_segment;
      }
      /// Access to container names containing data
      int input_mask()  const   {
	return m_input_mask;
      }
      /// Callback to read event input
      virtual void execute(context_t& context)  const override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIINPUTACTION_H
