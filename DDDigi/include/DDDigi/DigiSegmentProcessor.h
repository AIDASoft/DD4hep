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
#ifndef DDDIGI_DIGISEGMENTPROCESSOR_H
#define DDDIGI_DIGISEGMENTPROCESSOR_H

// Framework include files
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiContainerProcessor.h>
#include <DDDigi/DigiSegmentationTool.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiSegmentProcessor;
    class DigiSegmentSequence;

    /// Default base class to process parts of a subdetector segmentation data
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  Default base class to process parts of a subdetector segmentation data.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITAZATION
     */
    class DigiSegmentProcessor : public DigiContainerProcessor   {
    public:
      /// Segmentation split context
      using segment_t = DigiSegmentContext;
      using deposit_cell_t = std::pair<CellID,EnergyDeposit>;
      using predicate_t = std::unary_function<deposit_cell_t, bool>;

      struct accept_all_t : public predicate_t  {
	accept_all_t() = default;
	inline bool operator()(const deposit_cell_t& ) const  {
	  return true;
	}
      };
      struct accept_segment_t : public predicate_t  {
	const segment_t& segment;
        accept_segment_t(const segment_t& s) : segment(s) {}
	inline bool operator()(const deposit_cell_t& depo) const  {
	  return this->segment.matches(depo.first);
	}
      };

      segment_t  segment  { };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSegmentProcessor);

    public:
      /// Standard constructor
      DigiSegmentProcessor(const DigiKernel& kernel, const std::string& name);
      /// Default destructor
      virtual ~DigiSegmentProcessor();
      /// Main functional callback if specific work is known
      virtual void execute(DigiContext& context, work_t& data)  const override;
    };

    /// Sequencer class to process parts of a subdetector segmentation data
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  The sequencer calls all registered processors for the contaiers registered.
     *  The sequencer manages a set of workers all acting on the same data segment
     *  being a part of the data of a collection
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSegmentSequence : public DigiSegmentProcessor  {
    protected:
      /// Property to steer parallel processing
      bool m_parallel { false };

    protected:
      using self_t = DigiSegmentSequence;
      using processor_t = DigiSegmentProcessor;
      using worker_t  = DigiParallelWorker<processor_t,work_t,VolumeID>;
      using workers_t = DigiParallelWorkers<worker_t>;
      friend class DigiParallelWorker<processor_t,work_t,VolumeID>;

      /// Array of sub-workers
      workers_t         m_workers;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSegmentSequence);
      /// Default destructor
      virtual ~DigiSegmentSequence();

    public:
      /// Standard constructor
      DigiSegmentSequence(const DigiKernel& kernel, const std::string& name);
      /// Adopt new parallel worker
      virtual void adopt_processor(DigiContainerProcessor* action);
      /// Main functional callback if specific work is known
      virtual void execute(DigiContext& context, work_t& work)  const  override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEGMENTPROCESSOR_H
