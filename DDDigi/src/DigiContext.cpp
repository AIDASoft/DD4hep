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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiKernel.h"

// C/C++ include files
#include <algorithm>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::digi;

/// Default constructor
DigiContext::DigiContext(const DigiKernel* k, DigiEvent* e)
  : m_kernel(k), m_event(e)
{
  if ( !m_kernel )    {
    except("DigiContext","Cannot initialize Digitization context with invalid DigiKernel!");
  }
  InstanceCount::increment(this);
}

/// Default destructor
DigiContext::~DigiContext() {
  // Do not delete run and event structures here. This is done outside in the framework
  InstanceCount::decrement(this);
}

/// Set the geant4 event reference
void DigiContext::setEvent(DigiEvent* new_event)   {
  m_event = new_event;
}

/// Access the geant4 event -- valid only between BeginEvent() and EndEvent()!
DigiEvent& DigiContext::event()  const   {
  if ( m_event ) return *m_event;
  invalidHandleError<DigiEvent>();
  return *m_event;
}

/// Access to detector description
Detector& DigiContext::detectorDescription()  const {
  return m_kernel->detectorDescription();
}

/// Generic framework access
DigiContext::UserFramework& DigiContext::userFramework()  const  {
  return m_kernel->userFramework();
}

/// Access to the main input action sequence from the kernel object
DigiActionSequence& DigiContext::inputAction() const    {
  return m_kernel->inputAction();
}

/// Access to the main event action sequence from the kernel object
DigiActionSequence& DigiContext::eventAction()  const  {
  return m_kernel->eventAction();
}

/// Access to the main output action sequence from the kernel object
DigiActionSequence& DigiContext::outputAction() const    {
  return m_kernel->outputAction();
}

