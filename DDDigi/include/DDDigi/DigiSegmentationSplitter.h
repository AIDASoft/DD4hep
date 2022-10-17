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
#ifndef DDDIGI_DIGISEGMENTATIONSPLITTER_H
#define DDDIGI_DIGISEGMENTATIONSPLITTER_H

// Framework include files
#include <DDDigi/DigiSegmentAction.h>
#include <DDDigi/DigiActionSequence.h>
#include <DDDigi/DigiSegmentationTool.h>

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
    class DigiSegmentationSplitter : public DigiActionSequence   {

    protected:
      /// Implementation declaration
      class internals_t;
      /// Reference to the implementation
      std::unique_ptr<internals_t> internals;

      /// Property: Identifier of the input repository
      std::string m_input_id;
      /// Property: Split element of the ID descriptor
      std::string m_processor_type;
      /// Name of the subdetector to be handed
      std::string m_detector_name;
      /// Splitter field in the segmentation description
      std::string m_split_by;
      /// Property: Flag if processors should be shared
      bool        m_share_processor   { true };
      /// Property: Input mask in the repository
      int         m_input_mask;

      /// Segmentation too instance
      DigiSegmentationTool  m_split_tool;
      /// Segmentation split context
      DigiSegmentContext    m_split_context;

      /// Split elements used to parallelize the processing
      std::map<VolumeID, std::pair<DetElement, VolumeID> > m_splits;
      /// Input data keys: depend on dd4hep::Readout and the input mask(s)
      std::vector<Key> m_data_keys;

    protected:
      /// Default destructor
      virtual ~DigiSegmentationSplitter();

      /// Initialization function
      void initialize();

    public:
      /// Standard constructor
      DigiSegmentationSplitter(const DigiKernel& kernel, const std::string& name);

      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTATIONSPLITTER_H
