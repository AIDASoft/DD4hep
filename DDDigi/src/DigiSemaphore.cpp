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

// Framework include files
#include <DDDigi/DigiSemaphore.h>

using namespace dd4hep::digi;

/// Wait until reference count is NULL
std::unique_lock<std::mutex> DigiSemaphore::wait_null()  {
  std::unique_lock<std::mutex> protect(this->lock);
  this->reference_count_is_null.wait(protect, [this] { return this->reference_count == 0; } );
  return protect;
}

/// Acquire semaphore count
void DigiSemaphore::aquire()   {
  std::lock_guard guard(this->lock);
  ++this->reference_count;
}

/// Release semaphore count
void DigiSemaphore::release()  {
  if ( --this->reference_count == 0 )
    this->reference_count_is_null.notify_one();
}
