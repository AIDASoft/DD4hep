// $Id$
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
#include "DDCond/ConditionsInterna.h"
#include "DDCond/AlignmentsLoaderImp.h"

using namespace DD4hep::Conditions;
using DD4hep::Alignments::Alignment;
using DD4hep::Alignments::AlignmentCondition;

/// Default constructor
AlignmentsLoaderImp::AlignmentsLoaderImp(ConditionsManager mgr)
  : m_manager(mgr), m_refCount(1)
{
}

/// Default destructor
AlignmentsLoaderImp::~AlignmentsLoaderImp()   {
}

/// Addreference count. Use object
void AlignmentsLoaderImp::addRef()   {
  ++m_refCount;
}

/// Release object. The client may not use any reference any further.
void AlignmentsLoaderImp::release()    {
  if ( --m_refCount <= 0 )   {
    delete this;
  }
}

/// Access the conditions loading mechanism
Alignment AlignmentsLoaderImp::get(key_type key, const Condition::iov_type& iov)  {
  AlignmentCondition cond = m_manager.ptr()->get(key, iov);
  return &cond.data();
}

/// Access the conditions loading mechanism. Only conditions in the user pool will be accessed.
Alignment AlignmentsLoaderImp::get(key_type key, const UserPool& pool)   {
  AlignmentCondition cond = pool.get(key);
  return &cond.data();
}
