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
#ifndef DDCOND_CONDITIONSMAPPEDUSERPOOL_H
#define DDCOND_CONDITIONSMAPPEDUSERPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"

// C/C++ include files
#include <map>
#include <unordered_map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Forward declarations
    class ConditionsDataLoader;
    
    /// Class implementing the conditions user pool for a given IOV type
    /**
     *
     *  Please note:
     *  Users should not directly interact with object instances of this type.
     *  Data are not thread protected and interaction may cause serious harm.
     *  Only the ConditionsManager implementation should interact with
     *  this class or any subclass to ensure data integrity.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING> 
    class ConditionsMappedUserPool : public UserPool    {
      typedef MAPPING Mapping;
      Mapping               m_conditions;
      /// IOV Pool as data source
      ConditionsIOVPool*    m_iovPool = 0;
      /// The loader to access non-existing conditions
      ConditionsDataLoader* m_loader = 0;

      /// Internal helper to find conditions
      Condition::Object* i_findCondition(key_type key)  const;

      /// Internal insertion helper
      bool i_insert(Condition::Object* o);

    public:
      /// Default constructor
      ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool);
      /// Default destructor
      virtual ~ConditionsMappedUserPool();
      /// Print pool content
      virtual void print(const std::string& opt)   const;
      /// Total entry count
      virtual size_t size()  const;
      /// Full cleanup of all managed conditions.
      virtual void clear();
      /// Check a condition for existence
      virtual bool exists(key_type hash)  const;
      /// Check a condition for existence
      virtual bool exists(const ConditionKey& key)  const;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(key_type hash)  const;
      /// Check if a condition exists in the pool and return it to the caller     
      virtual Condition get(const ConditionKey& key)  const;
      /// Remove condition by key from pool.
      virtual bool remove(key_type hash_key);
      /// Remove condition by key from pool.
      virtual bool remove(const ConditionKey& key);
      /// Register a new condition to this pool
      virtual bool insert(Condition cond);
      /// ConditionsMap overload: Add a condition directly to the slice
      virtual bool insert(DetElement detector, unsigned int key, Condition condition);
      /// ConditionsMap overload: Access a condition
      virtual Condition get(DetElement detector, unsigned int key)  const;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(Condition::Processor& processor) const;

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual Result prepare(const IOV&              required,
                             ConditionsSlice&        slice,
                             void*                   user_param);

      /// Evaluate and register all derived conditions from the dependency list
      virtual size_t compute(const Dependencies&     dependencies,
                             void*                   user_param,
                             bool                    force);

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual Result load   (const IOV&              required,
                             ConditionsSlice&        slice);
      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual Result compute(const IOV&              required,
                             ConditionsSlice&        slice,
                             void*                   user_param);
    };

  }    /* End namespace Conditions               */
}      /* End namespace DD4hep                   */
#endif /* DDCOND_CONDITIONSMAPPEDUSERPOOL_H      */

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
//#include "DDCond/ConditionsMappedPool.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/InstanceCount.h"

#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsSelectors.h"
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DDCond/ConditionsDependencyHandler.h"

#include <mutex>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;

namespace {

  class SimplePrint : public Condition::Processor {
    /// Conditions callback for object processing
    virtual int process(Condition)  override    { return 1; }
    /// Conditions callback for object processing
    virtual int operator()(Condition)  override { return 1; }
    /// Conditions callback for object processing in maps
    virtual int operator()(const pair<Condition::key_type,Condition>& i)  override  {
      Condition c = i.second;
      printout(INFO,"UserPool","++ %16llX/%16llX Val:%s %s",i.first, c->hash, c->value.c_str(), c.str().c_str());
      return 1;
    }
  };
  template <typename T> struct MapSelector : public ConditionsSelect {
    T& m;
    MapSelector(T& o) : m(o) {}
    bool operator()(Condition::Object* o)  const
    { return m.insert(make_pair(o->hash,o)).second;    }
  };
  template <typename T> MapSelector<T> mapSelector(T& container)
  {  return MapSelector<T>(container);   }

  template <typename T> struct Inserter {
    T& m;
    IOV* iov;
    Inserter(T& o, IOV* i=0) : m(o), iov(i) {}
    void operator()(const Condition& c)  {
      Condition::Object* o = c.ptr();
      m.insert(make_pair(o->hash,o));
      if ( iov ) iov->iov_intersection(o->iov->key());
    }
    void operator()(const pair<Condition::key_type,Condition>& e) { (*this)(e.second);  }
  };
}

/// Default constructor
template<typename MAPPING>
ConditionsMappedUserPool<MAPPING>::ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool) 
  : UserPool(mgr), m_iovPool(pool)
{
  InstanceCount::increment(this);
  if ( mgr.isValid() )  {
    if ( m_iovPool )  {
      m_iov.iovType = m_iovPool->type;
      m_loader = mgr->loader();
      if ( m_loader ) return;
      except("UserPool","The conditions manager is not properly setup. No conditions loader present.");
    }
    except("UserPool","FAILED to create mapped user pool. [Invalid IOV pool]");
  }
  except("UserPool","FAILED to create mapped user pool. [Invalid conditions manager]");
}

/// Default destructor
template<typename MAPPING>
ConditionsMappedUserPool<MAPPING>::~ConditionsMappedUserPool()  {
  clear();
  InstanceCount::decrement(this);
}

template<typename MAPPING> inline Condition::Object* 
ConditionsMappedUserPool<MAPPING>::i_findCondition(key_type key)  const {
  typename MAPPING::const_iterator i=m_conditions.find(key);
#if 0
  if ( i == m_conditions.end() )  {
    print("*"); // This causes CTEST to bail out, due too much output!
  }
#endif
  return i != m_conditions.end() ? (*i).second : 0;
}

template<typename MAPPING> inline bool
ConditionsMappedUserPool<MAPPING>::i_insert(Condition::Object* o)   {
  int ret = m_conditions.insert(make_pair(o->hash,o)).second;
  if ( flags&PRINT_INSERT )  {
    printout(INFO,"UserPool","++ %s condition [%016llX]: %s.",
             ret ? "Successfully inserted" : "FAILED to insert",
             o->hash, o->name.c_str());
  }
  return ret;
}
  
/// Total entry count
template<typename MAPPING>
size_t ConditionsMappedUserPool<MAPPING>::size()  const  {
  return  m_conditions.size();
}

/// Print pool content
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::print(const string& opt)   const  {
  const IOV* iov = &m_iov;
  printout(INFO,"UserPool","+++ %s Conditions for USER pool with IOV: %-32s [%4d entries]",
           opt.c_str(), iov->str().c_str(), size());
  if ( opt == "*" )  {
    SimplePrint prt;
    scan(prt);
  }
}

/// Full cleanup of all managed conditions.
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::clear()   {
  if ( flags&PRINT_CLEAR )  {
    printout(INFO,"UserPool","++ Cleared %ld conditions from pool.",m_conditions.size());
  }
  m_iov = IOV(0);
  m_conditions.clear();
}

/// Check a condition for existence
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::exists(key_type hash)  const   {
  return i_findCondition(hash) != 0;
}

/// Check a condition for existence
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::exists(const ConditionKey& key)  const   {
  return i_findCondition(key.hash) != 0;
}

/// Check if a condition exists in the pool and return it to the caller
template<typename MAPPING>
Condition ConditionsMappedUserPool<MAPPING>::get(key_type hash)  const   {
  return i_findCondition(hash);
}

/// Check if a condition exists in the pool and return it to the caller     
template<typename MAPPING>
Condition ConditionsMappedUserPool<MAPPING>::get(const ConditionKey& key)  const   {
  return i_findCondition(key.hash);
}

/// Register a new condition to this pool
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::insert(Condition cond)   {
  bool result = i_insert(cond.ptr());
  if ( result ) return true;
  except("UserPool","++ Attempt to double insert condition: %16llX Name:%s", cond->hash, cond.name());
  return false;
}

/// ConditionsMap overload: Add a condition directly to the slice
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::insert(DetElement detector,
                                               unsigned int item_key,
                                               Condition cond)   {
  ConditionKey::KeyMaker m(detector.key(),item_key);
  cond->hash = m.hash;
  return insert(cond);
}

/// ConditionsMap overload: Access a condition
template<typename MAPPING>
Condition ConditionsMappedUserPool<MAPPING>::get(DetElement detector, unsigned int item_key)  const  {
  ConditionKey::KeyMaker m(detector.key(),item_key);
  return get(m.hash);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(Condition::Processor& processor) const  {
  for( const auto& i : m_conditions )
    processor(i.second);
}

/// Remove condition by key from pool.
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::remove(const ConditionKey& key)   {
  return remove(key.hash);
}

/// Remove condition by key from pool.
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::remove(key_type hash_key)    {
  typename MAPPING::iterator i = m_conditions.find(hash_key);
  if ( i != m_conditions.end() ) {
    m_conditions.erase(i);
    return true;
  }
  return false;
}

/// Evaluate and register all derived conditions from the dependency list
template<typename MAPPING>
size_t ConditionsMappedUserPool<MAPPING>::compute(const Dependencies& deps,
                                                  void* user_param,
                                                  bool force)
{
  size_t num_updates = 0;
  if ( !deps.empty() )  {
    vector<const ConditionDependency*> missing;
    // Loop over the dependencies and check if they have to be upgraded
    missing.reserve(deps.size());
    for ( const auto& i : deps )  {
      typename MAPPING::iterator j = m_conditions.find(i.first);
      if ( j != m_conditions.end() )  {
        if ( !force )  {
          Condition::Object* c = (*j).second;
          // Remeber: key ist first, test is second!
          if ( IOV::key_is_contained(m_iov.keyData,c->iov->keyData) )  {
            /// This condition is no longer valid. remove it!
            /// This condition will be added again by the handler.
            m_conditions.erase(j);
            missing.push_back(i.second);
          }
          continue;
        }
        else  {
          m_conditions.erase(j);
        }
      }
      missing.push_back(i.second);      
    }
    if ( !missing.empty() )  {
      ConditionsManagerObject*    m(m_manager.access());
      ConditionsDependencyHandler h(m, *this, deps, user_param);
      for ( const ConditionDependency* d : missing )  {
        Condition::Object* c = h(d);
        if ( c ) ++num_updates;
      }
    }
  }
  return num_updates;
}

namespace {
  struct COMP {
    typedef pair<Condition::key_type,const ConditionDependency*> Dep;
    typedef pair<const Condition::key_type,Interna::ConditionObject*>   Cond;
    typedef pair<const Condition::key_type,ConditionsLoadInfo* >   Info;
    typedef pair<const Condition::key_type,Condition>   Cond2;
    
    bool operator()(const Dep& a,const Cond& b) const { return a.first < b.first; }
    bool operator()(const Cond& a,const Dep& b) const { return a.first < b.first; }

    bool operator()(const Info& a,const Cond& b) const { return a.first < b.first; }
    bool operator()(const Cond& a,const Info& b) const { return a.first < b.first; }

    bool operator()(const Info& a,const Cond2& b) const { return a.first < b.first; }
    bool operator()(const Cond2& a,const Info& b) const { return a.first < b.first; }
  };
}

template<typename MAPPING> UserPool::Result
ConditionsMappedUserPool<MAPPING>::prepare(const IOV&              required, 
                                           ConditionsSlice&        slice,
                                           void*                   user_param)
{
  typedef vector<pair<Condition::key_type,ConditionDependency*> > CalcMissing;
  typedef vector<pair<Condition::key_type,ConditionsLoadInfo*> >  CondMissing;
  const auto& slice_cond = slice.content->conditions();
  const auto& slice_calc = slice.content->derived();
  auto&  slice_miss_cond = slice.missingConditions();
  auto&  slice_miss_calc = slice.missingDerivations();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output_miss  = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static mutex lock;
  lock_guard<mutex> guard(lock);

  m_conditions.clear();
  slice_miss_cond.clear();
  slice_miss_calc.clear();
  pool_iov.reset().invert();
  m_iovPool->select(required, Operators::mapConditionsSelect(m_conditions), pool_iov);
  m_iov = pool_iov;
  CondMissing cond_missing(slice_cond.size()+m_conditions.size());
  CalcMissing calc_missing(slice_calc.size()+m_conditions.size());

  CondMissing::iterator last_cond = set_difference(begin(slice_cond),   end(slice_cond),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(cond_missing), COMP());
  int num_cond_miss = int(last_cond-begin(cond_missing));
  printout(num_cond_miss==0 ? DEBUG : INFO,"UserPool",
           "Found %ld missing conditions out of %ld conditions.",
           num_cond_miss, m_conditions.size());
  CalcMissing::iterator last_calc = set_difference(begin(slice_calc),   end(slice_calc),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(calc_missing), COMP());
  int num_calc_miss = int(last_calc-begin(calc_missing));
  printout(num_calc_miss==0 ? DEBUG : INFO,"UserPool",
           "Found %ld missing derived conditions out of %ld conditions.",
           num_calc_miss, m_conditions.size());

  result.loaded   = 0;
  result.computed = 0;
  result.selected = m_conditions.size();
  result.missing  = num_cond_miss+num_calc_miss;
  //
  // Now we load the missing conditions from the conditions loader
  //
  if ( num_cond_miss > 0 )  {
    if ( do_load )  {
      ConditionsDataLoader::LoadedItems loaded;
      size_t updates = m_loader->load_many(required, cond_missing, loaded, pool_iov);
      if ( updates > 0 )  {
        // Need to compute the intersection: All missing entries are required....
        CondMissing load_missing(cond_missing.size()+loaded.size());
        // Note: cond_missing is already sorted (doc of 'set_difference'). No need to re-sort....
        CondMissing::iterator load_last = set_difference(begin(cond_missing), last_cond,
                                                         begin(loaded), end(loaded),
                                                         begin(load_missing), COMP());
        int num_load_miss = int(load_last-begin(load_missing));
        printout(num_load_miss==0 ? DEBUG : ERROR,"UserPool",
                 "Found %ld out of %d conditions, which CANNOT be loaded...",
                 num_load_miss, int(loaded.size()));
        if ( do_output_miss )  {
          copy(begin(load_missing), load_last, inserter(slice_miss_cond, slice_miss_cond.begin()));
        }
        for_each(loaded.begin(),loaded.end(),Inserter<MAPPING>(m_conditions,&m_iov));
        result.loaded  = slice_cond.size()-num_load_miss;
        result.missing = num_load_miss+num_calc_miss;
        if ( cond_missing.size() != loaded.size() )  {
          // ERROR!
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(cond_missing), last_cond, inserter(slice_miss_cond, slice_miss_cond.begin()));
    }
  }
  //
  // Now we update the already existing dependencies, which have expired
  //
  if ( num_calc_miss > 0 )  {
    if ( do_load )  {
      map<key_type,const ConditionDependency*> deps(calc_missing.begin(),last_calc);
      ConditionsDependencyHandler handler(m_manager, *this, deps, user_param);
      for( const auto& i : deps )   {
        const ConditionDependency* d = i.second;
        typename MAPPING::iterator j = m_conditions.find(d->key());
        // If we would know, that dependencies are only ONE level, we could skip this search....
        if ( j == m_conditions.end() )  {
          handler(d);
          continue;
        }
        // printout(INFO,"UserPool","Already calcluated: %s",d->name());
        continue;
      }
      result.computed = handler.num_callback;
      result.missing -= handler.num_callback;
      if ( do_output_miss && result.computed < deps.size() )  {
        // Is this cheaper than an intersection ?
        for(auto i=calc_missing.begin(); i != last_calc; ++i)   {
          typename MAPPING::iterator j = m_conditions.find((*i).first);
          if ( j == m_conditions.end() )
            slice_miss_calc.insert(*i);
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(calc_missing), last_calc, inserter(slice_miss_calc, slice_miss_calc.begin()));
    }
  }
  return result;
}

template<typename MAPPING> UserPool::Result
ConditionsMappedUserPool<MAPPING>::load(const IOV&              required, 
                                        ConditionsSlice&        slice)
{
  typedef vector<pair<Condition::key_type,ConditionsLoadInfo*> >  CondMissing;
  const auto& slice_cond = slice.content->conditions();
  auto&  slice_miss_cond = slice.missingConditions();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output_miss  = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static mutex lock;
  lock_guard<mutex> guard(lock);

  m_conditions.clear();
  slice_miss_cond.clear();
  pool_iov.reset().invert();
  m_iovPool->select(required, Operators::mapConditionsSelect(m_conditions), pool_iov);
  m_iov = pool_iov;
  CondMissing cond_missing(slice_cond.size()+m_conditions.size());
  CondMissing::iterator last_cond = set_difference(begin(slice_cond),   end(slice_cond),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(cond_missing), COMP());
  int num_cond_miss = int(last_cond-begin(cond_missing));
  printout(num_cond_miss==0 ? DEBUG : INFO,"UserPool",
           "Found %ld missing conditions out of %ld conditions.",
           num_cond_miss, m_conditions.size());
  result.loaded   = 0;
  result.computed = 0;
  result.missing  = num_cond_miss;
  result.selected = m_conditions.size();
  //
  // Now we load the missing conditions from the conditions loader
  //
  if ( num_cond_miss > 0 )  {
    if ( do_load )  {
      ConditionsDataLoader::LoadedItems loaded;
      size_t updates = m_loader->load_many(required, cond_missing, loaded, pool_iov);
      if ( updates > 0 )  {
        // Need to compute the intersection: All missing entries are required....
        CondMissing load_missing(cond_missing.size()+loaded.size());
        // Note: cond_missing is already sorted (doc of 'set_difference'). No need to re-sort....
        CondMissing::iterator load_last = set_difference(begin(cond_missing), last_cond,
                                                         begin(loaded), end(loaded),
                                                         begin(load_missing), COMP());
        int num_load_miss = int(load_last-begin(load_missing));
        printout(num_load_miss==0 ? DEBUG : ERROR,"UserPool",
                 "+++ %ld out of %d conditions CANNOT be loaded... [Not found by loader]",
                 num_load_miss, int(loaded.size()));
        if ( do_output_miss )  {
          copy(begin(load_missing), load_last, inserter(slice_miss_cond, slice_miss_cond.begin()));
        }
        for_each(loaded.begin(),loaded.end(),Inserter<MAPPING>(m_conditions,&m_iov));
        result.loaded  = slice_cond.size()-num_load_miss;
        result.missing = num_load_miss;
        if ( cond_missing.size() != loaded.size() )  {
          // ERROR!
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(cond_missing), last_cond, inserter(slice_miss_cond, slice_miss_cond.begin()));
    }
  }
  return result;
}

template<typename MAPPING> UserPool::Result
ConditionsMappedUserPool<MAPPING>::compute(const IOV&              required, 
                                           ConditionsSlice&        slice,
                                           void*                   user_param)
{
  typedef vector<pair<Condition::key_type,ConditionDependency*> > CalcMissing;
  const auto& slice_calc = slice.content->derived();
  auto&  slice_miss_calc = slice.missingDerivations();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output       = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static mutex lock;
  lock_guard<mutex> guard(lock);

  slice_miss_calc.clear();
  CalcMissing calc_missing(slice_calc.size()+m_conditions.size());
  CalcMissing::iterator last_calc = set_difference(begin(slice_calc),   end(slice_calc),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(calc_missing), COMP());
  int num_calc_miss = int(last_calc-begin(calc_missing));
  printout(num_calc_miss==0 ? DEBUG : INFO,"UserPool",
           "Found %ld missing derived conditions out of %ld conditions.",
           num_calc_miss, m_conditions.size());

  result.loaded   = 0;
  result.computed = 0;
  result.missing  = num_calc_miss;
  result.selected = m_conditions.size();
  //
  // Now we update the already existing dependencies, which have expired
  //
  if ( num_calc_miss > 0 )  {
    if ( do_load )  {
      map<key_type,const ConditionDependency*> deps(calc_missing.begin(),last_calc);
      ConditionsDependencyHandler handler(m_manager, *this, deps, user_param);
      for( const auto& i : deps )   {
        const ConditionDependency* d = i.second;
        typename MAPPING::iterator j = m_conditions.find(d->key());
        // If we would know, that dependencies are only ONE level, we could skip this search....
        if ( j == m_conditions.end() )  {
          handler(d);
          continue;
        }
        // printout(INFO,"UserPool","Already calcluated: %s",d->name());
        continue;
      }
      result.computed = handler.num_callback;
      result.missing -= handler.num_callback;
      if ( do_output && result.computed < deps.size() )  {
        for(auto i=calc_missing.begin(); i != last_calc; ++i)   {
          typename MAPPING::iterator j = m_conditions.find((*i).first);
          if ( j == m_conditions.end() )
            slice_miss_calc.insert(*i);
        }
      }
    }
    else if ( do_output )  {
      copy(begin(calc_missing), last_calc, inserter(slice_miss_calc, slice_miss_calc.begin()));
    }
  }
  return result;
}

namespace {
  template <typename MAPPING>
  void* create_pool(Geometry::LCDD&, int argc, char** argv)  {
    if ( argc > 1 )  {
      ConditionsManagerObject* m = (ConditionsManagerObject*)argv[0];
      ConditionsIOVPool* p = (ConditionsIOVPool*)argv[1];
      UserPool* pool = new ConditionsMappedUserPool<MAPPING>(m, p);
      return pool;
    }
    except("ConditionsMappedUserPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return 0;
  }
}

// Factory for the user pool using a binary tree map
void* create_map_user_pool(Geometry::LCDD& lcdd, int argc, char** argv)
{  return create_pool<map<Condition::key_type,Condition::Object*> >(lcdd, argc, argv);  }
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsMapUserPool, create_map_user_pool)

// Factory for the user pool using a unordered map (hash-map)
void* create_unordered_map_user_pool(Geometry::LCDD& lcdd, int argc, char** argv)
{  return create_pool<unordered_map<Condition::key_type,Condition::Object*> >(lcdd, argc, argv);  }
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsUnorderedMapUserPool, create_unordered_map_user_pool)

