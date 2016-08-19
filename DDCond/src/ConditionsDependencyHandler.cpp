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
#include "DD4hep/Printout.h"
#include "DDCond/ConditionsDependencyHandler.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default constructor
ConditionsDependencyHandler::ConditionsDependencyHandler(ConditionsManager::Object* mgr,
							 UserPool& pool,
							 const Dependencies& dependencies)
  : m_manager(mgr), m_pool(pool), m_dependencies(dependencies)
{
}

/// Default destructor
ConditionsDependencyHandler::~ConditionsDependencyHandler()   {
}

/// ConditionResolver implementation: Interface to access conditions
Condition ConditionsDependencyHandler::get(unsigned int key)  const  {
  Condition c = m_pool.get(key);
  if ( c.isValid() )  {
    Condition::Object* obj = c.ptr();
    const IOV& required = m_pool.validity();
    if ( obj->iov && IOV::key_is_contained(required.keyData,obj->iov->keyData) )
      return c;
    Dependencies::const_iterator i = m_dependencies.find(key);
    if ( i != m_dependencies.end() )  {
      /// This condition is no longer valid. remove it! Will be added again afterwards.
      m_pool.remove(key);
      return do_callback((*i).second);
    }
  }
  Dependencies::const_iterator i = m_dependencies.find(key);
  if ( i != m_dependencies.end() )
    return do_callback((*i).second);
  return Condition();
}


/// Internal call to trigger update callback
Condition::Object* 
ConditionsDependencyHandler::do_callback(const ConditionDependency* dep)  const {
  try  {
    Condition::iov_type iov(m_pool.validity().iovType);
    ConditionUpdateCall::Context ctxt(*this, *dep, iov.reset().invert());
    Condition          cond = (*dep->callback)(dep->target, ctxt);
    Condition::Object* obj = cond.ptr();
    if ( obj )  {
      if ( !obj->hash ) obj->hash = ConditionKey::hashCode(obj->name);
      cond->setFlag(Condition::DERIVED);
      cond->iov = m_pool.validityPtr();
      // Must IMMEDIATELY insert to handle inter-dependencies.
      m_pool.insert(cond);
    }
    return obj;
  }
  catch(const std::exception& e)   {
    printout(ERROR,"ConditionDependency",
	     "+++ Exception while creating dependent Condition %s:",
	     dep->target.name.c_str());
    printout(ERROR,"ConditionDependency","\t\t%s", e.what());
  }
  catch(...)   {
    printout(ERROR,"ConditionDependency",
	     "+++ UNKNOWN exception while creating dependent Condition %s.",
	     dep->target.name.c_str());
  }
  m_pool.print("*");
  except("ConditionDependency",
	 "++ Exception while creating dependent Condition %s.",
	 dep->target.name.c_str());
  return 0;
}

/// Handler callback to process multiple derived conditions
Condition::Object* ConditionsDependencyHandler::operator()(const ConditionDependency* dep)  const   {
  return do_callback(dep);
}
