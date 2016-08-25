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
  : target(tar), dependencies(deps), callback(call)
{
}

/// Default constructor
ConditionDependency::ConditionDependency(const ConditionKey& tar, 
                                         ConditionUpdateCall* call)
  : target(tar), callback(call)
{
}

/// Initializing constructor
ConditionDependency::ConditionDependency()   {
}

/// Copy constructor
ConditionDependency::ConditionDependency(const ConditionDependency& c)
  : target(c.target), dependencies(c.dependencies)
{
  except("Dependency",
         "++ Condition: %s. Dependencies may not be assigned or copied!",
         target.name.c_str());
}

/// Default destructor
ConditionDependency::~ConditionDependency()  {
}

/// Assignment operator
ConditionDependency& ConditionDependency::operator=(const ConditionDependency& )  {
  except("Dependency",
         "++ Condition: %s. Dependencies may not be assigned or copied!",
         target.name.c_str());
  return *this;
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(const ConditionKey& target, ConditionUpdateCall* call)
  : dependency(new ConditionDependency(target,call))
{
}

/// Default destructor
DependencyBuilder::~DependencyBuilder()   {
}

/// Add a new dependency
void DependencyBuilder::add(const ConditionKey& source)   {
  ConditionDependency* dep = dependency.get();
  if ( dep )   {
    dep->dependencies.push_back(source);
    return;
  }
  except("Dependency","++ Invalid object. No further source may be added!");
}

/// Release the created dependency and take ownership.
ConditionDependency* DependencyBuilder::release()   {
  if ( dependency.get() )   {
    return dependency.release();
  }
  except("Dependency","++ Invalid object. Cannot access built objects!");
  return dependency.release(); // Not necessary, but need to satisfy compiler
}

