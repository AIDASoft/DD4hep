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
#ifndef DDCOND_CONDITIONSLINEARPOOL_H
#define DDCOND_CONDITIONSLINEARPOOL_H

// Framework include files
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsSelectors.h"

#include "DD4hep/Printout.h"

// C/C++ include files
#include <list>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /// Class implementing the conditions collection for a given IOV type
    /**
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
    class ConditionsLinearPool : public BASE   {
    protected:
      MAPPING           m_entries;

      /// Helper function to loop over the conditions container and apply a functor
      template <typename R,typename T> size_t loop(R& result, T functor) {
        size_t len = result.size();
        for_each(m_entries.begin(),m_entries.end(),functor);
        return result.size() - len;
      }
    public:
      /// Default constructor
      ConditionsLinearPool(ConditionsManager mgr, IOV* iov);

      /// Default destructor
      virtual ~ConditionsLinearPool();

      /// Total entry count
      virtual size_t size()  const   final  {
        return m_entries.size();
      }

      /// Full cleanup of all managed conditions.
      virtual void clear()  final   {
        for_each(m_entries.begin(), m_entries.end(), Operators::poolRemove(*this));
        m_entries.clear();
      }

      /// Check if a condition exists in the pool
      virtual Condition exists(Condition::key_type key)  const  final   {
        auto i = find_if(m_entries.begin(), m_entries.end(), Operators::keyFind(key));
        return i==m_entries.end() ? Condition() : (*i);
      }

      /// Register a new condition to this pool
      virtual bool insert(Condition condition)  final 
      {  m_entries.insert(m_entries.end(),condition.access()); return true;     }

      /// Register a new condition to this pool. May overload for performance reasons.
      virtual void insert(RangeConditions& rc)  final 
      {  for_each(rc.begin(), rc.end(), Operators::sequenceSelect(m_entries));  }

      /// Select the conditions matching the DetElement and the conditions name
      virtual size_t select(Condition::key_type key, RangeConditions& result)  final 
      {  return loop(result, Operators::keyedSelect(key, result));              }

      /// Select the conditons, used also by the DetElement of the condition
      virtual size_t select_all(const ConditionsSelect& result)  final 
      {  return loop(result, Operators::operatorWrapper(result));               }

      /// Select the conditons, used also by the DetElement of the condition
      virtual size_t select_all(RangeConditions& result)  final 
      {  return loop(result, Operators::sequenceSelect(result));                }

      /// Select the conditons, used also by the DetElement of the condition
      virtual size_t select_all(ConditionsPool& result)  final 
      {  return loop(result, Operators::poolSelect(result));                    }
    };

    /// Class implementing the conditions update pool for a given IOV type
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    template<typename MAPPING, typename BASE> class ConditionsLinearUpdatePool 
      : public ConditionsLinearPool<MAPPING,BASE>
    {
    public:
      /// Default constructor
      ConditionsLinearUpdatePool(ConditionsManager mgr, IOV*)
        : ConditionsLinearPool<MAPPING,BASE>(mgr,0)
      {
        this->BASE::SetTitle("ConditionsLinearUpdatePool");
      }

      /// Default destructor
      virtual ~ConditionsLinearUpdatePool()  {
      }

      /// Adopt all entries sorted by IOV. Entries will be removed from the pool
      virtual size_t popEntries(UpdatePool::UpdateEntries& entries)   final  {
        MAPPING& m = this->ConditionsLinearPool<MAPPING,BASE>::m_entries;
        size_t len = entries.size();
        if ( !m.empty() )  {
          for(typename MAPPING::iterator i=m.begin(); i!=m.end(); ++i)   {
            Condition::Object* o = *i;
            entries[o->iov].push_back(Condition(o));
          }
          m.clear();        
        }
        return entries.size()-len;
      }

      /// Select the conditions matching the DetElement and the conditions name
      virtual void select_range(Condition::key_type key,
                                const IOV& req, 
                                RangeConditions& result)  final 
      {
        MAPPING& m = this->ConditionsLinearPool<MAPPING,BASE>::m_entries;
        if ( !m.empty() )   {
          unsigned int req_typ = req.iovType ? req.iovType->type : req.type;
          const IOV::Key& req_key = req.key();
          for(typename MAPPING::const_iterator i=m.begin(); i != m.end(); ++i)  {
            if ( key == (*i)->hash )  {
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


  } /* End namespace cond             */
} /* End namespace dd4hep                   */
#endif // DDCOND_CONDITIONSLINEARPOOL_H

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
//#include "DDCond/ConditionsLinearPool.h"
#include "DD4hep/InstanceCount.h"

using dd4hep::Handle;
using namespace dd4hep::cond;

/// Default constructor
template<typename MAPPING, typename BASE>
ConditionsLinearPool<MAPPING,BASE>::ConditionsLinearPool(ConditionsManager mgr, IOV* i)
  : BASE(mgr, i)
{
  
  this->BASE::SetName(i ? i->str().c_str() : "Unknown IOV");
  this->BASE::SetTitle("ConditionsLinearPool");
  InstanceCount::increment(this);
}

/// Default destructor
template<typename MAPPING, typename BASE>
ConditionsLinearPool<MAPPING,BASE>::~ConditionsLinearPool()  {
  clear();
  detail::deletePtr(this->BASE::iov);
  InstanceCount::decrement(this);
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

  } /* End namespace cond             */
} /* End namespace dd4hep                   */

#include "DD4hep/Factories.h"
namespace {
  using namespace dd4hep;
  
  ConditionsManager _mgr(int argc, char** argv)  {
    if ( argc > 0 )  {
      ConditionsManager::Object* m = (ConditionsManager::Object*)argv[0];
      return m;
    }
    dd4hep::except("ConditionsLinearPool","++ Insufficient arguments: arg[0] = ConditionManager!");
    return ConditionsManager(0);
  }
  IOV* _iov(int argc, char** argv)  {
    if ( argc > 1 )  {
      IOV* i = (IOV*)argv[1];
      return i;
    }
    dd4hep::except("ConditionsLinearPool","++ Insufficient arguments: arg[1] = IOV!");
    return nullptr;
  }

#define _CR(fun,x,b,y) void* fun(dd4hep::Detector&, int argc, char** argv) \
  {  return new b<x<Condition::Object*>,y>(_mgr(argc,argv), _iov(argc,argv));  }
  /// Create a conditions pool based on STL vectors
  _CR(create_vector_pool,std::vector,ConditionsLinearPool,ConditionsPool)
  /// Create a conditions pool based on STL lists
  _CR(create_list_pool,std::list,ConditionsLinearPool,ConditionsPool)
  /// Create a conditions update pool based on STL vectors
  _CR(create_vector_update_pool,std::vector,ConditionsLinearUpdatePool,UpdatePool)
  /// Create a conditions update pool based on STL list
  _CR(create_list_update_pool,std::list,ConditionsLinearUpdatePool,UpdatePool)
}

DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsLinearPool,            create_vector_pool)
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsLinearVectorPool,      create_vector_pool)
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsLinearUpdatePool,      create_vector_update_pool)
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsLinearVectorUpdatePool,create_vector_update_pool)

DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsLinearListPool,        create_list_pool)
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_ConditionsLinearListUpdatePool,  create_list_update_pool)
