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
#include <DDDigi/DigiParallelWorker.h>
#include <DDDigi/DigiSegmentationTool.h>
#include <DDDigi/DigiContainerProcessor.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Segmentation split context
    /**
     *  
     *  
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSegmentProcessContext : public DigiSegmentContext  {
    public:
      using predicate_t = DigiContainerProcessor::predicate_t;
      predicate_t predicate;

    public:
      /// Default constructor
      DigiSegmentProcessContext() = default;
      /// Default copy constructor
      DigiSegmentProcessContext(const DigiSegmentContext& copy);
      /// Default move constructor
      DigiSegmentProcessContext(DigiSegmentProcessContext&& copy) = default;
      /// Default copy constructor
      DigiSegmentProcessContext(const DigiSegmentProcessContext& copy) = default;
      /// Default destructor
      virtual ~DigiSegmentProcessContext() = default;
      /// Default move assignment
      DigiSegmentProcessContext& operator=(const DigiSegmentContext& copy);
      /// Default move assignment
      DigiSegmentProcessContext& operator=(DigiSegmentProcessContext&& copy) = default;
      /// Default copy assignment
      DigiSegmentProcessContext& operator=(const DigiSegmentProcessContext& copy) = default;

      /// Full identifier (field + id)
      std::string identifier()  const;
      /// Check a given cell id if it matches this selection
      //bool matches(uint64_t cell)  const  {
      //  return this->split_id(cell) == this->predicate.id;
      //}
      /// Check a given cell id if it matches this selection
      bool matches(CellID cell)  const  {
        return this->split_id(cell) == this->predicate.id;
      }

      /// Check if a deposit should be processed
      bool use_depo(const std::pair<const CellID, EnergyDeposit>& deposit)   const   {
	return this->matches(deposit.first);
      }
      void enable(uint32_t split_id);
    };

    /// Predicate function to accept a subset of segment entries
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct accept_segment_t : public DigiContainerProcessor::predicate_t  {
      accept_segment_t(const DigiSegmentContext* s, uint32_t i)
	: predicate_t(std::bind(&accept_segment_t::use_depo, this, std::placeholders::_1), i, s) {
      }
      /// Check if a deposit should be processed
      bool use_depo(const deposit_t& deposit)   const   {
	return this->segmentation->split_id(deposit.first) == this->id;
      }
    };

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSegmentSplitter : public DigiContainerProcessor   {
    protected:

      /**  Local use definitions                      */
      using self_t      = DigiSegmentSplitter;
      using tool_t      = DigiSegmentationTool;
      using splits_t    = std::set<uint32_t>;
      using segment_t   = DigiSegmentProcessContext;
      using processor_t = DigiContainerProcessor;

      using worker_t    = DigiParallelWorker<processor_t, work_t, segment_t>;
      using workers_t   = DigiParallelWorkers<worker_t>;
      friend class DigiParallelWorker<processor_t, work_t, segment_t>;

    protected:

      /**  Object properties                          */
      /// Property: Split element of the ID descriptor
      std::string          m_processor_type;
      /// Property: Name of the subdetector to be handed
      std::string          m_detector_name;
      /// Property: Splitter field in the segmentation description
      std::string          m_split_by;
      /// Property to steer parallel processing
      bool                 m_parallel          { false };
      /// Property: Flag if processors should be shared
      bool                 m_share_processor   { true };

      /**  Member variables                           */
      /// Data keys from the readout collection names
      std::vector<Key>   m_keys;
      /// Segmentation split context
      segment_t          m_split_context;
      /// Split elements used to parallelize the processing
      splits_t           m_splits;
      /// Array of sub-workers
      workers_t          m_workers;

      /// Lock for output merging
      mutable std::mutex m_output_lock;
      /// Segmentation too instance
      mutable tool_t     m_split_tool;

    protected:
      /// Default destructor
      virtual ~DigiSegmentSplitter();

      /// Initialization function
      void initialize();

    public:
      /// Adopt new parallel worker handling single split identifier
      virtual void adopt_segment_processor(DigiContainerProcessor* action, int split_id);
      /// Adopt new parallel worker handling multiple split-identifiers
      virtual void adopt_segment_processor(DigiContainerProcessor* action, const std::vector<int>&  ids);

    public:
      /// Standard constructor
      DigiSegmentSplitter(const kernel_t& kernel, const std::string& name);
      /// Access the readout collection keys
      std::vector<std::string> collection_names()   const;
      /// Main functional callback
      virtual void execute(context_t& context, work_t& work, const predicate_t& predicate)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTSPLITTER_H
