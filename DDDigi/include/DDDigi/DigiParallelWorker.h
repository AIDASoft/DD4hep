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

/// C/C++ include files
#include <cstdint>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class WorkerPredicate;

    /// Wrapper class to submit bulk actions
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class ParallelWorker     {
    public:
      /// Default constructor
      ParallelWorker() = default;
      ParallelWorker(ParallelWorker&& copy) = default;
      ParallelWorker(const ParallelWorker& copy) = default;
      ParallelWorker& operator=(ParallelWorker&& copy) = default;
      ParallelWorker& operator=(const ParallelWorker& copy) = default;
      virtual ~ParallelWorker() = default;
      virtual void execute(void* args) const = 0;
    };

    /// Wrapper class to submit bulk actions
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    template <typename ACTION_TYPE, typename CALLDATA, typename OPTIONS=std::size_t, typename PREDICATE=std::size_t>
      class DigiParallelWorker : public ParallelWorker   {
    public:
    using action_t = ACTION_TYPE;
    using options_t = OPTIONS;
    using calldata_t = CALLDATA;
    using predicate_t = PREDICATE;

    action_t*        action    { nullptr };
    options_t        options   { nullptr };
    predicate_t      predicate { nullptr };

    public:
    /// Initializing constructor
    DigiParallelWorker(action_t* a, const options_t& opts, const predicate_t& p={});
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
    template <typename ACTION_TYPE, typename CALLDATA, typename OPTIONS, typename PREDICATE>
      DigiParallelWorker<ACTION_TYPE, CALLDATA, OPTIONS, PREDICATE>::DigiParallelWorker(action_t* a, const options_t& o, const predicate_t& p)
      : ParallelWorker(), action(a), options(o), predicate(p)
    {
      if ( action ) action->addRef();
    }

    /// Default destructor
    template <typename ACTION_TYPE, typename CALLDATA, typename OPTIONS, typename PREDICATE>
      DigiParallelWorker<ACTION_TYPE, CALLDATA, OPTIONS, PREDICATE>::~DigiParallelWorker()   {
      if ( action )   {
	action->release();
	action = nullptr;
      }
    }

    typedef ParallelWorker ParallelCall;
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiParallelWorkers.h>
#include <DDDigi/DigiParallelWorkerGroup.h>

#endif // DDDIGI_DIGIPARALLELWORKER_H
