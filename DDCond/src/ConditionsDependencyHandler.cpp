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
#include "DDCond/ConditionsDependencyHandler.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DD4hep/Printout.h"

using namespace dd4hep;
using namespace dd4hep::cond;

/// Default constructor
ConditionsDependencyHandler::ConditionsDependencyHandler(ConditionsManager mgr,
                                                         UserPool& pool,
                                                         const Dependencies& dependencies,
                                                         ConditionUpdateUserContext* user_param)
  : m_manager(mgr.access()), m_pool(pool), m_dependencies(dependencies),
    m_userParam(user_param), num_callback(0)
{
  const IOV& iov = m_pool.validity();
  m_iovPool = m_manager->registerIOV(*iov.iovType, iov.keyData);
}

/// Default destructor
ConditionsDependencyHandler::~ConditionsDependencyHandler()   {
}

/// ConditionResolver implementation: Access to the detector description instance
Detector& ConditionsDependencyHandler::detectorDescription() const  {
  return m_manager->detectorDescription();
}

/// 1rst pass: Compute/create the missing conditions
void ConditionsDependencyHandler::compute()   {
  m_state = CREATED;
  for( const auto& i : m_dependencies )   {
    const ConditionDependency* dep = i.second;
    Condition c = m_pool.get(dep->key());
    // If we would know, that dependencies are only ONE level, we could skip this search....
    if ( !c.isValid() )  {
      do_callback(dep);
      continue;
    }
    // printout(INFO,"UserPool","Already calcluated: %s",d->name());
    continue;
  }
}

/// 2nd pass:  Handler callback for the second turn to resolve missing dependencies
void ConditionsDependencyHandler::resolve()    {
  m_state = RESOLVED;
  for( auto& c : m_created )   {
    if ( c.second.state == RESOLVED ) continue;
    c.second.state = RESOLVED;
    c.second.dependency->callback->resolve(c.second.condition, *this);    
  }
}

/// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
std::vector<Condition> ConditionsDependencyHandler::get(DetElement de)   {
  return this->get(de.key());
}

/// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
std::vector<Condition> ConditionsDependencyHandler::get(Condition::detkey_type det_key)   {
  if ( m_state == RESOLVED )   {
    ConditionKey::KeyMaker lower(det_key, 0);
    ConditionKey::KeyMaker upper(det_key, ~0x0);
    return m_pool.get(lower.hash, upper.hash);
  }
  except("ConditionsDependencyHandler",
         "Conditions bulk accesses are only possible during conditions resolution!");
  return std::vector<Condition>();
}

/// ConditionResolver implementation: Interface to access conditions
Condition ConditionsDependencyHandler::get(Condition::key_type key)  {
  /// Check if the required condition is one of the newly created ones:
  auto e = m_created.find(key);
  if ( e != m_created.end() )  {
    Created& c = (*e).second;
    if ( c.state == CREATED )  {
      c.state = RESOLVED;
      c.dependency->callback->resolve(c.condition, *this);
      return c.condition;
    }
  }
  /// If we are not already resolving here, we follow the normal procedure
  Condition c = m_pool.get(key);
  if ( c.isValid() )  {
    Condition::Object* obj = c.ptr();
    const IOV& required = m_pool.validity();
    if ( obj && obj->iov && IOV::key_is_contained(required.keyData,obj->iov->keyData) )
      return c;
    /// We should with normal pools never end up here, because the existing conditions
    /// in a reasonable pool are valid for the IOV and we should have returned above.
    Dependencies::const_iterator i = m_dependencies.find(key);
    if ( i != m_dependencies.end() )  {
      /// This condition is no longer valid. remove it! Will be added again afterwards.
      const ConditionDependency* dep = (*i).second;
      m_pool.remove(key);
      return do_callback(dep);
    }
  }
  Dependencies::const_iterator i = m_dependencies.find(key);
  if ( i != m_dependencies.end() )   {
    const ConditionDependency* dep = (*i).second;
    return do_callback(dep);
  }
  return Condition();
}


/// Internal call to trigger update callback
Condition::Object* 
ConditionsDependencyHandler::do_callback(const ConditionDependency* dep)   {
  try  {
    IOV iov(m_pool.validity().iovType);
    ConditionUpdateContext ctxt(this, dep, m_userParam, &iov.reset().invert());
    Condition          cond = (*dep->callback)(dep->target, ctxt);
    Condition::Object* obj  = cond.ptr();
    if ( obj )  {
      obj->hash = dep->target.hash;
      cond->setFlag(Condition::DERIVED);
      cond->iov = m_pool.validityPtr();
      // Must IMMEDIATELY insert to handle inter-dependencies.
      ++num_callback;
      m_created[dep->target.hash] = Created(dep, obj);
      m_pool.insert(cond);
      m_manager->registerUnlocked(*m_iovPool, cond);
    }
    return obj;
  }
  catch(const std::exception& e)   {
    printout(ERROR,"ConditionDependency",
             "+++ Exception while creating dependent Condition %s:",
             dep->name());
    printout(ERROR,"ConditionDependency","\t\t%s", e.what());
  }
  catch(...)   {
    printout(ERROR,"ConditionDependency",
             "+++ UNKNOWN exception while creating dependent Condition %s.",
             dep->name());
  }
  m_pool.print("*");
  except("ConditionDependency",
         "++ Exception while creating dependent Condition %s.",
         dep->name());
  return 0;
}
