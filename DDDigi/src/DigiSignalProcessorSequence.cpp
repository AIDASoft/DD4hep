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
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiSignalProcessorSequence.h"

// C/C++ include files
#include <stdexcept>

using namespace dd4hep::digi;

/// Standard constructor
DigiSignalProcessorSequence::DigiSignalProcessorSequence(const DigiKernel& kernel, const std::string& nam)
  : DigiSignalProcessor(kernel, nam)
{
  InstanceCount::increment(this);
}

/// Default destructor
DigiSignalProcessorSequence::~DigiSignalProcessorSequence() {
  InstanceCount::decrement(this);
}

/// Adopt a new action as part of the sequence. Sequence takes ownership.
void DigiSignalProcessorSequence::adopt(DigiSignalProcessor* action)    {
  if ( action )    {
    action->addRef();
    m_actors.add(action);
    return;
  }
  except("DigiSignalProcessorSequence","++ Attempt to add invalid actor!");
}

/// Pre-track action callback
double DigiSignalProcessorSequence::operator()(const DigiCellData& data)  const   {
  double result = data.raw_value;
  for ( const auto* p : m_actors )
    result += p->operator()(data);
  return data.kill ? 0e0 : result;
}
