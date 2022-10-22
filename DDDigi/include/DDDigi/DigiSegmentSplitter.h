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
#include <DDDigi/DigiSegmentProcessor.h>
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
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSegmentSplitter : public DigiContainerSequence   {
    protected:
      using self_t      = DigiSegmentSplitter;
      using split_t     = std::pair<DetElement, VolumeID>;
      using splits_t    = std::map<VolumeID, split_t>;
      friend class DigiParallelWorker<processor_t, work_t, VolumeID>;
      
    protected:
      /// Property: Split element of the ID descriptor
      std::string          m_processor_type;
      /// Name of the subdetector to be handed
      std::string          m_detector_name;
      /// Splitter field in the segmentation description
      std::string          m_split_by;
      /// Property: Flag if processors should be shared
      bool                 m_share_processor   { true };

      /**  Member variables  */
      /// Segmentation too instance
      mutable DigiSegmentationTool m_split_tool;
      /// Segmentation split context
      DigiSegmentContext   m_split_context;
      /// Data keys from the readout collection names
      std::vector<Key>     m_keys;
      /// Split elements used to parallelize the processing
      splits_t             m_splits;

    protected:
      /// Default destructor
      virtual ~DigiSegmentSplitter();

      /// Initialization function
      void initialize();
      /// Adopt new parallel worker: INHIBITED: will throw exception
      virtual void adopt_processor(DigiContainerProcessor* action) override final;

    public:
      /// Standard constructor
      DigiSegmentSplitter(const DigiKernel& kernel, const std::string& name);
      /// Access the readout collection keys
      std::vector<std::string> collection_names()   const;
      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTSPLITTER_H
