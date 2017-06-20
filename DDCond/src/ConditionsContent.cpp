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
#include "DDCond/ConditionsContent.h"
#include "DD4hep/InstanceCount.h"

using namespace dd4hep::cond;

/// Default destructor. 
ConditionsLoadInfo::~ConditionsLoadInfo()  {
}

/// Initializing constructor
ConditionsContent::ConditionsContent()
{
  InstanceCount::increment(this);  
}

/// Default destructor. 
ConditionsContent::~ConditionsContent()   {
  detail::releaseObjects(m_derived);
  detail::destroyObjects(m_conditions);
  InstanceCount::decrement(this);  
}

/// Clear the container. Destroys the contained stuff
void ConditionsContent::clear()   {
  detail::releaseObjects(m_derived);
  detail::destroyObjects(m_conditions);
}

/// Remove a new shared condition
bool ConditionsContent::remove(Condition::key_type hash)   {
  auto i = m_conditions.find(hash);
  if ( i != m_conditions.end() )  {
    detail::deleteObject((*i).second);
    m_conditions.erase(i);
    return true;
  }
  auto j = m_derived.find(hash);
  if ( j != m_derived.end() )  {
    detail::releasePtr((*j).second);
    m_derived.erase(j);
    return true;
  }
  return false;
}
