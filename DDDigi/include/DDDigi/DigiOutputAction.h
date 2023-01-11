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
#include <DDDigi/DigiContainerProcessor.h>

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
    class DigiOutputAction : public DigiContainerSequenceAction {
    public:
    protected:
      /// Property: Processor type to manage containers
      std::string                        m_processor_type  { };
      /// Property: Container / data type mapping
      std::map<std::string, std::string> m_containers  { };
      /// Property: Output data specification
      std::string                        m_output { };
      /// Property: Create stream names with sequence numbers
      bool                               m_sequence_streams  {  true };

      /// Total numbe rof events to be processed
      long num_events  { -1 };
      /// Running event counter
      long event_count {  0 };
      /// Stream sequence counter
      long fseq_count  {  0 };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiOutputAction);

      /// Default destructor
      virtual ~DigiOutputAction();

    public:
      /// Standard constructor
      DigiOutputAction(const kernel_t& kernel, const std::string& nam);

      /// Initialization callback
      virtual void initialize();

      /// Finalization callback
      virtual void finalize();

      /// Check for valid output stream
      virtual bool have_output()  const = 0;

      /// Open new output stream
      virtual void open_output() const = 0;

      /// Close current output stream
      virtual void close_output() const = 0;

      /// Commit event data to output stream
      virtual void commit_output() const = 0;

      /// Create new output stream name
      virtual std::string next_stream_name();

      /// Adopt new parallel worker
      virtual void adopt_processor(DigiContainerProcessor* action,
                                   const std::string& container)  override;

      /// Adopt new parallel worker acting on multiple containers
      virtual void adopt_processor(DigiContainerProcessor* action,
                                   const std::vector<std::string>& containers);

      /// Callback to read event output
      virtual void execute(context_t& context)  const override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIOUTPUTACTION_H
