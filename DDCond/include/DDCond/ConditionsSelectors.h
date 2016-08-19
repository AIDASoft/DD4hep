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
#ifndef DDCOND_CONDITIONSSELECTORS_H
#define DDCOND_CONDITIONSSELECTORS_H

// Framework include files
#include "DD4hep/objects/ConditionsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    class Cond__Oper {
    public:
      typedef Condition cond_t;
      typedef Condition::Object object_t;
      typedef std::pair<const Condition::key_type,Condition> mapentry_t;
      typedef std::pair<const Condition::key_type,object_t*> ptr_mapentry_t;
    };

    /// Helper to insert objects into a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsPoolInsert : public Cond__Oper  {
      ConditionsPool& pool;
    public:
      ConditionsPoolInsert(ConditionsPool& p) : pool(p)         {                   }
      void operator()(object_t* o) const             { pool.insert(o);               }
      void operator()(const cond_t& o) const         { pool.insert(o.ptr());         }
      void operator()(const mapentry_t& o) const     { (*this)(o.second.ptr());      }
      void operator()(const ptr_mapentry_t& o) const { (*this)(o.second);            }
    };

    /// Helper to insert objects into a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class ConditionsPoolRemove : public Cond__Oper  {
      ConditionsPool& pool;
    public:
      ConditionsPoolRemove(ConditionsPool& p) : pool(p)         {                   }
      void operator()(object_t* o) const             {
	pool.onRemove(o);
	delete o;
      }
      void operator()(const cond_t& o) const         { (*this)(o.ptr());             }
      void operator()(const mapentry_t& o) const     { (*this)(o.second.ptr());      }
      void operator()(const ptr_mapentry_t& o) const { (*this)(o.second);            }
    };

    /// Helper to insert objects into a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    template<typename pool_type> class PoolInsert : public Cond__Oper  {
      pool_type& pool;
    public:
      PoolInsert(pool_type& p) : pool(p)             {                               }
      void operator()(object_t* o) const             { pool.insert(o);               }
      void operator()(const cond_t& o) const         { pool.insert(o.ptr());         }
      void operator()(const mapentry_t& o) const     { (*this)(o.second.ptr());      }
      void operator()(const ptr_mapentry_t& o) const { (*this)(o.second);            }
    };
    template <typename pool_type> PoolInsert<pool_type> poolInsert(pool_type& pool) {
      return PoolInsert<pool_type>(pool);
    }

    /// Helper to select objects from a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    template<typename collection_type> class CollectionSelect : public Cond__Oper  {
      collection_type& coll;
    public:
      CollectionSelect(collection_type& p) : coll(p) {                               }
      void operator()(object_t* o) const          { coll.insert(coll.end(),o);       }
      void operator()(const cond_t& o) const      { coll.insert(coll.end(),o.ptr()); }
      void operator()(const mapentry_t& o) const  { (*this)(o.second.ptr());         }
      void operator()(const ptr_mapentry_t& o)    { (*this)(o.second);               }
    };
    template <typename collection_type> 
      CollectionSelect<collection_type> collectionSelect(collection_type& collection) {
      return CollectionSelect<collection_type>(collection);
    }

    /// Helper to select active objects from a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    template<typename collection_type> class ActiveSelect : public Cond__Oper  {
      collection_type& collection;
    public:
      ActiveSelect(collection_type& p) : collection(p) {}
      void operator()(object_t* o)  const {
	if ( (o->flags & cond_t::ACTIVE) )
	  collection.insert(collection.end(),o); 
      }
      void operator()(const cond_t& o) const          { (*this)(o.ptr());            }
      void operator()(const mapentry_t& o) const      { (*this)(o.second.ptr());     }
      void operator()(const ptr_mapentry_t& o)  const { (*this)(o.second);           }
    };
    template <typename collection_type> 
      ActiveSelect<collection_type> activeSelect(collection_type& active) {
      return ActiveSelect<collection_type>(active);
    }

    /// Helper to select keyed objects from a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    template<typename collection_type> class KeyedSelect : public Cond__Oper  {
      cond_t::key_type key;
      collection_type& collection;
    public:
    KeyedSelect(cond_t::key_type k, collection_type& p) : key(k), collection(p) {}
      void operator()(object_t* o) const {
	if ( o->hash == key )
	  collection.insert(collection.end(),o); 
      }
      void operator()(const cond_t& o) const          { (*this)(o.ptr());            }
      void operator()(const mapentry_t& o) const      { (*this)(o.second.ptr());     }
      void operator()(const ptr_mapentry_t& o) const  { (*this)(o.second);           }
    };
    template <typename collection_type> 
      KeyedSelect<collection_type> keyedSelect(Condition::key_type k, collection_type& keyed) {
      return KeyedSelect<collection_type>(k, keyed);
    }

    /// Helper to select condition objects by hash key from a conditions pool
    /** 
     *  \author  M.Frank
     *  \version 1.0
     */
    class HashConditionFind : public Cond__Oper  {
      cond_t::key_type hash;
    public:
      HashConditionFind(cond_t::key_type h) : hash(h)  {                   }
      bool operator()(const cond_t& o) const         {  return o->hash == hash;      }
      bool operator()(const object_t* o) const       {  return o->hash == hash;      }
      bool operator()(const mapentry_t& o) const     {  return (*this)(o.second);    }
      bool operator()(const ptr_mapentry_t& o) const {  return (*this)(o.second);    }
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSSELECTORS_H       */
