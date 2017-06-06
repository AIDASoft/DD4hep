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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionDerived.h"

// C/C++ include files

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Standard destructor
ConditionUpdateCall::ConditionUpdateCall() : m_refCount(1)  {
  InstanceCount::increment(this);
}

/// Standard destructor
ConditionUpdateCall::~ConditionUpdateCall()  {
  InstanceCount::decrement(this);
}

/// Standard destructor
ConditionResolver::~ConditionResolver()  {
}

/// Initializing constructor
ConditionDependency::ConditionDependency(Geometry::DetElement de,
                                         unsigned int         item_key,
                                         ConditionUpdateCall* call)
  : m_refCount(0), detector(de), target(de, item_key), callback(call)
{
  InstanceCount::increment(this);
  if ( callback ) callback->addRef();
}

/// Initializing constructor
ConditionDependency::ConditionDependency(Geometry::DetElement de,
                                         const std::string&   item, 
                                         ConditionUpdateCall* call)
  : m_refCount(0), detector(de), target(de, item), callback(call)
{
  InstanceCount::increment(this);
  if ( callback ) callback->addRef();
}

/// Default constructor
ConditionDependency::ConditionDependency()
  : m_refCount(0), target(0), callback(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
ConditionDependency::~ConditionDependency()  {
  InstanceCount::decrement(this);
  releasePtr(callback);
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(Geometry::DetElement de,
                                     unsigned int         item_key,
                                     ConditionUpdateCall* call)
  : m_dependency(new ConditionDependency(de,item_key,call))
{
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(Geometry::DetElement de,
                                     const std::string&   item,
                                     ConditionUpdateCall* call)
  : m_dependency(new ConditionDependency(de,item,call))
{
}

/// Default destructor
DependencyBuilder::~DependencyBuilder()   {
  releasePtr(m_dependency);
}

/// Add a new dependency
void DependencyBuilder::add(const ConditionKey& source)   {
  if ( m_dependency )   {
    m_dependency->dependencies.push_back(source);
    return;
  }
  except("Dependency","++ Invalid object. No further source may be added!");
}

/// Release the created dependency and take ownership.
ConditionDependency* DependencyBuilder::release()   {
  if ( m_dependency )   {
    ConditionDependency* tmp = m_dependency;
    m_dependency = 0;
    return tmp;
  }
  except("Dependency","++ Invalid object. Cannot access built objects!");
  return m_dependency; // Not necessary, but need to satisfy compiler
}

