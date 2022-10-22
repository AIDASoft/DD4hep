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
#ifndef DDDIGI_DIGIPARALLELWORKERGROUP_H
#define DDDIGI_DIGIPARALLELWORKERGROUP_H

/// Framework include files
#include <DDDigi/DigiParallelWorker.h>

/// C/C++ include files
#include <cstdint>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    template <typename T> class DigiParallelWorkers;
    template <typename T> class DigiParallelWorkerGroup;

    /// Wrapper class to submit bulk actions
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    template <typename T> class DigiParallelWorkerGroup  {
      /// Reference to worker collection with semaphore
      const DigiParallelWorkers<T>& workers;

    public:
      /// Initializin constructor: aquire worker's semaphore
      DigiParallelWorkerGroup(const DigiParallelWorkers<T>& workers);
      /// Inhibit Move constructor
      DigiParallelWorkerGroup(DigiParallelWorkerGroup&& copy) = delete;
      /// Inhibit copy constructor
      DigiParallelWorkerGroup(const DigiParallelWorkerGroup& copy) = delete;
      /// Inhibit move assignment
      DigiParallelWorkerGroup& operator=(DigiParallelWorkerGroup&& copy) = delete;
      /// Inhibit copy assignment
      DigiParallelWorkerGroup& operator=(const DigiParallelWorkerGroup& copy) = delete;
      /// Default destructor. Releasing semaphore
      ~DigiParallelWorkerGroup();
      /// Access the worker array. As long as the object persists it shall not be altered
      operator ParallelWorker*const* ();
    };
    
    /// Initializin constructor: aquire worker's semaphore
    template <typename T> inline 
      DigiParallelWorkerGroup<T>::DigiParallelWorkerGroup(const DigiParallelWorkers<T>& w)
      : workers(w) 
      {
	workers.semaphore.aquire();
      }

    /// Default destructor. Releasing semaphore
    template <typename T> inline DigiParallelWorkerGroup<T>::~DigiParallelWorkerGroup()   {
      workers.semaphore.release();
    }

    /// Access the worker array. As long as the object persists the array stays intact
    template <typename T> inline DigiParallelWorkerGroup<T>::operator ParallelWorker*const* ()   {
      return (ParallelWorker**)&this->workers.actors.at(0);
    }

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIPARALLELWORKERGROUP_H
