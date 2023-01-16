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
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiInputAction.h>

// C/C++ include files
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace dd4hep::digi;

/// Standard constructor
DigiInputAction::DigiInputAction(const DigiKernel& kernel, const string& nam)
  : DigiEventAction(kernel, nam)
{
  declareProperty("input",            m_input_sources);
  declareProperty("segment",          m_input_segment);
  declareProperty("mask",             m_input_mask);
  declareProperty("rescan",           m_input_rescan);
  declareProperty("input_section",    m_input_section);
  declareProperty("objects_enabled",  m_objects_enabled);
  declareProperty("objects_disabled", m_objects_disabled);
  InstanceCount::increment(this);
}

/// Default destructor
DigiInputAction::~DigiInputAction()   {
  InstanceCount::decrement(this);
}

/// Check if a event object should be loaded: Default YES unless inhibited by selection or veto
bool DigiInputAction::object_loading_is_enabled(const std::string& nam)  const   {
  /// If there are no required branches, we convert everything
  if ( m_objects_enabled.empty() && m_objects_disabled.empty() )    {
    return true;
  }
  /// Check for disabled collections:
  for( const auto& bname : m_objects_disabled )    {
    if ( bname == nam )
      return false;
  }
  /// No selection to be performed: take them all
  if ( m_objects_enabled.empty() )    {
    return true;
  }
  /// .... Otherwise only the entities asked for
  for( const auto& bname : m_objects_enabled )    {
    if ( bname == nam )   {
      return true;
    }
  }
  return false;
}

/// Pre-track action callback
void DigiInputAction::execute(DigiContext& /* context */)  const   {
  info("+++ Virtual method execute() --- Should not be called");
  ::sleep(1);
}
