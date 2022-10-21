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
#ifndef DDDIGI_DIGIMULTICONTAINERPROCESSOR_H
#define DDDIGI_DIGIMULTICONTAINERPROCESSOR_H

// Framework include files
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiParallelWorker.h>

/// C/C++ include files
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiContainerProcessor;
    class DigiContainerSequence;
    class DigiContainerSequenceAction;
    class DigiMultiContainerProcessor;

    /// Worker base class to analyse containers from the input segment in parallel
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiContainerProcessor : public DigiAction   {
    public:
      struct work_t  {
	/// Event processing context
	DigiContext&    context;
	/// Input data key
	Key             key;
	/// Input deposits
	std::any&       input;
	/// Lock for secure output merging
	std::mutex&     output_lock;
	/// Handle to output 
	std::any&       output;

	/// Merge output data (thread safe, locked)
	void merge_output(DepositVector&& data);
	/// Merge output data (thread safe, locked)
	void emplace_output(CellID cell, EnergyDeposit&& deposit);
      };
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerProcessor);

    private:
      /// Main functional callback if specific work is known.
      virtual void execute(DigiContext& context, Key key, std::any& data)  const;

    public:
      /// Standard constructor
      DigiContainerProcessor(const DigiKernel& kernel, const std::string& name);
      /// Default destructor
      virtual ~DigiContainerProcessor();
      /// Main functional callback adapter
      virtual void execute(DigiContext& context, work_t& work)  const;
    };

    /// Worker class act on containers in an event identified by input masks and container name
    /**
     *  The sequencer calls all registered processors for the contaiers registered.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiContainerSequence : public DigiContainerProcessor  {
    protected:
      /// Property to steer parallel processing
      bool m_parallel { false };

    protected:
      using self_t = DigiContainerSequence;
      using processor_t = DigiContainerProcessor;
      using worker_t  = DigiParallelWorker<processor_t,work_t>;
      using workers_t = DigiParallelWorkers<worker_t>;
      friend class DigiParallelWorker<processor_t,work_t>;

      /// Array of sub-workers
      workers_t         m_workers;

      /// Lock for output merging
      mutable std::mutex        m_output_lock;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerSequence);
      /// Default destructor
      virtual ~DigiContainerSequence();

    public:
      /// Standard constructor
      DigiContainerSequence(const DigiKernel& kernel, const std::string& name);
      /// Adopt new parallel worker
      virtual void adopt_processor(DigiContainerProcessor* action);
      /// Main functional callback adapter
      virtual void execute(DigiContext& context, work_t& work)  const;
    };

    /// Worker base class to analyse containers from the input segment in parallel
    /**
     *  Depending on the adopted processors, the full input record is scanned and
     *  the registered processors are called.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiContainerSequenceAction : public DigiEventAction  {

    protected:
      /// Argument structure for client calls
      using self_t = DigiContainerSequenceAction;
      using processor_t = DigiContainerProcessor;
      struct work_item_t  {
	Key key;
	std::any* data;
      };
      struct work_t  {
	DigiContext&             context;
	std::vector<work_item_t> input_items;
	/// Lock for secure output merging
	std::mutex&              output_lock;
	std::any                 output;
	const self_t*            parent;
      };
      using worker_t         = DigiParallelWorker<processor_t, work_t>;
      using workers_t        = DigiParallelWorkers<worker_t>;
      using reg_workers_t    = std::map<Key, worker_t*>;
      using reg_processors_t = std::map<Key, processor_t*>;
      friend class DigiParallelWorker<processor_t, work_t>;

      /// Array of sub-workers
      workers_t         m_workers;
      /// Registered action map
      reg_processors_t  m_registered_processors;
      /// Registered worker map
      reg_workers_t     m_registered_workers;

      /// Property: Input data segment name
      std::string       m_input_segment { "inputs" };
      /// Property: Input mask to be handled
      int               m_mask { 0x0 };

      /// Lock for output merging
      mutable std::mutex        m_output_lock;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerSequenceAction);

      /// Default destructor
      virtual ~DigiContainerSequenceAction();
      /// Initialization callback
      void initialize();

    public:
      /// Standard constructor
      DigiContainerSequenceAction(const DigiKernel& kernel, const std::string& name);
      /// Adopt new parallel worker
      void adopt_processor(DigiContainerProcessor* action, const std::string& container);
      /// Main functional callback if specific work is known
      virtual void execute(DigiContext& context)  const override;
    };

    /// Sequencer class to analyse containers from the input segment in parallel
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiMultiContainerProcessor : virtual public DigiEventAction   {
    protected:
      using self_t       = DigiMultiContainerProcessor;
      using processor_t  = DigiContainerProcessor;
      using work_items_t = std::vector<std::pair<Key, std::any*> >;
      /// Argument structure for client calls
      struct work_t  {
	DigiContext&    context;
	work_items_t&   items;
	std::mutex&     output_lock;
	std::any        output;
	const self_t*   parent;
      };
      using worker_t  = DigiParallelWorker<processor_t, work_t>;
      using workers_t = DigiParallelWorkers<worker_t>;
      friend class DigiParallelWorker<processor_t, work_t>;

    protected:
      /// Property: Input data segment name
      std::string       m_input_segment { "inputs" };
      /// Property: event masks to be handled
      std::vector<int>  m_input_masks  { };

      /// Set of work items to be processed and passed to clients
      std::set<Key>     m_work_items;

      std::vector<std::vector<Key> > m_worker_keys;
      /// Array of sub-workers
      workers_t         m_workers;

      /// Lock for output merging
      mutable std::mutex        m_output_lock;

    protected:
       /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiMultiContainerProcessor);
      /// Default destructor
      virtual ~DigiMultiContainerProcessor();

    public:
      /// Standard constructor
      DigiMultiContainerProcessor(const DigiKernel& kernel, const std::string& name);
      const std::vector<Key>& worker_keys(size_t worker_id)  const  {
	return this->m_worker_keys.at(worker_id);
      }
      const std::vector<int>& input_masks()  const   {
	return this->m_input_masks;
      }
      /// Adopt new parallel worker
      void adopt_processor(DigiContainerProcessor* action, const std::vector<std::string>& containers);
      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIMULTICONTAINERPROCESSOR_H
