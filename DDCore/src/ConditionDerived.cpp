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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionDerived.h"

// C/C++ include files

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Standard destructor
ConditionUpdateCall::~ConditionUpdateCall()  {
}

/// Standard destructor
ConditionResolver::~ConditionResolver()  {
}

/// Default constructor
ConditionDependency::ConditionDependency(const ConditionKey& tar, 
                                         const Dependencies deps, 
                                         ConditionUpdateCall* call)
  : m_refCount(0), target(tar), dependencies(deps), callback(call)
{
  InstanceCount::increment(this);
  if ( callback ) callback->addRef();
}

/// Initializing constructor
ConditionDependency::ConditionDependency(const ConditionKey& tar, 
                                         ConditionUpdateCall* call)
  : m_refCount(0), target(tar), callback(call)
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

/// Copy constructor
ConditionDependency::ConditionDependency(const ConditionDependency& c)
  : m_refCount(0), target(c.target),
    dependencies(c.dependencies),
    callback(c.callback)
{
  InstanceCount::increment(this);
  if ( callback ) callback->addRef();
  except("Dependency",
         "++ Condition: %s. Dependencies may not be assigned or copied!",
         target.name.c_str());
}

/// Default destructor
ConditionDependency::~ConditionDependency()  {
  InstanceCount::decrement(this);
  releasePtr(callback);
}

/// Assignment operator
ConditionDependency& ConditionDependency::operator=(const ConditionDependency& )  {
  except("Dependency",
         "++ Condition: %s. Dependencies may not be assigned or copied!",
         target.name.c_str());
  return *this;
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(const ConditionKey& target,
                                     ConditionUpdateCall* call)
  : m_dependency(new ConditionDependency(target,call))
{
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(const ConditionKey& target,
                                     ConditionUpdateCall* call,
                                     Geometry::DetElement de)
  : m_dependency(new ConditionDependency(target,call))
{
  m_dependency->detector = de;
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

