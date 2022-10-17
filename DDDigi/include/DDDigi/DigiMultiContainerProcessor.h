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

    /// Worker base class to analyse containers from the input segment in parallel
    class DigiContainerProcessor : public DigiAction   {

    public:
      using WorkItems = std::vector<std::pair<Key, std::any*> >;
      
    public:
      /// Property: Input data segment name
      std::string      m_input_segment   { "inputs" };
      /// Property: event masks to be handled
      std::vector<int> m_input_masks     { };
      /// Item keys required by this worker
      std::vector<Key> m_container_keys  { };

    public:
      /// Standard constructor
      DigiContainerProcessor(const DigiKernel& kernel, const std::string& name);
      /// Main functional callback if specific work is known
      virtual void execute(DigiContext& context, WorkItems& work)  const;
      /// Check if the work item is for us
      bool use_container(Key key)  const;
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
      struct CallData  {
	DigiContext& context;
	WorkItems&   work;
      };
      using Worker    = DigiParallelWorker<DigiContainerProcessor,CallData,int>;
      using Workers   = std::vector<DigiKernel::ParallelCall*>;

    protected:
      /// Property: Input data segment name
      std::string       m_input_segment { "inputs" };
      /// Property: event masks to be handled
      std::vector<int>  m_input_masks  { };

      std::set<Key>     m_work_items;
      /// Array of sub-workers
      Workers           m_workers;

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
