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
#ifndef DDDIGI_DIGIPARALLELWORKERS_H
#define DDDIGI_DIGIPARALLELWORKERS_H

/// Framework include files
#include <DDDigi/DigiSemaphore.h>
#include <DDDigi/DigiParallelWorkerGroup.h>

/// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class ParallelWorker;
    template <typename T, typename A, typename O> class DigiParallelWorker;
    template <typename T> class DigiParallelWorkers;
    template <typename T> class DigiParallelWorkerGroup;

    /// Wrapper class to submit bulk actions
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    template <typename T> class DigiParallelWorkers   {
    public:
      typedef T worker_t;
      typedef DigiParallelWorkers<T> self_t;
      typedef DigiParallelWorkerGroup<T> group_t;

      friend class DigiParallelWorkerGroup<worker_t>;

    private:
      mutable std::vector<worker_t*>   actors;
      mutable DigiSemaphore            semaphore;

    public:
      /// Default constructor
      DigiParallelWorkers() = default;
      /// Inhibit Move constructor
      DigiParallelWorkers(DigiParallelWorkers&& copy) = delete;
      /// Inhibit copy constructor
      DigiParallelWorkers(const DigiParallelWorkers& copy) = delete;
      /// Inhibit move assignment
      DigiParallelWorkers& operator=(DigiParallelWorkers&& copy) = delete;
      /// Inhibit copy assignment
      DigiParallelWorkers& operator=(const DigiParallelWorkers& copy) = delete;
      /// Default destructor
      virtual ~DigiParallelWorkers();

      std::unique_lock<std::mutex> can_modify()  const;
      /// Return array protected worker group
      group_t get_group()  const;

      /// NOT thread-safe stuff. Do not use during event processing unless you are sequential
      const std::vector<worker_t*>& get() const { return actors; }
      std::size_t size()  const;
      bool empty()  const;
      bool insert(worker_t* entry)  const;
      ParallelCall*const* get_calls()   const;
    };

    template <typename T> inline
      DigiParallelWorkers<T>::~DigiParallelWorkers()  {
      for(auto* w : actors)
	delete w;
      actors.clear();
    }

    template <typename T> inline 
      std::unique_lock<std::mutex> DigiParallelWorkers<T>::can_modify()  const  {
      return this->semaphore.wait_null();
    }

    template <typename T> inline
      bool DigiParallelWorkers<T>::insert(worker_t* entry)   const {
      if ( entry )  {
	actors.emplace_back(entry);
	return true;
      }
      return false;
    }

    template <typename T> inline
      std::size_t DigiParallelWorkers<T>::size() const   {
      return actors.size();
    }

    template <typename T> inline
      bool DigiParallelWorkers<T>::empty() const   {
      return actors.empty();
    }

    template <typename T> inline
      ParallelWorker*const* DigiParallelWorkers<T>::get_calls()  const  {
      return (ParallelWorker**)&this->actors.at(0);
    }

    template <typename T> inline typename DigiParallelWorkers<T>::group_t
      DigiParallelWorkers<T>::get_group()  const  {
      return group_t(*this);
    }
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIPARALLELWORKERS_H
