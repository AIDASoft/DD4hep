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

/// C/C++ include files
#include <limits>

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
      /// Most probably need some minimum cutoff:
      static constexpr double epsilon = std::numeric_limits<double>::epsilon();

      /// Input source base
      class input_source   {
      public:
	/// Event counter for current file
	int event_count  { 0 };
      };

      /// Event frame base
      class event_frame   {
      public:
      };

    protected:
      /// Property: Input data specification
      std::vector<std::string> m_input_sources { };
      /// Property: Container names to be loaded
      std::vector<std::string> m_objects_enabled  { };
      /// Property: Container names to be ignored for loading
      std::vector<std::string> m_objects_disabled  { };
      /// Property: Option to specify event section in the input data (tree, table etc.)
      std::string              m_input_section { "EVENT" };
      /// Property: Input data segment name
      std::string              m_input_segment { "inputs" };
      /// Property: Mask to flag input source items
      int                      m_input_mask    { NO_MASK };
      /// Property: Number of events to be read by file
      int                      m_events_per_file { -1 };
      /// Property: Loop on inputs and restart at end
      bool                     m_input_rescan { true };
      /// Property: generate raw input history records to Digi store
      bool                     m_keep_raw       { true };
      
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
      /// Access to input section name containing the event data
      const std::string& input_section()  const   {
	return m_input_section;
      }
      /// Access to container names containing data
      const std::vector<std::string>& objects_enabled()  const   {
	return m_objects_enabled;
      }
      /// Access to container names containing data
      const std::vector<std::string>& objects_disabled()  const   {
	return m_objects_disabled;
      }
      /// Check if the number of events per file is reached
      bool fileLimitReached(input_source& source)   const;
      /// Callback when a new file is opened
      virtual void onOpenFile(input_source& source);
      /// Callback when a new event is processed
      virtual void onProcessEvent(input_source& source, event_frame& frame);

      /// Check if a event object should be loaded: Default YES unless inhibited by selection or veto
      bool object_loading_is_enabled(const std::string& nam)  const;

      /// Callback to read event input
      virtual void execute(context_t& context)  const override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIINPUTACTION_H
