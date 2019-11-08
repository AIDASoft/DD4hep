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
#include "DD4hep/Detector.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionDerived.h"

// C/C++ include files

using namespace dd4hep;
using namespace dd4hep::cond;

/// Default destructor
ConditionUpdateUserContext::~ConditionUpdateUserContext()   {
}

/// Access to the top level detector element
DetElement ConditionUpdateContext::world()  const   {
  return resolver->detectorDescription().world();
}

/// Access to all conditions of a detector element. Careful: This limits the validity!
std::vector<Condition> ConditionUpdateContext::conditions(Condition::detkey_type det_key)  const   {
  std::vector<Condition> v = resolver->get(det_key);
  /// Update result IOV according by and'ing the new iov structure
  for(Condition c : v) iov->iov_intersection(c.iov());
  return v;
}

/// Access conditions by the condition item key
std::vector<Condition> ConditionUpdateContext::getByItem(Condition::itemkey_type item_key)   const    {
  std::vector<Condition> v = resolver->getByItem(item_key);
  /// Update result IOV according by and'ing the new iov structure
  for(Condition c : v) iov->iov_intersection(c.iov());
  return v;
}

/// Access to condition object by dependency key
Condition ConditionUpdateContext::condition(const ConditionKey& key_value)  const  {
  Condition c = resolver->get(key_value);
  if ( c.isValid() )  {
    /// Update result IOV according by and'ing the new iov structure
    iov->iov_intersection(c.iov());
    return c;
  }
#if defined(DD4HEP_CONDITIONS_DEBUG) || defined(DD4HEP_CONDITIONKEY_HAVE_NAME)
  except("ConditionUpdateCall:","Failed to access non-existing condition:"+key_value.name);
#else
  ConditionKey::KeyMaker key(key_value.hash);
  except("ConditionUpdateCall:","Failed to access non-existing condition with key [%08X %08X]:",
         key.values.det_key, key.values.item_key);
#endif
  return Condition();
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

/// Interface to handle multi-condition inserts by callbacks: One single insert
bool ConditionUpdateContext::registerOne(const IOV& iov_val, Condition cond)   {
  return resolver->registerOne(iov_val, cond);
}

/// Handle multi-condition inserts by callbacks: block insertions of conditions with identical IOV
size_t ConditionUpdateContext::registerMany(const IOV& iov_val, const std::vector<Condition>& values)  {
  return resolver->registerMany(iov_val, values);
}

/// Standard destructor
ConditionUpdateCall::ConditionUpdateCall()  {
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
#if defined(DD4HEP_CONDITIONS_DEBUG) || defined(DD4HEP_CONDITIONKEY_HAVE_NAME)
  except("ConditionUpdateCall",
         "%s [%016llX]: FAILED to access non-existing item:%s [%016llX]",
         dependency->target.name.c_str(), dependency->target.hash,
         key_value.name.c_str(), key_value.hash);
#else
  ConditionKey::KeyMaker key(key_value.hash);
  ConditionKey::KeyMaker dep(dependency->target.hash);
  except("ConditionUpdateCall",
         "Derived condition [%08X %08X]: FAILED to access non-existing item:%s [%08X %08X]",
         dep.values.det_key, dep.values.item_key, key.values.det_key, key.values.item_key);
#endif
}

/// Initializing constructor
ConditionDependency::ConditionDependency(Condition::key_type  key,
                                         std::shared_ptr<ConditionUpdateCall> call)
  : m_refCount(0), target(key), callback(std::move(call))
{
  InstanceCount::increment(this);
}

/// Initializing constructor
ConditionDependency::ConditionDependency(Condition::detkey_type det_key,
                                         Condition::itemkey_type item_key,
                                         std::shared_ptr<ConditionUpdateCall> call)
  : m_refCount(0), target(det_key, item_key), callback(std::move(call))
{
  InstanceCount::increment(this);
}

/// Initializing constructor
ConditionDependency::ConditionDependency(DetElement              de,
                                         Condition::itemkey_type item_key,
                                         std::shared_ptr<ConditionUpdateCall> call)
  : m_refCount(0), 
#ifdef DD4HEP_CONDITIONS_DEBUG
  detector(de),
#endif
  target(de, item_key), callback(std::move(call))
{
  InstanceCount::increment(this);
}

/// Initializing constructor
ConditionDependency::ConditionDependency(DetElement de,
                                         const std::string&   item, 
                                         std::shared_ptr<ConditionUpdateCall> call)
  : 
#ifdef DD4HEP_CONDITIONS_DEBUG
  detector(de),
#endif
  target(de, item), callback(std::move(call))
{
  InstanceCount::increment(this);
}

/// Default constructor
ConditionDependency::ConditionDependency()
{
  InstanceCount::increment(this);
}

/// Default destructor
ConditionDependency::~ConditionDependency()
{
  InstanceCount::decrement(this);
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(DetElement           de,
                                     unsigned int         item_key,
                                     std::shared_ptr<ConditionUpdateCall> call)
  : m_dependency(new ConditionDependency(de,item_key,std::move(call)))
{
}

/// Initializing constructor
DependencyBuilder::DependencyBuilder(DetElement           de,
                                     const std::string&   item,
                                     std::shared_ptr<ConditionUpdateCall> call)
  : m_dependency(new ConditionDependency(de,item,std::move(call)))
{
}

/// Default destructor
DependencyBuilder::~DependencyBuilder()   {
}

/// Add a new dependency
void DependencyBuilder::add(const ConditionKey& source)   {
  if ( m_dependency )   {
    m_dependency->dependencies.emplace_back(source);
    return;
  }
  except("Dependency","++ Invalid object. No further source may be added!");
}

/// Release the created dependency and take ownership.
ConditionDependency* DependencyBuilder::release()   {
  if ( m_dependency )   {
    return m_dependency.release();
  }
  except("Dependency","++ Invalid object. Cannot access built objects!");
  return m_dependency.release(); // Not necessary, but need to satisfy compiler
}

