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
#ifndef DDDIGI_DIGICONTAINERPROCESSOR_H
#define DDDIGI_DIGICONTAINERPROCESSOR_H

// Framework include files
#include <DD4hep/Callback.h>
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiDepositMonitor.h>
#include <DDDigi/DigiParallelWorker.h>

/// C/C++ include files
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiDepositMonitor;
    class DigiSegmentContext;
    class DigiContainerSequence;
    class DigiContainerProcessor;
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
      /// Structure imports
      using action_t       = DigiAction;
      using segment_t      = DataSegment;
      using property_t     = PropertyManager;
      using segmentation_t = DigiSegmentContext;

      /// Input definition
      struct input_t  {
        /// Input segment reference
        segment_t*      segment;
        /// Input data key
        Key             key;
        /// Input deposits
        std::any*       data;
      };

      /// Output handle definition
      struct output_t  {
        int             mask;
        segment_t&      data;
      };

      /// Hit processing predicate
      struct predicate_t  {
        using deposit_t  = std::pair<const CellID, EnergyDeposit>;
        using callback_t = std::function<bool(const deposit_t&)>;
        callback_t            callback      { };
        uint32_t              id            { 0 };
        const segmentation_t* segmentation  { nullptr };

        predicate_t() = default;
        predicate_t(std::function<bool(const deposit_t&)> func, uint32_t i, const segmentation_t* s)
          : callback(std::move(func)), id(i), segmentation(s) {}
        predicate_t(predicate_t&& copy) = default;
        predicate_t(const predicate_t& copy) = default;
        predicate_t& operator = (predicate_t&& copy) = default;
        predicate_t& operator = (const predicate_t& copy) = default;
        /// Check if a deposit should be processed
        bool operator()(const deposit_t& deposit)   const;
        static bool always_true(const deposit_t&)        { return true; }
        static bool not_killed (const deposit_t& depo)   { return 0 == (depo.second.flag&EnergyDeposit::KILLED); }
      };

      struct env_t   {
        /// Event processing context
        context_t&        context;
        /// Optional properties
        const property_t& properties;
        /// Output data
        output_t&         output;
      };

      /// Work definition
      struct work_t  {
        env_t             environ;
        /// Input data
        input_t           input;

        /// Basic check if input data are present
        bool has_input()  const    {  return this->input.data->has_value();  }
        /// Access key of input data
        Key  input_key()  const    {  return this->input.key;               }
        /// Access the input data type
        const std::type_info& input_type()  const;
        /// String form of the input data type
        std::string input_type_name()  const;
        /// Access input data by type
        template <typename DATA> DATA* get_input(bool exc=false);
        /// Access input data by type
        template <typename DATA> const DATA* get_input(bool exc=false)  const;
      };

      /// Monitoring object
      DigiDepositMonitor* m_monitor { nullptr };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerProcessor);

    public:
      /// Access to default deposit predicate accepting all
      static const predicate_t& accept_all();
      /// Access to default deposit predicate accepting all
      static const predicate_t& accept_not_killed();

      /// Standard constructor
      DigiContainerProcessor(const kernel_t& kernel, const std::string& name);
      /// Default destructor
      virtual ~DigiContainerProcessor();
      /// Adopt monitoring action
      virtual void adopt_monitor(DigiDepositMonitor* monitor);
      /// Main functional callback adapter
      virtual void execute(context_t& context, work_t& work, const predicate_t& predicate)  const;
    };

    /// Check if a deposit should be processed
    inline bool DigiContainerProcessor::predicate_t::operator()(const deposit_t& deposit)   const   {
      return this->callback(deposit);
    }

    /// Worker class act on ONLY act on energy deposit containers in an event
    /**
     *  Worker class act on ONLY act on energy deposit containers in an event.
     *  The deposit containers are identified by input masks and container name.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositsProcessor : public DigiContainerProcessor  {
    protected:
      std::function<void(context_t& context, DepositVector& cont,  work_t& work, const predicate_t& predicate)>	m_handleVector;
      std::function<void(context_t& context, DepositMapping& cont, work_t& work, const predicate_t& predicate)>	m_handleMapping;

    public:
      /// Standard constructor
      using DigiContainerProcessor::DigiContainerProcessor;

      /// Main functional callback adapter
      virtual void execute(context_t& context, work_t& work, const predicate_t& predicate)  const;
    };
    
#define DEPOSIT_PROCESSOR_BIND_HANDLERS(X)                              \
    this->m_handleVector  = std::bind( &X<DepositVector>,  this,        \
                                       std::placeholders::_1,           \
                                       std::placeholders::_2,           \
                                       std::placeholders::_3,           \
                                       std::placeholders::_4);          \
    this->m_handleMapping = std::bind( &X<DepositMapping>, this,        \
                                       std::placeholders::_1,           \
                                       std::placeholders::_2,           \
                                       std::placeholders::_3,           \
                                       std::placeholders::_4)

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
      /// Structure imports
      using self_t      = DigiContainerSequence;
      using processor_t = DigiContainerProcessor;
      using worker_t    = DigiParallelWorker<processor_t,work_t,std::size_t,self_t&>;
      using workers_t   = DigiParallelWorkers<worker_t>;
      friend class DigiParallelWorker<processor_t,work_t,std::size_t,self_t&>;

    protected:
      /**  Member variables                           */
      /// Property to steer parallel processing
      bool               m_parallel { false };
      /// Array of sub-workers
      workers_t          m_workers;
      /// Default Deposit predicate
      predicate_t        m_worker_predicate = processor_t::accept_all();

      /// Lock for output merging
      mutable std::mutex m_output_lock;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerSequence);
      /// Default destructor
      virtual ~DigiContainerSequence();

      /// Get hold of the registered processor for a given container
      worker_t* need_registered_worker(Key item_key)  const;

    public:
      /// Standard constructor
      DigiContainerSequence(const kernel_t& kernel, const std::string& name);
      /// Set the default predicate
      virtual void set_predicate(const predicate_t& predicate);
      /// Adopt new parallel worker
      virtual void adopt_processor(DigiContainerProcessor* action);
      /// Main functional callback adapter
      virtual void execute(context_t& context, work_t& work, const predicate_t& predicate)  const;
    };


    struct DigiMultiProcessorParent   {
    public:
      using action_t     = DigiAction;
      using processor_t  = DigiContainerProcessor;
      using env_t        = processor_t::env_t;
      using segment_t    = processor_t::segment_t;
      using predicate_t  = processor_t::predicate_t;
      using output_t     = processor_t::output_t;
      using property_t   = processor_t::property_t;
      using work_item_t  = processor_t::input_t;
      using work_items_t = std::vector<processor_t::input_t>;
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
    class DigiContainerSequenceAction : public DigiEventAction, public DigiMultiProcessorParent  {

    protected:
      /// Structure imports
      using self_t      = DigiContainerSequenceAction;
      /// Work definition structure: Argument structure for client calls
      struct work_t  {
        env_t&        environ;
        work_items_t& input_items;
        const self_t& parent;
      };

      using worker_t         = DigiParallelWorker<processor_t, work_t, std::size_t, self_t&>;
      using workers_t        = DigiParallelWorkers<worker_t>;
      using predicate_t      = processor_t::predicate_t;
      using reg_workers_t    = std::map<Key::key_type, worker_t*>;
      using reg_processors_t = std::map<Key::key_type, processor_t*>;
      friend class DigiParallelWorker<processor_t, work_t, std::size_t, self_t&>;

      /// Property: Input data segment name
      std::string              m_input_segment         { "inputs" };
      /// Property: Input mask to be handled
      int                      m_input_mask            { 0x0 };
      /// Property: Input data segment name
      std::string              m_output_segment        { "outputs" };
      /// Property: event mask for output data
      int                      m_output_mask           { 0x0 };

      /// Array of sub-workers
      workers_t                m_workers               { };
      /// Registered action map
      reg_processors_t         m_registered_processors { };
      /// Registered worker map
      reg_workers_t            m_registered_workers    { };
      /// Default Deposit predicate
      predicate_t              m_worker_predicate      { processor_t::accept_all() };

      /// Lock for output merging
      mutable std::mutex       m_output_lock           { };
      
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerSequenceAction);

      /// Default destructor
      virtual ~DigiContainerSequenceAction();

      /// Initialization callback
      virtual void initialize();

      /// Finalization callback
      virtual void finalize();

      /// Get hold of the registered processor for a given container
      worker_t* need_registered_worker(Key item_key, bool exc=true)  const;

    public:
      /// Standard constructor
      DigiContainerSequenceAction(const kernel_t& kernel, const std::string& name);
      /// Set the default predicate
      virtual void set_predicate(const predicate_t& predicate);
      /// Adopt new parallel worker acting on one single container
      virtual void adopt_processor(DigiContainerProcessor* action, const std::string& container);
      /// Adopt new parallel worker acting on multiple containers
      virtual void adopt_processor(DigiContainerProcessor* action, const std::vector<std::string>& containers);
      /// Main functional callback if specific work is known
      virtual void execute(context_t& context)  const override;
    };

    /// Sequencer class to analyse containers from the input segment in parallel
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiMultiContainerProcessor : public DigiEventAction, public DigiMultiProcessorParent   {
    protected:
      using self_t        = DigiMultiContainerProcessor;
      using worker_keys_t = std::vector<std::vector<Key> >;

      /// Argument structure required to support multiple client calls
      struct work_t  {
        env_t&              environ;
        work_items_t&       items;
        const self_t&       parent;
      };
      using worker_t      = DigiParallelWorker<processor_t, work_t, std::size_t, self_t&>;
      using workers_t     = DigiParallelWorkers<worker_t>;
      friend class DigiParallelWorker<processor_t, work_t, std::size_t, self_t&>;

    protected:
      /// Property: Input data segment name
      std::string        m_input_segment  { "inputs" };
      /// Property: event masks to be handled
      std::vector<int>   m_input_masks    { };
      /// Property: Input data segment name
      std::string        m_output_segment { "outputs" };
      /// Property: event mask for output data
      int                m_output_mask    { 0x0 };

      /// Set of container names to be used by this processor
      std::map<std::string, std::vector<processor_t*> >    m_processors;
      std::map<Key::itemkey_type, std::vector<worker_t*> > m_worker_map;

      /// Set of work items to be processed and passed to clients
      std::set<Key>             m_work_items;
      /// Set of keys required by each worker
      worker_keys_t             m_worker_keys;
      /// Ordered list of actions registered
      std::vector<processor_t*> m_actions;
      /// Default Deposit predicate
      predicate_t               m_worker_predicate = processor_t::accept_all();
      /// Lock for output merging
      mutable std::mutex        m_output_lock;

      /// Array of sub-workers
      workers_t                 m_workers;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiMultiContainerProcessor);
      /// Default destructor
      virtual ~DigiMultiContainerProcessor();
      /// Initialize action object
      void initialize();

    public:
      /// Standard constructor
      DigiMultiContainerProcessor(const kernel_t& kernel, const std::string& name);
      const std::vector<Key>& worker_keys(size_t worker_id)  const  {
        return this->m_worker_keys.at(worker_id);
      }
      const std::vector<int>& input_masks()  const   {
        return this->m_input_masks;
      }
      /// Set the default predicate
      virtual void set_predicate(const predicate_t& predicate);
      /// Adopt new parallel worker
      virtual void adopt_processor(DigiContainerProcessor* action, const std::vector<std::string>& containers);
      /// Main functional callback
      virtual void execute(context_t& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGICONTAINERPROCESSOR_H
