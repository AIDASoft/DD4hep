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
#ifndef DDDIGI_DIGIPARALLELWORKER_H
#define DDDIGI_DIGIPARALLELWORKER_H

/// Framework include files
#include <DDDigi/DigiKernel.h>

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
    template <typename ACTION_TYPE, typename CALLDATA, typename OPTIONS>
      class DigiParallelWorker : public DigiKernel::ParallelCall   {
    public:
      using action_t = ACTION_TYPE;
      using options_t = OPTIONS;
      using calldata_t = CALLDATA;

      action_t* action;
      options_t options;

    public:
      /// Initializing constructor
      DigiParallelWorker(ACTION_TYPE* p, const OPTIONS& opts);
      /// Default constructor
      DigiParallelWorker() = delete;
      /// Move constructor
      DigiParallelWorker(DigiParallelWorker&& copy) = default;
      /// Inhibit copy constructor
      DigiParallelWorker(const DigiParallelWorker& copy) = default;
      /// Inhibit move assignment
      DigiParallelWorker& operator=(DigiParallelWorker&& copy) = delete;
      /// Inhibit copy assignment
      DigiParallelWorker& operator=(const DigiParallelWorker& copy) = delete;
      /// Default destructor
      virtual ~DigiParallelWorker();
      /// Access to processor name
      const char* name()  const {  return action->name().c_str();   }
      /// Callback on data
      virtual void execute(void* data) const override;
    };

    /// Initializing constructor
    template <typename ACTION_TYPE, typename CALLDATA, typename OPTIONS>
      DigiParallelWorker<ACTION_TYPE, CALLDATA, OPTIONS>::DigiParallelWorker(ACTION_TYPE* proc, const OPTIONS& opts)
      : ParallelCall(), action(proc), options(opts)
    {
      action->addRef();
    }

    /// Default destructor
    template <typename ACTION_TYPE, typename CALLDATA, typename OPTIONS>
      DigiParallelWorker<ACTION_TYPE, CALLDATA, OPTIONS>::~DigiParallelWorker()   {
      if ( action )   {
	action->release();
	action = nullptr;
      }
    }
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIPARALLELWORKER_H
