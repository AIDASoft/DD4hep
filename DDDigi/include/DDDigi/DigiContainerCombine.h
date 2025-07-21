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
#ifndef DDDIGI_DIGICONTAINERCOMBINE_H
#define DDDIGI_DIGICONTAINERCOMBINE_H

/// Framework include files
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiParallelWorker.h>

/// C/C++ include files
#include <mutex>
#include <functional>

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
    class DigiContainerCombine : public DigiEventAction   {
    public:
      class work_definition_t;
      using self_t  = DigiContainerCombine;
      using Worker  = DigiParallelWorker<self_t,work_definition_t>;
      using Workers = DigiParallelWorkers<Worker>;

    protected:
      /// Property: Container names to be loaded
      std::vector<std::string>       m_containers   { };
      /// Property: Output container dressing
      std::string                    m_output_name_flag;
      /// Property: Input data segment name
      std::string                    m_input;
      /// Property: event masks to be handled
      std::vector<int>               m_input_masks  { };
      /// Property: Output data segment name
      std::string                    m_output;
      /// Property: mask of the deposit
      int                            m_deposit_mask { 0 };
      /// Property: Flag to erase already combined containers (not thread-safe!!!)
      bool                           m_erase_combined;
      /// Property: Flag to indicate to merge 
      bool                           m_merge_deposits;
      /// Property: Flag to indicate to merge 
      bool                           m_merge_response;
      /// Property: Flag to indicate to merge 
      bool                           m_merge_history;
      /// Property: Flag to indicate to merge 
      bool                           m_merge_particles;

      /// Fully qualified keys of all containers to be manipulated
      std::set<Key::key_type>        m_keys  { };
      /// Container keys of all containers to be manipulated
      std::set<Key::key_type>        m_cont_keys  { };

      /// Lock to properly accumulate keys which were already used
      mutable std::mutex             m_used_keys_lock;
      /// Worker objects to be submitted to TBB each performing part of the job
      Workers m_workers;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiContainerCombine);

      /// Default destructor
      virtual ~DigiContainerCombine();

      /// Initializing function: compute values which depend on properties
      void initialize();

      /// Check if we have sufficient workers
      void have_workers(size_t len)  const;

      /// Combine selected containers to one single deposit container
      std::size_t combine_containers(context_t& context,
				     DigiEvent& event,
				     DataSegment& inputs,
				     DataSegment& outputs)  const;

      /// Decide if a container is to merged based on the properties
      virtual bool use_key(Key key)  const;

    public:
      /// Standard constructor
      DigiContainerCombine(const kernel_t& kernel, const std::string& name);

      /// Main functional callback
      virtual void execute(context_t& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGICONTAINERCOMBINE_H
