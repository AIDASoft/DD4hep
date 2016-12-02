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
#ifndef DDCOND_CONDITIONSMAPPEDPOOL_H
#define DDCOND_CONDITIONSMAPPEDPOOL_H

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSelectors.h"

// C/C++ include files
#include <map>
#include <unordered_map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Class implementing the conditions collection for a given IOV type
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
    template<typename MAPPING, typename BASE> 
    class ConditionsMappedPool : public BASE   {
    protected:
      typedef BASE     Base;
      typedef MAPPING  Mapping;
      typedef ConditionsMappedPool<Mapping,Base> Self;
      typedef typename BASE::key_type key_type;
      Mapping          m_entries;
      
      /// Helper function to loop over the conditions container and apply a functor
      template <typename R,typename T> size_t loop(R& result, T functor) {
        size_t len = result.size();
        for_each(m_entries.begin(),m_entries.end(),functor);
        return result.size() - len;
      }
    public:
      /// Default constructor
      ConditionsMappedPool(ConditionsManager mgr);

      /// Default destructor
      virtual ~ConditionsMappedPool();

      /// Total entry count
      virtual size_t size()  const   {
        return m_entries.size();
      }

      /// Register a new condition to this pool
      virtual bool insert(Condition condition)    {
        Condition::Object* c = condition.access();
        bool result = m_entries.insert(std::make_pair(c->hash,c)).second;
        if ( result ) return true;
        auto i = m_entries.find(c->hash);
        Condition present = (*i).second;
          
        printout(ERROR,"MappedPool","ConditionsClash: %s %08llX <> %08llX %s",
                 present.name(), present.key(), condition.key(), condition.name());
        return false;
       }

      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& new_entries)   {
        Condition::Object* o;
        for( Condition c : new_entries )  {
          o = c.access();
          m_entries.insert(std::make_pair(o->hash,o));
        }
      }

      /// Full cleanup of all managed conditions.
      virtual void clear()   {
        for_each(m_entries.begin(), m_entries.end(), Operators::poolRemove(*this));
        m_entries.clear();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(Condition::key_type key)  const   {
        auto i=find_if(m_entries.begin(), m_entries.end(), Operators::keyFind(key));
        return i==m_entries.end() ? Condition() : (*i).second;
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual size_t select(Condition::key_type key, RangeConditions& result)
      {  return loop(result, Operators::keyedSelect(key,result));      }

      /// Select the conditons, used also by the DetElement of the condition
      virtual size_t select_all(const ConditionsSelect& result)
      {  return loop(result, Operators::operatorWrapper(result));      }

      /// Select the conditons, used also by the DetElement of the condition
      virtual size_t select_all(RangeConditions& result)
      {  return loop(result, Operators::sequenceSelect(result));       }

      /// Select the conditons, used also by the DetElement of the condition
      virtual size_t select_all(ConditionsPool& result)
      {  return loop(result, Operators::poolSelect(result));           }
    };

    /// Class implementing the conditions update pool for a given IOV type
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING, typename BASE> class ConditionsMappedUpdatePool 
      : public ConditionsMappedPool<MAPPING,BASE>
    {
      typedef ConditionsMappedPool<MAPPING,BASE> Self;
      typedef typename ConditionsMappedPool<MAPPING,BASE>::key_type key_type;
    public:
      /// Default constructor
      ConditionsMappedUpdatePool(ConditionsManager mgr)
        : ConditionsMappedPool<MAPPING,BASE>(mgr) { }

      /// Default destructor
      virtual ~ConditionsMappedUpdatePool()  { }

      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual size_t popEntries(UpdatePool::UpdateEntries& entries)   {
        ClearOnReturn<MAPPING> clear(this->Self::m_entries);
        return this->Self::loop(entries, [&entries](const std::pair<key_type,Condition::Object*>& o) {
            entries[o.second->iov].push_back(Condition(o.second));});
#if 0
        MAPPING& m = this->ConditionsMappedPool<MAPPING,BASE>::m_entries;
        size_t len = entries.size();
        size_t len = m.size();
        if ( !m.empty() )  {
          for(typename MAPPING::iterator i=m.begin(); i!=m.end(); ++i)   {
            Condition::Object* o = (*i).second;
            entries[o->iov].push_back(Condition(o));
          }
          m.clear();
        }
        return len;
#endif
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual void select_range(Condition::key_type key,
                                const Condition::iov_type& req, 
                                RangeConditions& result)
      {
        //return this->Self::loop(entries, [&entries](const std::pair<key_type,Condition::Object*>& o) {
        //    entries[o.second->iov].push_back(Condition(o.second));});
        MAPPING& m = this->ConditionsMappedPool<MAPPING,BASE>::m_entries;
        if ( !m.empty() )   {
          unsigned int req_typ = req.iovType ? req.iovType->type : req.type;
          const IOV::Key& req_key = req.key();
          for(typename MAPPING::const_iterator i=m.begin(); i != m.end(); ++i)  {
            Condition::Object* o = (*i).second;
            if ( key == o->hash )  {
              const IOV* _iov = o->iov;
              unsigned int typ = _iov->iovType ? _iov->iovType->type : _iov->type;
              if ( req_typ == typ )   {
                if ( IOV::key_is_contained(_iov->key(),req_key) )
                  // IOV test contained in key. Take it!
                  result.push_back(o);
                else if ( IOV::key_overlaps_lower_end(_iov->key(),req_key) )
                  // IOV overlap on test on the lower end of key
                  result.push_back(o);
                else if ( IOV::key_overlaps_higher_end(_iov->key(),req_key) )
                  // IOV overlap of test on the higher end of key
                  result.push_back(o);
              }
            }
          }
        }
      }
    };
  }    /* End namespace Conditions               */
}      /* End namespace DD4hep                   */
#endif /* DDCOND_CONDITIONSMAPPEDPOOL_H          */

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
#include "DD4hep/InstanceCount.h"

using DD4hep::Handle;
using namespace DD4hep::Conditions;

/// Default constructor
template<typename MAPPING, typename BASE>
ConditionsMappedPool<MAPPING,BASE>::ConditionsMappedPool(ConditionsManager mgr) : BASE(mgr)  {
  InstanceCount::increment(this);
}

/// Default destructor
template<typename MAPPING, typename BASE>
ConditionsMappedPool<MAPPING,BASE>::~ConditionsMappedPool()  {
  clear();
  InstanceCount::decrement(this);
}

#include "DD4hep/Factories.h"
namespace {
  typedef Condition::key_type key_type;
  ConditionsManager _mgr(int argc, char** argv)  {
    if ( argc > 0 )  {
      ConditionsManagerObject* m = (ConditionsManagerObject*)argv[0];
      return m;
    }
    DD4hep::except("ConditionsMappedPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return ConditionsManager(0);
  }
#define _CR(fun,x,b,y) void* fun(DD4hep::Geometry::LCDD&, int argc, char** argv) \
  {  return new b<x<key_type,Condition::Object*>,y>(_mgr(argc,argv));  }

  /// Create a conditions pool based on STL maps
  _CR(create_map_pool,std::map,ConditionsMappedPool,ConditionsPool)
  /// Create a conditions pool based on STL hash-maps (unordered_map)
  _CR(create_unordered_map_pool,std::unordered_map,ConditionsMappedPool,ConditionsPool)
  /// Create a conditions update pool based on STL maps
  _CR(create_map_update_pool,std::map,ConditionsMappedUpdatePool,UpdatePool)
  /// Create a conditions update pool based on STL hash-maps (unordered_map)
  _CR(create_unordered_map_update_pool,std::unordered_map,ConditionsMappedUpdatePool,UpdatePool)
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsMappedPool,            create_map_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsHashedPool,            create_unordered_map_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsMappedUpdatePool,      create_map_update_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsHashedUpdatePool,      create_unordered_map_update_pool)
