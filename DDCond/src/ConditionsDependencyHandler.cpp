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

namespace {
  std::string dependency_name(const ConditionDependency* d)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
    return d->target.name;
#else
    char text[64];
    ConditionKey::KeyMaker key(d->target.hash);
    ::snprintf(text,sizeof(text),"%08X %08X",key.values.det_key, key.values.item_key);
    return text;
#endif
  }

}

/// Default constructor
ConditionsDependencyHandler::ConditionsDependencyHandler(ConditionsManager mgr,
                                                         UserPool& pool,
                                                         const Dependencies& dependencies,
                                                         ConditionUpdateUserContext* user_param)
  : m_manager(mgr.access()), m_pool(pool), m_dependencies(dependencies),
    m_userParam(user_param), num_callback(0)
{
  const IOV& iov = m_pool.validity();
  //IOV full_iov(iov.iovType,IOV::Key(IOV::MIN_KEY,IOV::MAX_KEY));
  IOV full_iov(iov.iovType,IOV::Key(0,detail::makeTime(2099,12,31)));
  m_iovPool = m_manager->registerIOV(*iov.iovType, iov.keyData);
  unsigned char* p = new unsigned char[dependencies.size()*sizeof(Work)];
  Dependencies::const_iterator idep = dependencies.begin();
  m_block = (Work*)p;
  for(size_t i=0; i<dependencies.size(); ++i, ++idep, p+=sizeof(Work))  {
    Work* w = new(p) Work(this,(*idep).second,user_param,full_iov);
    m_todo.insert(std::make_pair((*idep).first,w));
  }
}

/// Default destructor
ConditionsDependencyHandler::~ConditionsDependencyHandler()   {
  m_todo.clear();
  m_created.clear();
  if ( m_block ) delete [] m_block;
  m_block = 0;
}

/// ConditionResolver implementation: Access to the detector description instance
Detector& ConditionsDependencyHandler::detectorDescription() const  {
  return m_manager->detectorDescription();
}

/// 1rst pass: Compute/create the missing conditions
void ConditionsDependencyHandler::compute()   {
  m_state = CREATED;
  for( const auto& i : m_todo )   {
    if ( !i.second->condition )  {
      do_callback(i.second);
      continue;
    }
    // printout(INFO,"UserPool","Already calcluated: %s",d->name());
    continue;
  }
}

/// 2nd pass:  Handler callback for the second turn to resolve missing dependencies
void ConditionsDependencyHandler::resolve()    {
  size_t num_resolved = 0;
  std::map<IOV::Key,ConditionsPool*> pools;
  m_state = RESOLVED;
  for( auto& c : m_created )   {
    Work* w = c.second;
    if ( w->state == RESOLVED ) continue;
    w->state = RESOLVED;
    w->context.dependency->callback->resolve(w->condition, w->context);
    ++num_resolved;
  }
  // Optimize pool interactions: Cache pool in map assuming there are only few pools created
  for( auto& c : m_created ) {
    Work* w = c.second;
    ConditionsPool* p = 0;
    auto res = pools.insert(make_pair(w->iov.keyData,p));
    if ( res.second )   {
      p = m_manager->registerIOV(*w->iov.iovType, w->iov.keyData);
      (*res.first).second = p;
    }
    p = (*res.first).second;
    w->condition->iov = p->iov;
    m_manager->registerUnlocked(*p, w->condition);
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
Condition ConditionsDependencyHandler::get(Condition::key_type key, bool throw_if_not)  {
  /// Check if the required condition is one of the newly created ones:
  auto e = m_created.find(key);
  if ( e != m_created.end() )  {
    Work* w = (*e).second;
    if ( w->state == CREATED )  {
      w->state = RESOLVED;
      w->context.dependency->callback->resolve(w->condition, w->context);
      return w->condition;
    }
  }
  /// If we are not already resolving here, we follow the normal procedure
  Condition c = m_pool.get(key);
  if ( c.isValid() )  {
    return c;
  }
  auto i = m_todo.find(key);
  if ( i != m_todo.end() )   {
    c = do_callback((*i).second);
    if ( c.isValid() )  {
      return c;
    }
  }
  if ( throw_if_not )  {
    except("ConditionsDependencyHandler","Failed to resolve conditon:%16lX",key);
  }
  return Condition();
}


/// Internal call to trigger update callback
Condition::Object* 
ConditionsDependencyHandler::do_callback(Work* work)   {
  const ConditionDependency* dep  = work->context.dependency;
  try  {
    work->condition = (*dep->callback)(dep->target, work->context).ptr();
    if ( work->condition )  {
      work->condition->iov  = &work->iov;
      work->condition->hash = dep->target.hash;
      work->condition->setFlag(Condition::DERIVED);
      work->state = CREATED;
      //TEST  cond->iov = m_pool.validityPtr();
      // Must IMMEDIATELY insert to handle inter-dependencies.
      ++num_callback;
      m_created[dep->target.hash] = work;
      //TEST m_manager->registerUnlocked(*m_iovPool, work.condition);
      m_pool.insert(work->condition);
    }
    return work->condition;
  }
  catch(const std::exception& e)   {
    printout(ERROR,"ConditionDependency",
             "+++ Exception while creating dependent Condition %s:",
             dependency_name(dep).c_str());
    printout(ERROR,"ConditionDependency","\t\t%s", e.what());
  }
  catch(...)   {
    printout(ERROR,"ConditionDependency",
             "+++ UNKNOWN exception while creating dependent Condition %s.",
             dependency_name(dep).c_str());
  }
  m_pool.print("*");
  except("ConditionDependency",
         "++ Exception while creating dependent Condition %s.",
         dependency_name(dep).c_str());
  return 0;
}
