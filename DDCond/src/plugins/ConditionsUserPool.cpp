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
#ifndef DDCOND_CONDITIONSMAPPEDUSERPOOL_H
#define DDCOND_CONDITIONSMAPPEDUSERPOOL_H

// Framework include files
#include <DDCond/ConditionsPool.h>
#include <DD4hep/ConditionsMap.h>

// C/C++ include files
#include <map>
#include <unordered_map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

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
      Condition::Object* i_findCondition(Condition::key_type key)  const;

      /// Internal insertion helper
      bool i_insert(Condition::Object* o);

    public:
      /// Default constructor
      ConditionsMappedUserPool(ConditionsManager mgr, ConditionsIOVPool* pool);
      /// Default destructor
      virtual ~ConditionsMappedUserPool();
      /// Print pool content
      virtual void print(const std::string& opt)   const  override;
      /// Total entry count
      virtual size_t size()  const  override;
      /// Full cleanup of all managed conditions.
      virtual void clear()  override;
      /// Check a condition for existence
      virtual bool exists(Condition::key_type hash)  const  override;
      /// Check a condition for existence
      virtual bool exists(const ConditionKey& key)  const  override;
      /// Check if a condition exists in the pool and return it to the caller
      virtual Condition get(Condition::key_type hash)  const  override;
      /// Check if a condition exists in the pool and return it to the caller     
      virtual Condition get(const ConditionKey& key)  const  override;

      /// Remove condition by key from pool.
      virtual bool remove(Condition::key_type hash_key)  override;
      /// Remove condition by key from pool.
      virtual bool remove(const ConditionKey& key)  override;
      /// Do single insertion of condition including registration to the manager
      virtual bool registerOne(const IOV& iov, Condition cond)  override;
      /// Do block insertions of conditions with identical IOV including registration to the manager
      virtual size_t registerMany(const IOV& iov, const std::vector<Condition>& values) override;
      /// Register a new condition to this pool
      virtual bool insert(Condition cond)  override;

      /// ConditionsMap overload: Add a condition directly to the slice
      virtual bool insert(DetElement detector, Condition::itemkey_type key, Condition condition)  override;
      /// ConditionsMap overload: Access a condition
      virtual Condition get(DetElement detector, Condition::itemkey_type key)  const  override;
      /// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
      virtual std::vector<Condition> get(DetElement detector,
                                         Condition::itemkey_type lower,
                                         Condition::itemkey_type upper)  const override;
      /// Access all conditions within the key range of a detector element
      virtual std::vector<Condition> get(Condition::key_type lower,
                                         Condition::key_type upper)  const  override;

      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(const Condition::Processor& processor) const  override;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(Condition::key_type lower,
                        Condition::key_type upper,
                        const Condition::Processor& processor) const  override;
      /// ConditionsMap overload: Interface to scan data content of the conditions mapping
      virtual void scan(DetElement detector,
                        Condition::itemkey_type lower,
                        Condition::itemkey_type upper,
                        const Condition::Processor& processor) const  override;

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual ConditionsManager::Result prepare(const IOV&                  required,
                                                ConditionsSlice&            slice,
                                                ConditionUpdateUserContext* user_param)  override;

      /// Evaluate and register all derived conditions from the dependency list
      virtual size_t compute(const Dependencies&         dependencies,
                             ConditionUpdateUserContext* user_param,
                             bool                        force)  override;

      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual ConditionsManager::Result load   (const IOV&              required,
                                                ConditionsSlice&        slice,
                                                ConditionUpdateUserContext* user_param)  override;
      /// Prepare user pool for usage (load, fill etc.) according to required IOV
      virtual ConditionsManager::Result compute(const IOV&                  required,
                                                ConditionsSlice&            slice,
                                                ConditionUpdateUserContext* user_param)  override;
    };

  }    /* End namespace cond               */
}      /* End namespace dd4hep                   */
#endif /* DDCOND_CONDITIONSMAPPEDUSERPOOL_H      */

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
//#include <DDCond/ConditionsMappedPool.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Factories.h>
#include <DD4hep/InstanceCount.h>

#include <DDCond/ConditionsIOVPool.h>
#include <DDCond/ConditionsSelectors.h>
#include <DDCond/ConditionsDataLoader.h>
#include <DDCond/ConditionsManagerObject.h>
#include <DDCond/ConditionsDependencyHandler.h>

// C/C++ include files
#include <mutex>

using namespace dd4hep::cond;

namespace {

  class SimplePrint : public dd4hep::Condition::Processor {
    /// Conditions callback for object processing
    virtual int process(dd4hep::Condition)  const override    { return 1; }
    /// Conditions callback for object processing
    virtual int operator()(dd4hep::Condition)  const override { return 1; }
    /// Conditions callback for object processing in maps
    virtual int operator()(const std::pair<dd4hep::Condition::key_type,dd4hep::Condition>& i)  const override  {
      dd4hep::Condition c = i.second;
      dd4hep::printout(dd4hep::INFO,"UserPool","++ %16llX/%16llX Val:%s %s",i.first, c->hash, c->value.c_str(), c.str().c_str());
      return 1;
    }
  };
  template <typename T> struct MapSelector : public dd4hep::ConditionsSelect {
    T& m;
    MapSelector(T& o) : m(o) {}
    bool operator()(dd4hep::Condition::Object* o)  const
    { return m.emplace(o->hash,o).second;    }
  };
  template <typename T> MapSelector<T> mapSelector(T& container)
  {  return MapSelector<T>(container);       }

  template <typename T> struct Inserter {
    T& m;
    dd4hep::IOV* iov;
    Inserter(T& o, dd4hep::IOV* i=0) : m(o), iov(i) {}
    void operator()(const dd4hep::Condition& c)  {
      dd4hep::Condition::Object* o = c.ptr();
      m.emplace(o->hash,o);
      if ( iov ) iov->iov_intersection(o->iov->key());
    }
    void operator()(const std::pair<dd4hep::Condition::key_type,dd4hep::Condition>& e) { (*this)(e.second);  }
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

template<typename MAPPING> inline dd4hep::Condition::Object* 
ConditionsMappedUserPool<MAPPING>::i_findCondition(Condition::key_type key)  const {
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
  int ret = m_conditions.emplace(o->hash,o).second;
  if ( flags&PRINT_INSERT )  {
    printout(INFO,"UserPool","++ %s condition [%016llX]"
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
             ": %s [%s].", ret ? "Successfully inserted" : "FAILED to insert",
             o->hash, o->GetName(), o->GetTitle());
#else
    , ret ? "Successfully inserted" : "FAILED to insert", o->hash);
#endif
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
void ConditionsMappedUserPool<MAPPING>::print(const std::string& opt)   const  {
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
bool ConditionsMappedUserPool<MAPPING>::exists(Condition::key_type hash)  const   {
  return i_findCondition(hash) != 0;
}

/// Check a condition for existence
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::exists(const ConditionKey& key)  const   {
  return i_findCondition(key.hash) != 0;
}

/// Check if a condition exists in the pool and return it to the caller
template<typename MAPPING>
dd4hep::Condition ConditionsMappedUserPool<MAPPING>::get(Condition::key_type hash)  const   {
  return i_findCondition(hash);
}

/// Check if a condition exists in the pool and return it to the caller     
template<typename MAPPING>
dd4hep::Condition ConditionsMappedUserPool<MAPPING>::get(const ConditionKey& key)  const   {
  return i_findCondition(key.hash);
}

/// Do block insertions of conditions with identical IOV including registration to the manager
template<typename MAPPING> bool
ConditionsMappedUserPool<MAPPING>::registerOne(const IOV& iov,
                                               Condition cond)   {
  if ( iov.iovType )   {
    ConditionsPool* pool = m_manager.registerIOV(*iov.iovType,iov.keyData);
    if ( pool )   {
      return m_manager.registerUnlocked(*pool, cond);
    }
    except("UserPool","++ Failed to register IOV: %s",iov.str().c_str());
  }
  except("UserPool","++ Cannot register conditions with invalid IOV.");
  return 0;
}

/// Do block insertions of conditions with identical IOV including registration to the manager
template<typename MAPPING> std::size_t
ConditionsMappedUserPool<MAPPING>::registerMany(const IOV& iov,
                                                const std::vector<Condition>& conds)   {
  if ( iov.iovType )   {
    ConditionsPool* pool = m_manager.registerIOV(*iov.iovType,iov.keyData);
    if ( pool )   {
      std::size_t result = m_manager.blockRegister(*pool, conds);
      if ( result == conds.size() )   {
        for(auto c : conds) i_insert(c.ptr());
        return result;
      }
      except("UserPool","++ Conditions registration was incomplete: "
             "registerd only  %ld out of %ld conditions.",
             result, conds.size());
    }
    except("UserPool","++ Failed to register IOV: %s",iov.str().c_str());
  }
  except("UserPool","++ Cannot register conditions with invalid IOV.");
  return 0;
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
  cond->hash = ConditionKey::KeyMaker(detector.key(),item_key).hash;
  return insert(cond);
}

/// ConditionsMap overload: Access a condition
template<typename MAPPING>
dd4hep::Condition ConditionsMappedUserPool<MAPPING>::get(DetElement detector, unsigned int item_key)  const  {
  return get(ConditionKey::KeyMaker(detector.key(), item_key).hash);
}
  
/// No ConditionsMap overload: Access all conditions within a key range in the interval [lower,upper]
template<typename MAPPING> std::vector<dd4hep::Condition>
ConditionsMappedUserPool<MAPPING>::get(DetElement detector,
                                       Condition::itemkey_type lower,
                                       Condition::itemkey_type upper)  const {
  Condition::detkey_type det_key = detector.key();
  return this->get(ConditionKey::KeyMaker(det_key,lower).hash,
                   ConditionKey::KeyMaker(det_key,upper).hash);
}

/// Specialization for std::map: Access all conditions within a given key range
template<typename MAPPING> std::vector<dd4hep::Condition>
ConditionsMappedUserPool<MAPPING>::get(Condition::key_type lower, Condition::key_type upper)   const  {
  std::vector<Condition> result;
  if ( !m_conditions.empty() )   {
    typename MAPPING::const_iterator first = m_conditions.lower_bound(lower);
    for(; first != m_conditions.end(); ++first )  {
      if ( (*first).first > upper ) break;
      result.emplace_back((*first).second);
    }
  }
  return result;
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(const Condition::Processor& processor) const  {
  for( const auto& i : m_conditions )
    processor(i.second);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(DetElement detector,
                                             Condition::itemkey_type lower,
                                             Condition::itemkey_type upper,
                                             const Condition::Processor& processor) const
{
  Condition::detkey_type det_key = detector.key();
  scan(ConditionKey::KeyMaker(det_key,lower).hash,
       ConditionKey::KeyMaker(det_key,upper).hash,
       processor);
}

/// ConditionsMap overload: Interface to scan data content of the conditions mapping
template<typename MAPPING>
void ConditionsMappedUserPool<MAPPING>::scan(Condition::key_type lower,
                                             Condition::key_type upper,
                                             const Condition::Processor& processor) const
{
  typename MAPPING::const_iterator first = m_conditions.lower_bound(lower);
  for(; first != m_conditions.end() && (*first).first <= upper; ++first )
    processor((*first).second);
}

/// Remove condition by key from pool.
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::remove(const ConditionKey& key)   {
  return remove(key.hash);
}

/// Remove condition by key from pool.
template<typename MAPPING>
bool ConditionsMappedUserPool<MAPPING>::remove(Condition::key_type hash_key)    {
  typename MAPPING::iterator i = m_conditions.find(hash_key);
  if ( i != m_conditions.end() ) {
    m_conditions.erase(i);
    return true;
  }
  return false;
}

/// Evaluate and register all derived conditions from the dependency list
template<typename MAPPING>
std::size_t ConditionsMappedUserPool<MAPPING>::compute(const Dependencies& deps,
                                                       ConditionUpdateUserContext* user_param,
                                                       bool force)
{
  if ( !deps.empty() )  {
    Dependencies missing;
    // Loop over the dependencies and check if they have to be upgraded
    for ( const auto& i : deps )  {
      typename MAPPING::iterator j = m_conditions.find(i.first);
      if ( j != m_conditions.end() )  {
        if ( !force )  {
          Condition::Object* c = (*j).second;
          // Remeber: key ist first, test is second!
          if ( !IOV::key_is_contained(m_iov.keyData,c->iov->keyData) )  {
            /// This condition is no longer valid. remove it!
            /// It will be added again by the handler.
            m_conditions.erase(j);
            missing.emplace(i);
          }
          continue;
        }
        else  {
          m_conditions.erase(j);
        }
      }
      missing.emplace(i);
    }
    if ( !missing.empty() )  {
      ConditionsManagerObject* mgr(m_manager.access());
      ConditionsDependencyHandler handler(mgr, *this, missing, user_param);
      /// 1rst pass: Compute/create the missing condiions
      handler.compute();
      /// 2nd pass:  Resolve missing dependencies
      handler.resolve();
      return handler.num_callback;
    }
  }
  return 0;
}

namespace {
  struct COMP {
    typedef std::pair<dd4hep::Condition::key_type,const ConditionDependency*>             Dep;
    typedef std::pair<const dd4hep::Condition::key_type,dd4hep::detail::ConditionObject*> Cond;
    typedef std::pair<const dd4hep::Condition::key_type,ConditionsLoadInfo* >             Info;
    typedef std::pair<const dd4hep::Condition::key_type,dd4hep::Condition>                Cond2;
    
    bool operator()(const Dep& a,const Cond& b) const   { return a.first < b.first; }
    bool operator()(const Cond& a,const Dep& b) const   { return a.first < b.first; }

    bool operator()(const Info& a,const Cond& b) const  { return a.first < b.first; }
    bool operator()(const Cond& a,const Info& b) const  { return a.first < b.first; }

    bool operator()(const Info& a,const Cond2& b) const { return a.first < b.first; }
    bool operator()(const Cond2& a,const Info& b) const { return a.first < b.first; }
  };
}

template<typename MAPPING> ConditionsManager::Result
ConditionsMappedUserPool<MAPPING>::prepare(const IOV&                  required, 
                                           ConditionsSlice&            slice,
                                           ConditionUpdateUserContext* user_param)
{
  typedef std::vector<std::pair<Condition::key_type,ConditionDependency*> > CalcMissing;
  typedef std::vector<std::pair<Condition::key_type,ConditionsLoadInfo*> >  CondMissing;
  const auto& slice_cond = slice.content->conditions();
  const auto& slice_calc = slice.content->derived();
  auto&  slice_miss_cond = slice.missingConditions();
  auto&  slice_miss_calc = slice.missingDerivations();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output_miss  = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  ConditionsManager::Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static std::mutex lock;
  std::lock_guard<std::mutex> guard(lock);

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
  long num_cond_miss = last_cond-begin(cond_missing);
  cond_missing.resize(num_cond_miss);
  printout((flags&PRINT_LOAD) ? INFO : DEBUG,"UserPool",
           "%ld conditions out of %ld conditions are MISSING.",
           num_cond_miss, slice_cond.size());
  CalcMissing::iterator last_calc = set_difference(begin(slice_calc),   end(slice_calc),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(calc_missing), COMP());
  long num_calc_miss = last_calc-begin(calc_missing);
  calc_missing.resize(num_calc_miss);
  printout((flags&PRINT_COMPUTE) ? INFO : DEBUG,"UserPool",
           "%ld derived conditions out of %ld conditions are MISSING.",
           num_calc_miss, slice_calc.size());

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
        long num_load_miss = load_last-begin(load_missing);
        printout(num_load_miss==0 ? DEBUG : ERROR,"UserPool",
                 "%ld out of %ld conditions CANNOT be loaded...",
                 num_load_miss, loaded.size());
        if ( do_output_miss )  {
          copy(begin(load_missing), load_last, inserter(slice_miss_cond, slice_miss_cond.begin()));
          for ( const auto& missing : slice_miss_cond )   {
            printout (ERROR, "TEST", "Unloaded: %s",missing.second->toString().c_str());
          }
        }
        for_each(loaded.begin(),loaded.end(),Inserter<MAPPING>(m_conditions,&m_iov));
        result.loaded  = slice_cond.size()-num_load_miss;
        result.missing = num_load_miss+num_calc_miss;
        if ( cond_missing.size() != loaded.size() )  {
          // ERROR!
          printout(ERROR,"UserPool","Could not load %ld out of %ld conditions. "
                   "[Unhandled error]",cond_missing.size(), slice_cond.size());
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(cond_missing), last_cond, inserter(slice_miss_cond, slice_miss_cond.begin()));
      for ( const auto& missing : slice_miss_cond )   {
        printout (ERROR, "TEST", "Unloaded: %s",missing.second->toString().c_str());
      }
    }
  }
  //
  // Now we update the already existing dependencies, which have expired
  //
  if ( num_calc_miss > 0 )  {
    if ( do_load )  {
      std::map<Condition::key_type,const ConditionDependency*> deps(calc_missing.begin(),last_calc);
      ConditionsDependencyHandler handler(m_manager, *this, deps, user_param);
      /// 1rst pass: Compute/create the missing condiions
      handler.compute();
      /// 2nd pass:  Resolve missing dependencies
      handler.resolve();
      
      result.computed = handler.num_callback;
      result.missing -= handler.num_callback;
      if ( do_output_miss && result.computed < deps.size() )  {
        // Is this cheaper than an intersection ?
        for( auto i = calc_missing.begin(); i != last_calc; ++i )   {
          typename MAPPING::iterator j = m_conditions.find((*i).first);
          if ( j == m_conditions.end() )
            slice_miss_calc.emplace(*i);
        }
      }
    }
    else if ( do_output_miss )  {
      copy(begin(calc_missing), last_calc, inserter(slice_miss_calc, slice_miss_calc.begin()));
    }
  }
  slice.status = result;
  slice.used_pools.clear();
  if ( slice.flags&ConditionsSlice::REF_POOLS )   {
    m_iovPool->select(required, slice.used_pools);
  }
  return result;
}

template<typename MAPPING> ConditionsManager::Result
ConditionsMappedUserPool<MAPPING>::load(const IOV&                  required, 
                                        ConditionsSlice&            slice,
                                        ConditionUpdateUserContext* /* user_param */)
{
  typedef std::vector<std::pair<Condition::key_type,ConditionsLoadInfo*> >  CondMissing;
  const auto& slice_cond = slice.content->conditions();
  auto&  slice_miss_cond = slice.missingConditions();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output_miss  = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  ConditionsManager::Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static std::mutex lock;
  std::lock_guard<std::mutex> guard(lock);

  m_conditions.clear();
  slice_miss_cond.clear();
  pool_iov.reset().invert();
  m_iovPool->select(required, Operators::mapConditionsSelect(m_conditions), pool_iov);
  m_iov = pool_iov;
  CondMissing cond_missing(slice_cond.size()+m_conditions.size());
  CondMissing::iterator last_cond = set_difference(begin(slice_cond),   end(slice_cond),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(cond_missing), COMP());
  long num_cond_miss = last_cond-begin(cond_missing);
  cond_missing.resize(num_cond_miss);
  printout((flags&PRINT_LOAD) ? INFO : DEBUG,"UserPool",
           "Found %ld missing conditions out of %ld conditions.",
           num_cond_miss, slice_cond.size());
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
        long num_load_miss = load_last-begin(load_missing);
        printout(num_load_miss==0 ? DEBUG : ERROR,"UserPool",
                 "+++ %ld out of %ld conditions CANNOT be loaded... [Not found by loader]",
                 num_load_miss, loaded.size());
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
  slice.status = result;
  return result;
}

template<typename MAPPING> ConditionsManager::Result
ConditionsMappedUserPool<MAPPING>::compute(const IOV&                  required, 
                                           ConditionsSlice&            slice,
                                           ConditionUpdateUserContext* user_param)
{
  typedef std::vector<std::pair<Condition::key_type,ConditionDependency*> > CalcMissing;
  const auto& slice_calc = slice.content->derived();
  auto&  slice_miss_calc = slice.missingDerivations();
  bool   do_load         = m_manager->doLoadConditions();
  bool   do_output       = m_manager->doOutputUnloaded();
  IOV    pool_iov(required.iovType);
  ConditionsManager::Result result;

  // This is a critical operation, because we have to ensure the
  // IOV pools are ONLY manipulated by the current thread.
  // Otherwise the selection and the population are unsafe!
  static std::mutex lock;
  std::lock_guard<std::mutex> guard(lock);

  slice_miss_calc.clear();
  CalcMissing calc_missing(slice_calc.size()+m_conditions.size());
  CalcMissing::iterator last_calc = set_difference(begin(slice_calc),   end(slice_calc),
                                                   begin(m_conditions), end(m_conditions),
                                                   begin(calc_missing), COMP());
  long num_calc_miss = last_calc-begin(calc_missing);
  calc_missing.resize(num_calc_miss);
  printout((flags&PRINT_COMPUTE) ? INFO : DEBUG,"UserPool",
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
      std::map<Condition::key_type,const ConditionDependency*> deps(calc_missing.begin(),last_calc);
      ConditionsDependencyHandler handler(m_manager, *this, deps, user_param);

      /// 1rst pass: Compute/create the missing condiions
      handler.compute();
      /// 2nd pass:  Resolve missing dependencies
      handler.resolve();

      result.computed = handler.num_callback;
      result.missing -= handler.num_callback;
      if ( do_output && result.computed < deps.size() )  {
        for(auto i=calc_missing.begin(); i != last_calc; ++i)   {
          typename MAPPING::iterator j = m_conditions.find((*i).first);
          if ( j == m_conditions.end() )
            slice_miss_calc.emplace(*i);
        }
      }
    }
    else if ( do_output )  {
      copy(begin(calc_missing), last_calc, inserter(slice_miss_calc, slice_miss_calc.begin()));
    }
  }
  slice.status += result;
  slice.used_pools.clear();
  if ( slice.flags&ConditionsSlice::REF_POOLS )   {
    m_iovPool->select(required, slice.used_pools);
  }
  return result;
}


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    typedef std::unordered_map<Condition::key_type,Condition::Object*> umap_t;

    /// Access all conditions within a given key range
    /** Specialization necessary, since unordered maps have no lower bound.
     */
    template<> void
    ConditionsMappedUserPool<umap_t>::scan(Condition::key_type lower,
                                           Condition::key_type upper,
                                           const Condition::Processor& processor)   const  {
      for( const auto& e : m_conditions )
        if ( e.second->hash >= lower && e.second->hash <= upper )
          processor(e.second);
    }
    /// Access all conditions within a given key range
    /** Specialization necessary, since unordered maps have no lower bound.
     */
    template<> std::vector<Condition>
    ConditionsMappedUserPool<umap_t>::get(Condition::key_type lower, Condition::key_type upper)   const  {
      std::vector<Condition> result;
      for( const auto& e : m_conditions )  {
        if ( e.second->hash >= lower && e.second->hash <= upper )
          result.emplace_back(e.second);
      }
      return result;
    }
  }    /* End namespace cond               */
}      /* End namespace dd4hep                   */

namespace {
  template <typename MAPPING>
  void* create_pool(dd4hep::Detector&, int argc, char** argv)  {
    if ( argc > 1 )  {
      ConditionsManagerObject* m = (ConditionsManagerObject*)argv[0];
      ConditionsIOVPool* p = (ConditionsIOVPool*)argv[1];
      UserPool* pool = new ConditionsMappedUserPool<MAPPING>(m, p);
      return pool;
    }
    dd4hep::except("ConditionsMappedUserPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return 0;
  }
}

// Factory for the user pool using a binary tree map
void* create_map_user_pool(dd4hep::Detector& description, int argc, char** argv)
{  return create_pool<std::map<dd4hep::Condition::key_type,dd4hep::Condition::Object*> >(description, argc, argv);  }
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsMapUserPool, create_map_user_pool)

// Factory for the user pool using a binary tree map
void* create_unordered_map_user_pool(dd4hep::Detector& description, int argc, char** argv)
{  return create_pool<std::unordered_map<dd4hep::Condition::key_type,dd4hep::Condition::Object*> >(description, argc, argv);  }
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsUnorderedMapUserPool, create_map_user_pool)
