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
#include <DD4hep/Printout.h>
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiKernel.h>

// C/C++ include files
#include <algorithm>

using namespace dd4hep::digi;

/// Default constructor
DigiContext::DigiContext(const DigiKernel& k, std::unique_ptr<DigiEvent>&& e)
  : kernel(k), event(std::move(e))
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiContext::~DigiContext() {
  // Do not delete run and event structures here. This is done outside in the framework
  InstanceCount::decrement(this);
}

/// Have a shared initializer lock
std::mutex& DigiContext::initializer_lock()  const  {
  return kernel.initializer_lock();
}

/// Have a global I/O lock (e.g. for ROOT)
std::mutex& DigiContext::global_io_lock()   const  {
  return kernel.global_io_lock();
}

/// Have a global output log lock
std::mutex& DigiContext::global_output_lock()   const  {
  return kernel.global_output_lock();
}

/// Access to detector description
dd4hep::Detector& DigiContext::detectorDescription()  const {
  return kernel.detectorDescription();
}

/// Generic framework access
DigiContext::UserFramework& DigiContext::userFramework()  const  {
  return kernel.userFramework();
}

/// Access to the main input action sequence from the kernel object
DigiActionSequence& DigiContext::inputAction() const    {
  return kernel.inputAction();
}

/// Access to the main event action sequence from the kernel object
DigiActionSequence& DigiContext::eventAction()  const  {
  return kernel.eventAction();
}

/// Access to the main output action sequence from the kernel object
DigiActionSequence& DigiContext::outputAction() const    {
  return kernel.outputAction();
}

