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
#include "DDCond/ConditionsDependencyHandler.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DD4hep/Printout.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default constructor
ConditionsDependencyHandler::ConditionsDependencyHandler(ConditionsManager mgr,
                                                         UserPool& pool,
                                                         const Dependencies& dependencies,
                                                         void* user_param)
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
LCDD& ConditionsDependencyHandler::lcdd() const  {
  return m_manager->lcdd();
}

/// ConditionResolver implementation: Interface to access conditions
Condition ConditionsDependencyHandler::get(Condition::key_type key)  const  {
  Condition c = m_pool.get(key);
  if ( c.isValid() )  {
    Condition::Object* obj = c.ptr();
    const IOV& required = m_pool.validity();
    if ( obj && obj->iov && IOV::key_is_contained(required.keyData,obj->iov->keyData) )
      return c;
    Dependencies::const_iterator i = m_dependencies.find(key);
    if ( i != m_dependencies.end() )  {
      /// This condition is no longer valid. remove it! Will be added again afterwards.
      const ConditionDependency* dep = (*i).second.get();
      m_pool.remove(key);
      return do_callback(*dep);
    }
  }
  Dependencies::const_iterator i = m_dependencies.find(key);
  if ( i != m_dependencies.end() )   {
    const ConditionDependency* dep = (*i).second.get();
    return do_callback(*dep);
  }
  return Condition();
}


/// Internal call to trigger update callback
Condition::Object* 
ConditionsDependencyHandler::do_callback(const ConditionDependency& dep)  const {
  try  {
    Condition::iov_type iov(m_pool.validity().iovType);
    ConditionUpdateCall::Context ctxt(*this, dep, m_userParam, iov.reset().invert());
    Condition          cond = (*dep.callback)(dep.target, ctxt);
    Condition::Object* obj  = cond.ptr();
    if ( obj )  {
      if ( !obj->hash ) obj->hash = ConditionKey::hashCode(obj->name);
      cond->setFlag(Condition::DERIVED);
      //cond->validate();
      cond->iov = m_pool.validityPtr();
      // Must IMMEDIATELY insert to handle inter-dependencies.
      ++num_callback;
      m_pool.insert(cond);
      m_manager->registerUnlocked(m_iovPool, cond);
    }
    return obj;
  }
  catch(const std::exception& e)   {
    printout(ERROR,"ConditionDependency",
             "+++ Exception while creating dependent Condition %s:",
             dep.name());
    printout(ERROR,"ConditionDependency","\t\t%s", e.what());
  }
  catch(...)   {
    printout(ERROR,"ConditionDependency",
             "+++ UNKNOWN exception while creating dependent Condition %s.",
             dep.name());
  }
  m_pool.print("*");
  except("ConditionDependency",
         "++ Exception while creating dependent Condition %s.",
         dep.name());
  return 0;
}

/// Handler callback to process multiple derived conditions
Condition::Object* ConditionsDependencyHandler::operator()(const ConditionDependency* dep)  const   {
  return do_callback(*dep);
}
