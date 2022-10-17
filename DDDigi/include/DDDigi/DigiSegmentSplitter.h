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
#ifndef DDDIGI_DIGISEGMENTSPLITTER_H
#define DDDIGI_DIGISEGMENTSPLITTER_H

// Framework include files
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiSegmentAction.h>
#include <DDDigi/DigiSegmentationTool.h>
#include <DDDigi/DigiParallelWorker.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiSegmentSplitter : public DigiEventAction   {
    public:

      struct CallData  {
	DigiContext&    context;
	DepositMapping& input;
	DepositMapping* output;
	const DigiSegmentSplitter* parent;
      };

      using Worker  = DigiParallelWorker<DigiSegmentAction,CallData,int>;
      using Workers = std::vector<DigiKernel::ParallelCall*>;
      using Splits  = std::map<VolumeID, std::pair<DetElement, VolumeID> >;
      
    protected:
      /// Implementation declaration
      class internals_t;
      /// Reference to the implementation
      std::unique_ptr<internals_t> internals;

      /// Property: Split element of the ID descriptor
      std::string          m_processor_type;
      /// Name of the subdetector to be handed
      std::string          m_detector_name;
      /// Splitter field in the segmentation description
      std::string          m_split_by;
      /// Property: Flag if processors should be shared
      bool                 m_share_processor   { true };

      /// Property: Identifier of the input repository
      std::string          m_input_id;
      /// Property: Input mask in the repository
      int                  m_input_mask;
      /// Property: Identifier of the input repository
      std::string          m_output_id;
      /// Property: Input mask in the repository
      int                  m_output_mask;

      /// Segmentation too instance
      DigiSegmentationTool m_split_tool;
      /// Segmentation split context
      DigiSegmentContext   m_split_context;

      /// Split elements used to parallelize the processing
      Splits               m_splits;
      /// Input data keys: depend on dd4hep::Readout and the input mask(s)
      std::vector<Key>     m_data_keys;

      /// Array of sub-workers
      Workers              m_workers;

      mutable std::mutex   m_output_lock;

    protected:
      /// Default destructor
      virtual ~DigiSegmentSplitter();
      /// Initialization function
      void initialize();

    public:
      /// Standard constructor
      DigiSegmentSplitter(const DigiKernel& kernel, const std::string& name);
      /// Handle result from segment callbacks
      void register_output(DepositMapping& result, DepositVector&& output)  const;
      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTSPLITTER_H
