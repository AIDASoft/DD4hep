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
#ifndef DDCOND_CONDITIONSLINEARPOOL_H
#define DDCOND_CONDITIONSLINEARPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"

// C/C++ include files
#include <list>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Class implementing the conditions collection for a given IOV type
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING, typename BASE> 
    class ConditionsLinearPool : public BASE   {
    protected:
      typedef MAPPING Mapping;
      Mapping      m_conditions;

    public:
      /// Default constructor
      ConditionsLinearPool();

      /// Default destructor
      virtual ~ConditionsLinearPool();

      /// Register a new condition to this pool
      virtual void insert(Condition condition)    {
        m_conditions.insert(m_conditions.end(),condition.ptr());
      }

      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& new_entries)   {
        for(RangeConditions::iterator i=new_entries.begin(); i!=new_entries.end(); ++i)
          m_conditions.insert(m_conditions.end(),(*i).ptr());
      }

      /// Register a new condition to this pool
      virtual Condition insertEntry(ConditionsPool* pool, Entry* cond)   {
        MAPPING& m = this->ConditionsLinearPool<MAPPING,BASE>::m_conditions;
        Condition c = this->create(pool, cond);
        m.insert(m.end(),c.ptr());
        return c;
      }

      /// Full cleanup of all managed conditions.
      virtual void clear()   {
        for_each(m_conditions.begin(), m_conditions.end(), DD4hep::deleteObject<Condition::Object>);
        m_conditions.clear();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(DetElement det, const std::string& cond_name)  const   {
        if ( !m_conditions.empty() )  {
          int hash = DD4hep::hash32(cond_name);
          DetElement::Object* ptr = det.ptr();
          for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)
            if ( this->ConditionsPool::match(ptr,hash,(*i)) ) return *i;
        }
        return Condition();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(Condition test)  const  {
        int hash = DD4hep::hash32(test->name);
        DetElement::Object* ptr = test->detector.ptr();
        for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)  {
          if ( this->ConditionsPool::match(ptr,hash,(*i)) ) return *i;
        }
        return Condition();
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual void select(DetElement det, const std::string& cond_name, RangeConditions& result)   {
        if ( !m_conditions.empty() )  {
          int hash = DD4hep::hash32(cond_name);
          DetElement::Object* ptr = det.ptr();
          for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)
            if ( this->ConditionsPool::match(ptr,hash,(*i)) ) result.push_back(*i); 
        }
      }

      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_used(RangeConditions& result)  {
        for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)  {
          Condition::Object* c = *i;
          if ( (c->flags & Interna::ConditionObject::ACTIVE) ) result.push_back(c);
#if 0
          // Too slow. Simple check the conditions flag. If it is set to ACTIVE, take it.
          const Container::Entries& cond = c->detector->conditions->elements();
          Container::Entries::const_iterator k=cond.find(c->hash);
          if ( k != cond.end() )  {
            if ( (*k).second->pool == this )  {
              result.push_back(c);
            }
          }
#endif
        }
      }

      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_all(RangeConditions& result) {
        for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)
          result.push_back(*i); 
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual void select_range(DetElement det,
                                const std::string& cond_name,
                                const IOV& req,
                                RangeConditions& result)
      {
        if ( !m_conditions.empty() )   {
          unsigned int req_typ = req.iovType ? req.iovType->type : req.type;
          const IOV::Key& req_key = req.key();
          int hash = DD4hep::hash32(cond_name);
          DetElement::Object* ptr = det.ptr();
          for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)  {
            if ( this->ConditionsPool::match(ptr,hash,(*i)) )   {
              const IOV* _iov = (*i)->iov;
              unsigned int typ = _iov->iovType ? _iov->iovType->type : _iov->type;
              if ( req_typ == typ )   {
                if ( IOV::key_is_contained(_iov->key(),req_key) )
                  // IOV test contained in key. Take it!
                  result.push_back(*i);
                else if ( IOV::key_overlaps_lower_end(_iov->key(),req_key) )
                  // IOV overlap on test on the lower end of key
                  result.push_back(*i);
                else if ( IOV::key_overlaps_higher_end(_iov->key(),req_key) )
                  // IOV overlap of test on the higher end of key
                  result.push_back(*i);
              }
            }
          }
        }
      }

    };


    template<typename MAPPING> class ConditionsLinearReplacementPool 
      : public ConditionsLinearPool<MAPPING,ReplacementPool>
    {
    public:
      /// Default constructor
      ConditionsLinearReplacementPool() : ConditionsLinearPool<MAPPING,ReplacementPool>() {}

      /// Default destructor
      virtual ~ConditionsLinearReplacementPool()  {}
      /// Full cleanup of all managed conditions.
      virtual void clear()   {
        this->ConditionsLinearPool<MAPPING,ReplacementPool>::m_conditions.clear();
      }

      /// Pop conditions. May overloade for performance reasons!
      virtual void popEntries(RangeConditions& entries)   {
        MAPPING& m = this->ConditionsLinearPool<MAPPING,ReplacementPool>::m_conditions;
        if ( !m.empty() )  {
          for(typename MAPPING::iterator i=m.begin(); i!=m.end(); ++i)
            entries.push_back((*i));        
          m.clear();
        }
      }
    };


    template<typename MAPPING> class ConditionsLinearUpdatePool 
      : public ConditionsLinearPool<MAPPING,UpdatePool>
    {
    public:
      /// Default constructor
      ConditionsLinearUpdatePool() : ConditionsLinearPool<MAPPING,UpdatePool>() {}

      /// Default destructor
      virtual ~ConditionsLinearUpdatePool()  {}

      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual void popEntries(UpdatePool::UpdateEntries& entries)   {
        MAPPING& m = this->ConditionsLinearPool<MAPPING,UpdatePool>::m_conditions;
        if ( !m.empty() )  {
          for(typename MAPPING::iterator i=m.begin(); i!=m.end(); ++i)   {
            Interna::ConditionObject* o = *i;
            entries[o->iov].push_back(Condition(o));
          }
          m.clear();        
        }
      }
    };


  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif // DDCOND_CONDITIONSLINEARPOOL_H

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
//#include "DDCond/ConditionsLinearPool.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace DD4hep::Conditions;

/// Default constructor
template<typename MAPPING, typename BASE>
ConditionsLinearPool<MAPPING,BASE>::ConditionsLinearPool() : BASE() {
  InstanceCount::increment(this);
}

/// Default destructor
template<typename MAPPING, typename BASE>
ConditionsLinearPool<MAPPING,BASE>::~ConditionsLinearPool()  {
  clear();
  InstanceCount::decrement(this);
}

#include "DD4hep/Factories.h"
namespace {
  void* create_vector_pool(DD4hep::Geometry::LCDD&, int, char**)
  {  return new ConditionsLinearPool<std::vector<Condition::Object*> , ConditionsPool>();  }
  void* create_list_pool(DD4hep::Geometry::LCDD&, int, char**)
  {  return new ConditionsLinearPool<std::list<Condition::Object*> , ConditionsPool>();    }
  void* create_replacement_pool(DD4hep::Geometry::LCDD&, int, char**)  {
    ReplacementPool* pool = new ConditionsLinearReplacementPool<std::vector<Condition::Object*> >();
    return pool;
  }
  void* create_update_pool(DD4hep::Geometry::LCDD&, int, char**)  {
    UpdatePool* pool = new ConditionsLinearUpdatePool<std::vector<Condition::Object*> >();
    return pool;
  }
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearPool,      create_vector_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearListPool,  create_list_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearVectorPool,create_vector_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearReplacementPool, create_replacement_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearUpdatePool, create_update_pool)
