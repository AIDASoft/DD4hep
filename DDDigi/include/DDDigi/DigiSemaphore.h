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
#ifndef DDDIGI_DIGISEMAPHORE_H
#define DDDIGI_DIGISEMAPHORE_H

/// Framework include files

/// C/C++ include files
#include <mutex>
#include <atomic>
#include <condition_variable>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiSemaphore;

    /// Semaphore implementation, where object get notified when the reference count reaches NULL
    /** Semaphore implementation
     *  object get notified when the reference count reaches NULL
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSemaphore  final {
    private:
      /// Reference count
      std::atomic<std::size_t> reference_count { 0UL };
      /// Condition variable to wait until reference count is NULL
      std::condition_variable  reference_count_is_null;
      /// Lock to handle condition variable
      std::mutex lock;

    public:
      /// Default constructor: the one and only one
      DigiSemaphore() = default;
      /// Inhibit Move constructor
      DigiSemaphore(DigiSemaphore&& copy) = delete;
      /// Inhibit copy constructor
      DigiSemaphore(const DigiSemaphore& copy) = delete;
      /// Inhibit move assignment
      DigiSemaphore& operator=(DigiSemaphore&& copy) = delete;
      /// Inhibit copy assignment
      DigiSemaphore& operator=(const DigiSemaphore& copy) = delete;
      /// Default destructor. Decreasing reference count.
      ~DigiSemaphore() = default;

    public:
      /// Wait until reference count is NULL.
      /** @return unique_lock  [out] The semaphore is locks as long as the returned lock is held
       */
      std::unique_lock<std::mutex> wait_null();
      /// Aquire semaphore count
      void aquire();
      /// Release semaphore count
      void release();
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISEMAPHORE_H
