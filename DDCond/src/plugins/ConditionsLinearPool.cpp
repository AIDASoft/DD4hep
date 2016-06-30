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
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsInterna.h"

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
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING, typename BASE> 
    class ConditionsLinearPool : public BASE   {
    protected:
      typedef BASE    Base;
      typedef MAPPING Mapping;
      Mapping           m_conditions;
    public:
      /// Default constructor
      ConditionsLinearPool(ConditionsManager mgr);

      /// Default destructor
      virtual ~ConditionsLinearPool();

      /// Total entry count
      virtual int count()  const   {
	return (int)m_conditions.size();
      }
      /// Check if detector and name match
      bool __match(const DetElement::Object* det, int hash, const Condition::Object* c)  const {
        return det == c->detector.ptr() && hash == c->hash;
      }

      /// Register a new condition to this pool
      virtual void insert(Condition condition)    {
	Condition::Object* c = condition.access();
        m_conditions.insert(m_conditions.end(),c);
      }

      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& new_entries)   {
        for(RangeConditions::iterator i=new_entries.begin(); i!=new_entries.end(); ++i)  {
	  Condition::Object* c = (*i).access();
          m_conditions.insert(m_conditions.end(),c);
	}
      }

      /// Register a new condition to this pool
      virtual Condition insertEntry(ConditionsPool* pool, Entry* cond)   {
        Condition c = this->create(pool, cond);
	this->insert(c);
        return c;
      }

      /// Full cleanup of all managed conditions.
      virtual void clear()   {
	for(typename Mapping::iterator i=m_conditions.begin(); i != m_conditions.end(); ++i)
	  this->ConditionsPool::onRemove(*i);
	for(typename Mapping::iterator i=m_conditions.begin(); i != m_conditions.end(); ++i)
	  DD4hep::deleteObject<Condition::Object>(*i);
        m_conditions.clear();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(DetElement det, const std::string& cond_name)  const   {
        if ( !m_conditions.empty() )  {
          int hash = DD4hep::hash32(cond_name);
          DetElement::Object* ptr = det.ptr();
          for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)
            if ( __match(ptr,hash,(*i)) ) return *i;
        }
        return Condition();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(Condition test)  const  {
        int hash = DD4hep::hash32(test->name);
        DetElement::Object* ptr = test->detector.ptr();
        for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)  {
          if ( __match(ptr,hash,(*i)) ) return *i;
        }
        return Condition();
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual void select(DetElement det, const std::string& cond_name, RangeConditions& result)   {
        if ( !m_conditions.empty() )  {
          int hash = DD4hep::hash32(cond_name);
          DetElement::Object* ptr = det.ptr();
          for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)
            if ( __match(ptr,hash,(*i)) ) result.push_back(*i); 
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

      /// Select the conditons, used also by the DetElement of the condition
      virtual void select_all(ConditionsPool& selection_pool) {
        for(typename Mapping::const_iterator i=m_conditions.begin(); i!=m_conditions.end(); ++i)  {
	  selection_pool.insert(*i);
	}
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
            if ( __match(ptr,hash,(*i)) )   {
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

    /// Class implementing the conditions user pool for a given IOV type
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING> class ConditionsLinearUserPool 
      : public ConditionsLinearPool<MAPPING,UserPool>
    {
      IOV m_iov;
    public:
      /// Default constructor
      ConditionsLinearUserPool(ConditionsManager mgr) 
	: ConditionsLinearPool<MAPPING,UserPool>(mgr), m_iov(0)
      {
	this->ConditionsPool::iov = &m_iov;
	this->ConditionsPool::iovType = 0;
      }

      /// Default destructor
      virtual ~ConditionsLinearUserPool()  {}

      /// Access the interval of validity for this user pool
      virtual const IOV& validity() const  {
	return this->m_iov;
      }
      /// Update interval of validity for this user pool (should only be called by ConditionsManager)
      virtual void setValidity(const IOV& value)   {
	this->m_iov = value;
	this->ConditionsPool::iovType = m_iov.iovType;
      }

      /// Full cleanup of all managed conditions.
      virtual void clear()   {
	m_iov = IOV(0);
        this->ConditionsLinearPool<MAPPING,UserPool>::m_conditions.clear();
      }
    };


    /// Class implementing the conditions update pool for a given IOV type
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING> class ConditionsLinearUpdatePool 
      : public ConditionsLinearPool<MAPPING,UpdatePool>
    {
    public:
      /// Default constructor
      ConditionsLinearUpdatePool(ConditionsManager mgr)
	: ConditionsLinearPool<MAPPING,UpdatePool>(mgr) 
      {
      }

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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"

using namespace DD4hep::Conditions;

/// Default constructor
template<typename MAPPING, typename BASE>
ConditionsLinearPool<MAPPING,BASE>::ConditionsLinearPool(ConditionsManager mgr) : BASE(mgr)  {
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
  ConditionsManager _mgr(int argc, char** argv)  {
    if ( argc > 0 )  {
      ConditionsManager::Object* m = (ConditionsManager::Object*)argv[0];
      return m;
    }
    DD4hep::except("ConditionsLinearPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return ConditionsManager(0);
  }
  void* create_vector_pool(DD4hep::Geometry::LCDD&, int argc, char** argv)
  {  return new ConditionsLinearPool<std::vector<Condition::Object*> , ConditionsPool>(_mgr(argc,argv));  }
  void* create_list_pool(DD4hep::Geometry::LCDD&, int argc, char** argv)
  {  return new ConditionsLinearPool<std::list<Condition::Object*> , ConditionsPool>(_mgr(argc,argv));    }
  void* create_user_pool(DD4hep::Geometry::LCDD&, int argc, char** argv)  {
    UserPool* pool = new ConditionsLinearUserPool<std::vector<Condition::Object*> >(_mgr(argc,argv));
    return pool;
  }
  void* create_update_pool(DD4hep::Geometry::LCDD&, int argc, char** argv)  {
    UpdatePool* pool = new ConditionsLinearUpdatePool<std::vector<Condition::Object*> >(_mgr(argc,argv));
    return pool;
  }
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearPool,            create_vector_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearListPool,        create_list_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearVectorPool,      create_vector_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearUserPool,        create_user_pool)
DECLARE_LCDD_CONSTRUCTOR(DD4hep_ConditionsLinearUpdatePool,      create_update_pool)
