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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionDerived.h"

// C/C++ include files

using namespace dd4hep;
using namespace dd4hep::cond;

/// Default destructor
ConditionUpdateUserContext::~ConditionUpdateUserContext()   {
}

/// Access to condition object by dependency key
Condition ConditionUpdateContext::condition(const ConditionKey& key_value)  const  {
  Condition c = resolver->get(key_value);
  if ( c.isValid() )  {
    /// Update result IOV according by and'ing the new iov structure
    iov->iov_intersection(c.iov());
    return c;
  }
  throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing condition:"+key_value.name);
}
   
/// Access to all conditions of a detector element. Careful: This limits the validity!
std::vector<Condition> ConditionUpdateContext::conditions(Condition::detkey_type det_key)  const   {
  std::vector<Condition> v = resolver->get(det_key);
  for(Condition c : v)   {
    /// Update result IOV according by and'ing the new iov structure
    iov->iov_intersection(c.iov());
  }
  return v;
}

/// Access to condition object by dependency key
Condition ConditionUpdateContext::condition(Condition::key_type key_value)  const   {
  Condition c = resolver->get(key_value);
  if ( c.isValid() )  {
    /// Update result IOV according by and'ing the new iov structure
    iov->iov_intersection(c.iov());
    return c;
  }
  throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing condition.");
}

/// Access to condition object by dependency key
Condition ConditionUpdateContext::condition(Condition::key_type key_value,
                                            bool throw_if_not)  const   {
  Condition c = resolver->get(key_value, throw_if_not);
  if ( c.isValid() )  {
    /// Update result IOV according by and'ing the new iov structure
    iov->iov_intersection(c.iov());
    return c;
  }
  else if ( throw_if_not )  {
    throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing condition.");
  }
  return Condition();
}

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

/// Throw exception on conditions access failure
void ConditionUpdateContext::accessFailure(const ConditionKey& key_value)  const   {
  except("ConditionUpdateCall",
         "%s [%016llX]: FAILED to access non-existing item:%s [%016llX]",
         dependency->target.name.c_str(), dependency->target.hash,
         key_value.name.c_str(), key_value.hash);
}

/// Initializing constructor
ConditionDependency::ConditionDependency(DetElement           de,
                                         unsigned int         item_key,
                                         ConditionUpdateCall* call)
  : m_refCount(0), detector(de), target(de, item_key), callback(call)
{
  InstanceCount::increment(this);
}

/// Initializing constructor
ConditionDependency::ConditionDependency(DetElement de,
                                         const std::string&   item, 
                                         ConditionUpdateCall* call)
  : m_refCount(0), detector(de), target(de, item), callback(call)
{
  InstanceCount::increment(this);
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
  detail::releasePtr(callback);
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(DetElement           de,
                                     unsigned int         item_key,
                                     ConditionUpdateCall* call)
  : m_dependency(new ConditionDependency(de,item_key,call))
{
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(DetElement           de,
                                     const std::string&   item,
                                     ConditionUpdateCall* call)
  : m_dependency(new ConditionDependency(de,item,call))
{
}

/// Default destructor
DependencyBuilder::~DependencyBuilder()   {
  detail::releasePtr(m_dependency);
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

