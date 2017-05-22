//==========================================================================
//  AIDA Detector description implementation for LCD
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

using namespace DD4hep::Conditions;

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
  releaseObjects(m_derived);
  destroyObjects(m_conditions);
  InstanceCount::decrement(this);  
}

/// Clear the container. Destroys the contained stuff
void ConditionsContent::clear()   {
  releaseObjects(m_derived);
  destroyObjects(m_conditions);
}

/// Remove a new shared condition
bool ConditionsContent::remove(Condition::key_type hash)   {
  Conditions::iterator i = m_conditions.find(hash);
  if ( i != m_conditions.end() )  {
    deleteObject((*i).second);
    m_conditions.erase(i);
    return true;
  }
  Dependencies::iterator j = m_derived.find(hash);
  if ( j != m_derived.end() )  {
    releasePtr((*j).second);
    m_derived.erase(j);
    return true;
  }
  return false;
}
