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
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/Printout.h"
#include "TTimeStamp.h"

using namespace dd4hep;
using namespace dd4hep::cond;

namespace {
  std::string dependency_name(const ConditionDependency* d)  {
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
    return d->target.name;
#else
    char text[64];
    ConditionKey::KeyMaker key(d->target.hash);
    ::snprintf(text,sizeof(text),"%08X %08X",key.values.det_key, key.values.item_key);
    return text;
#endif
  }
}

void ConditionsDependencyHandler::Work::do_intersection(const IOV* iov_ptr)   {
  if ( iov_ptr )   {
    if ( !this->iov )   {
      this->_iov = *iov_ptr;
      this->iov = &this->_iov;
    }
    else  {
      this->_iov.iov_intersection(*iov_ptr);
    }
  }
}

Condition ConditionsDependencyHandler::Work::resolve(Work*& current)   {
  Work* previous = current;
  current = this;
  state = RESOLVED;
  if ( !condition )   {
    printout(ERROR,"DependencyHandler","ERROR: Cannot resolve not existing conditions.");
  }
  context.dependency->callback->resolve(condition, context);
  previous->do_intersection(iov);
  current = previous;
  return condition;
}

/// Default constructor
ConditionsDependencyHandler::ConditionsDependencyHandler(ConditionsManager   mgr,
                                                         UserPool&           pool,
                                                         const Dependencies& dependencies,
                                                         ConditionUpdateUserContext* user_param)
  : m_manager(mgr.access()), m_pool(pool), m_dependencies(dependencies),
    m_userParam(user_param), num_callback(0)
{
  const IOV& iov = m_pool.validity();
  unsigned char* p = new unsigned char[dependencies.size()*sizeof(Work)];
  m_block = (Work*)p;
  for(const auto& d : dependencies)  {
    Work* w = new(p) Work(this,d.second,user_param,iov);
    m_todo.emplace(d.first,w);
    p += sizeof(Work);
  }
  m_iovType = iov.iovType;
}

/// Default destructor
ConditionsDependencyHandler::~ConditionsDependencyHandler()   {
  m_todo.clear();
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
      if ( !i.second->condition )  {
        except("DependencyHandler",
               "Derived condition was not created after calling the creation callback!");
      }
    }
    // printout(INFO,"UserPool","Already calcluated: %s",d->name());
  }
}

/// 2nd pass:  Handler callback for the second turn to resolve missing dependencies
void ConditionsDependencyHandler::resolve()    {
  PrintLevel prt_lvl = INFO;
  size_t num_resolved = 0;
  std::vector<Condition> tmp;
  std::map<IOV::Key,std::vector<Condition> > work_pools;
  Work* w;

  m_state = RESOLVED;
  for( const auto& c : m_todo )   {
    w = c.second;
    m_currentWork = w;
    if ( w->state != RESOLVED )   {
      w->resolve(m_currentWork);
    }
    ++num_resolved;
    // Fill an empty map of condition vectors for the block inserts
    auto ret = work_pools.emplace(w->iov->keyData,tmp);
    if ( ret.second )   {
      // There is sort of the hope that most conditions go into 1 pool...
      ret.first->second.reserve(m_todo.size());
    }
  }
  // Optimize pool interactions: Cache pool in map assuming there are only few pools created
  for( const auto& c : m_todo )   {
    w = c.second;
    auto& section = work_pools[w->iov->keyData];
    section.emplace_back(w->condition);
#if 0
    printout(prt_lvl,"DependencyHandler","++ Register %s %s %s  [%s]",
             w->context.dependency->target.toString().c_str(),
             w->context.dependency->detector.path().c_str(),
             w->condition->iov->str().c_str(),
             typeName(typeid(*w->condition)).c_str());
#endif
  }
  // Now block register all conditions to the manager AND to the user pool
  // In principle at thi stage the conditions manager should be locked
  // provided all the work done so far can be undone.....in case of an error
  for( const auto& section : work_pools )   {
    TTimeStamp start;
    IOV iov(m_iovType, section.first);
    size_t result = registerMany(iov, section.second);
    if ( result != section.second.size() )  {
      // 
    }
    TTimeStamp stop;
    printout(prt_lvl,"DependencyHandler","Inserted %ld [%ld] conditions to pool-iov: %s   [%7.5f seconds]",
             result, section.second.size(), iov.str().c_str(),
             stop.AsDouble()-start.AsDouble());
  }
}

/// Interface to handle multi-condition inserts by callbacks: One single insert
bool ConditionsDependencyHandler::registerOne(const IOV& iov, Condition cond)    {
  return m_pool.registerOne(iov, cond);
}

/// Handle multi-condition inserts by callbacks: block insertions of conditions with identical IOV
size_t ConditionsDependencyHandler::registerMany(const IOV& iov, const std::vector<Condition>& values)   {
  return m_pool.registerMany(iov, values);
}

/// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
std::vector<Condition> ConditionsDependencyHandler::get(DetElement de)   {
  return this->get(de.key());
}

/// Interface to access conditions by hash value of the item (only valid at resolve!)
std::vector<Condition> ConditionsDependencyHandler::getByItem(Condition::itemkey_type key)   {
  if ( m_state == RESOLVED )   {
    struct item_selector {
      std::vector<Condition>  conditions;
      Condition::itemkey_type key;
      item_selector(Condition::itemkey_type k) : key(k) {}
      int operator()(Condition cond)   {
        ConditionKey::KeyMaker maker(cond->hash);
        if ( maker.values.item_key == key ) conditions.emplace_back(cond);
        return 1;
      }
    };
    item_selector proc(key);
    m_pool.scan(conditionsProcessor(proc));
    for (auto c : proc.conditions ) m_currentWork->do_intersection(c->iov);
    return proc.conditions;
  }
  except("DependencyHandler",
         "Conditions bulk accesses are only possible during conditions resolution!");
  return std::vector<Condition>();
}

/// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
std::vector<Condition> ConditionsDependencyHandler::get(Condition::detkey_type det_key)   {
  if ( m_state == RESOLVED )   {
    ConditionKey::KeyMaker lower(det_key, Condition::FIRST_ITEM_KEY);
    ConditionKey::KeyMaker upper(det_key, Condition::LAST_ITEM_KEY);
    std::vector<Condition> conditions = m_pool.get(lower.hash, upper.hash);
    for (auto c : conditions ) m_currentWork->do_intersection(c->iov);
    return conditions;
  }
  except("DependencyHandler",
         "Conditions bulk accesses are only possible during conditions resolution!");
  return std::vector<Condition>();
}

/// ConditionResolver implementation: Interface to access conditions
Condition ConditionsDependencyHandler::get(Condition::key_type key, bool throw_if_not)  {
  return this->get(key, nullptr, throw_if_not);
}

/// ConditionResolver implementation: Interface to access conditions
Condition ConditionsDependencyHandler::get(Condition::key_type key,
                                           const ConditionDependency* dependency,
                                           bool throw_if_not)
{
  /// If we are not already resolving here, we follow the normal procedure
  Condition c = m_pool.get(key);
  if ( c.isValid() )  {
    m_currentWork->do_intersection(c->iov);
    return c;
  }
  auto i = m_todo.find(key);
  if ( i != m_todo.end() )   {
    Work* w = i->second;
    if ( w->state == RESOLVED )   {
      return w->condition;
    }
    else if ( w->state == CREATED )   {
      return w->resolve(m_currentWork);
    }
    else if ( w->state == INVALID )  {
      do_callback(w);
      if ( w->condition && w->state == RESOLVED ) // cross-dependencies...
        return w->condition;
      else if ( w->condition )
        return w->resolve(m_currentWork);
    }
  }
  if ( throw_if_not )  {
    if ( dependency )    {
      // We need here more elaborate printout to ease debugging capabilities
      std::string de_path   = dependency->detector.path();
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
      std::string cond_from = dependency->target.name;
      std::string cond_to   = "UNKNOWN";
      for(const auto& d : dependency->dependencies)    {
        if ( d.hash == key )    {
          cond_to = d.name;
          break;
        }
      }
      printout(ERROR,"DependencyHandler",
               "Failed to resolve conditon:%016lX for DetElement: %s",
               key, de_path.c_str());
      printout(ERROR,"DependencyHandler",
               "Condition: %s dependent on missing condition: %s",
               cond_from.c_str(), cond_to.c_str());
#else
      if ( detail::have_condition_item_inventory(-1) )   {
        std::string item_from = detail::get_condition_item_name(dependency->target.hash);
        std::string item_to   = detail::get_condition_item_name(key);
        printout(ERROR,"DependencyHandler",
                 "Failed to resolve conditon:%016lX for DetElement: %s",
                 key, de_path.c_str());
        printout(ERROR,"DependencyHandler",
                 "Condition: %s#%s dependent on missing condition item: %s",
                 de_path.c_str(), item_from.c_str(), item_to.c_str());
      }
#endif
      except("DependencyHandler",
             "Failed to resolve conditon:%016lX for DetElement: %s",
             key,de_path.c_str());
    }
    except("ConditionsDependencyHandler","Failed to resolve conditon:%016lX",key);
  }
  return Condition();
}

/// Internal call to trigger update callback
void ConditionsDependencyHandler::do_callback(Work* work)   {
  const ConditionDependency* dep = work->context.dependency;
  try  {
    Work* previous  = m_currentWork;
    m_currentWork   = work;
    if ( work->callstack > 0 )   {
      // if we end up here it means a previous construction call never finished
      // because the bugger tried to access another condition, which in turn
      // during the construction tries to access this one.
      // ---> Classic dead-lock
      except("DependencyHandler",
             "++ Handler caught in infinite recursion loop. Key:%s %c%s%c",
             work->context.dependency->target.toString().c_str(),
#if defined(DD4HEP_CONDITIONS_DEBUG)
             '[',work->context.dependency->detector.path().c_str(),']'
#else
             ' ',"",' '
#endif
             );
    }
    ++work->callstack;
    work->condition = (*dep->callback)(dep->target, work->context).ptr();
    --work->callstack;
    m_currentWork   = previous;
    if ( work->condition )  {
      if ( !work->iov )  {
        work->_iov = IOV(m_iovType,IOV::Key(IOV::MIN_KEY, IOV::MAX_KEY));
        work->iov  = &work->_iov;
      }
      if ( previous )   {
        previous->do_intersection(work->iov);
      }
      work->condition->iov  = work->iov;
      work->condition->hash = dep->target.hash;
      work->condition->setFlag(Condition::DERIVED);
      work->state = CREATED;
      ++num_callback;
    }
    else   {
      printout(ERROR,"DependencyHandler",
	       "+++ Callback handler returned invalid condition.  Key:%s %c%s%c",
	       work->context.dependency->target.toString().c_str(),
#if defined(DD4HEP_CONDITIONS_DEBUG)
	       '[',work->context.dependency->detector.path().c_str(),']'
#else
	       ' ',"",' '
#endif
	       );
      throw std::runtime_error("Invalid derived condition callback");
    }
    return;
  }
  catch(const std::exception& e)   {
    printout(ERROR,"DependencyHandler",
             "+++ Exception while creating dependent Condition %s:",
             dependency_name(dep).c_str());
    printout(ERROR,"DependencyHandler","\t\t%s", e.what());
  }
  catch(...)   {
    printout(ERROR,"DependencyHandler",
             "+++ UNKNOWN exception while creating dependent Condition %s.",
             dependency_name(dep).c_str());
  }
  m_pool.print("*");
  except("DependencyHandler",
         "++ Exception while creating dependent Condition %s.",
         dependency_name(dep).c_str());
}
