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
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiActionSequence.h>

/// C/C++ include files
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Wrapper class to submit bulk actions
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename ACTION_TYPE, typename OPTIONS> 
      class DigiParallelWorker : public DigiKernel::CallWrapper   {
      ACTION_TYPE* processor  { nullptr };
    public:
      OPTIONS options;

    public:
      DigiParallelWorker(ACTION_TYPE* p, const OPTIONS& opts);
      DigiParallelWorker() = default;
      DigiParallelWorker(DigiParallelWorker&& copy) = default;
      DigiParallelWorker(const DigiParallelWorker& copy) = delete;
      DigiParallelWorker& operator=(DigiParallelWorker&& copy) = delete;
      DigiParallelWorker& operator=(const DigiParallelWorker& copy) = delete;
      virtual ~DigiParallelWorker();
      const char* name()  const {  return processor->name().c_str(); }
      virtual void operator()() const override final;
    };

    template <typename ACTION_TYPE, typename OPTIONS>
      DigiParallelWorker<ACTION_TYPE, OPTIONS>::DigiParallelWorker(ACTION_TYPE* p, const OPTIONS& opts)
      : processor(p), options(opts)
    {
      processor->addRef();
    }

    template <typename ACTION_TYPE, typename OPTIONS>
      DigiParallelWorker<ACTION_TYPE, OPTIONS>::~DigiParallelWorker()   {
      if ( processor )  {
	processor->release();
	processor = nullptr;
      }
    }


    /// Worker base class to analyse containers from the input segment in parallel
    class DigiContainerProcessor : public DigiAction   {

    public:
      using WorkItems = std::vector<std::pair<Key, std::any*> >;
      
    public:
      /// Property: Input data segment name
      std::string      m_input_segment { "inputs" };
      /// Property: event masks to be handled
      std::vector<int> m_input_masks  { };
      /// Item keys required by this worker
      std::vector<Key> m_work_keys;

    public:
      /// Standard constructor
      DigiContainerProcessor(const DigiKernel& kernel, const std::string& name);
      /// Main functional callback if specific work is known
      virtual void execute(DigiContext& context, WorkItems& work)  const;
    };


    /// Sequencer class to analyse containers from the input segment in parallel
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiMultiContainerProcessor : public DigiEventAction   {
    public:
      using WorkItems = DigiContainerProcessor::WorkItems;
      struct ProcessorOptions  {
	DigiContext*     context { nullptr };
	WorkItems*       work    { nullptr };
	std::vector<Key> keys    { };
      };
      using Worker  = DigiParallelWorker<DigiContainerProcessor,ProcessorOptions>;
      using Workers = std::vector<std::unique_ptr<Worker> >;
      using Callers = std::vector<DigiKernel::CallWrapper*>;

    protected:
      /// Property: Input data segment name
      std::string       m_input_segment { "inputs" };
      /// Property: event masks to be handled
      std::vector<int>  m_input_masks  { };
      /// Property: Allow for multiple workers accessing same container
      bool              m_allow_duplicates   { true };

      std::set<Key>     m_work_items;
      Workers           m_workers;
      Callers           m_callers;

    protected:
       /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiMultiContainerProcessor);
      /// Default destructor
      virtual ~DigiMultiContainerProcessor();

    public:
      /// Standard constructor
      DigiMultiContainerProcessor(const DigiKernel& kernel, const std::string& name);
      /// Adopt new parallel worker
      void adopt_processor(DigiContainerProcessor* action, const std::vector<std::string>& containers);
      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIMULTICONTAINERPROCESSOR_H
