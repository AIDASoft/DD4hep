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
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"
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
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING, typename BASE> 
    class ConditionsMappedPool : public BASE   {
    protected:
      typedef BASE     Base;
      typedef MAPPING  Mapping;
      typedef typename BASE::key_type key_type;
      Mapping          m_entries;
    public:
      /// Default constructor
      ConditionsMappedPool(ConditionsManager mgr);

      /// Default destructor
      virtual ~ConditionsMappedPool();

      /// Total entry count
      virtual size_t count()  const   {
        return m_entries.size();
      }

      /// Register a new condition to this pool
      virtual void insert(Condition condition)    {
        Condition::Object* c = condition.access();
        m_entries.insert(std::make_pair(c->hash,c));
      }

      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& new_entries)   {
        for(RangeConditions::iterator i=new_entries.begin(); i!=new_entries.end(); ++i)  {
          Condition::Object* c = (*i).access();
          m_entries.insert(std::make_pair(c->hash,c));
        }
      }

      /// Full cleanup of all managed conditions.
      virtual void clear()   {
        for_each(m_entries.begin(), m_entries.end(), ConditionsPoolRemove(*this));
        m_entries.clear();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(Condition::key_type key)  const   {
        typename Mapping::const_iterator i=
          find_if(m_entries.begin(), m_entries.end(), HashConditionFind(key));
        return i==m_entries.end() ? Condition() : (*i).second;
      }
      /// Select the conditions matching the DetElement and the conditions name
      virtual void select(Condition::key_type key, RangeConditions& result)
      {  for_each(m_entries.begin(),m_entries.end(),keyedSelect(key,result));   }

      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_used(RangeConditions& result)
      {  for_each(m_entries.begin(),m_entries.end(),activeSelect(result));      }

      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_all(RangeConditions& result)
      {  for_each(m_entries.begin(),m_entries.end(),collectionSelect(result));  }

      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_all(ConditionsPool& selection_pool)
      {  for_each(m_entries.begin(),m_entries.end(),ConditionsPoolInsert(selection_pool)); }
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
      typedef typename ConditionsMappedPool<MAPPING,BASE>::key_type key_type;
    public:
      /// Default constructor
      ConditionsMappedUpdatePool(ConditionsManager mgr)
        : ConditionsMappedPool<MAPPING,BASE>(mgr) 
      {
      }

      /// Default destructor
      virtual ~ConditionsMappedUpdatePool()  {}

      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual void popEntries(UpdatePool::UpdateEntries& entries)   {
        MAPPING& m = this->ConditionsMappedPool<MAPPING,BASE>::m_entries;
        if ( !m.empty() )  {
          for(typename MAPPING::iterator i=m.begin(); i!=m.end(); ++i)   {
            Condition::Object* o = (*i).second;
            entries[o->iov].push_back(Condition(o));
          }
          m.clear();        
        }
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual void select_range(Condition::key_type key,
                                const Condition::iov_type& req, 
                                RangeConditions& result)
      {
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


  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif // DDCOND_CONDITIONSMAPPEDPOOL_H

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
//#include "DDCond/ConditionsMappedPool.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"

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
      ConditionsManager::Object* m = (ConditionsManager::Object*)argv[0];
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
