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
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsLoaderImp.h"
#include "DDCond/ConditionsManagerObject.h"

using namespace DD4hep::Conditions;

/// Default constructor
ConditionsLoaderImp::ConditionsLoaderImp(ConditionsManager mgr)
  : m_manager(mgr), m_refCount(1)
{
}

/// Default destructor
ConditionsLoaderImp::~ConditionsLoaderImp()   {
}

/// Addreference count. Use object
void ConditionsLoaderImp::addRef()   {
  ++m_refCount;
}

/// Release object. The client may not use any reference any further.
void ConditionsLoaderImp::release()    {
  if ( --m_refCount <= 0 )   {
    delete this;
  }
}

/// Access the conditions loading mechanism
Condition ConditionsLoaderImp::get(key_type key, const Condition::iov_type& iov)  {
  return m_manager.ptr()->get(key, iov);
}

/// Access the conditions loading mechanism. Only conditions in the user pool will be accessed.
Condition ConditionsLoaderImp::get(key_type key, const UserPool& pool)   {
  return pool.get(key);
}
