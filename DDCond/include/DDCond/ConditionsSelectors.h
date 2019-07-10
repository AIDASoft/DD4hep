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
#ifndef DDCOND_CONDITIONSSELECTORS_H
#define DDCOND_CONDITIONSSELECTORS_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/detail/ConditionsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /// Namespace for condition operators to avoid clashes
    namespace Operators {

      /// Definition of the selector object base class to cover type definitions
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Cond__Oper {
      public:
        typedef Condition cond_t;
        typedef Condition::Object object_t;
        typedef std::pair<const Condition::key_type,Condition> mapentry_t;
        typedef std::pair<const Condition::key_type,object_t*> ptr_mapentry_t;
      };

      /// Sequential container select operator for conditions mappings
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename T> struct SequenceSelect : public Cond__Oper  {
        T& mapping;
        SequenceSelect(T& o) : mapping(o) {                                            }
        bool operator()(Condition::Object* o)  const
        { mapping.emplace(mapping.end(), o); return true;                              }
      };

      /// Mapped container selection operator for conditions mappings
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
     template <typename T> struct MapSelect : public Cond__Oper   {
        T& mapping;
        MapSelect(T& o) : mapping(o) {                                               }
        bool operator()(Condition::Object* o)  const
        { return mapping.emplace(o->hash,o).second;                                  }
      };

      /// Helper to collect conditions using a ConditionsSelect base class
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename T> struct MapConditionsSelect : public ConditionsSelect  {
        T& mapping;
        MapConditionsSelect(T& o) : mapping(o) {                                     }
        virtual bool operator()(Condition::Object* o)  const
        { return mapping.emplace(o->hash,o).second;                                  }
        virtual size_t size() const  { return mapping.size();                        }
      };
      
      /// Helper to insert objects into a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename T> struct PoolSelect : public Cond__Oper  {
        T& pool;
        PoolSelect(T& o) : pool(o)                           {                         }
        bool operator()(Condition::Object* o)  const         { return pool.insert(o);  }
      };

      /// Helper to remove objects from a conditions pool. The removed condition is deleted.
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename T> struct PoolRemove : public Cond__Oper  {
        T& pool;
        PoolRemove(T& o) : pool(o)         {                                           }
        bool operator()(object_t* o) const { pool.onRemove(o); delete(o); return true; }
      };
    
      /// Container select operator for conditions mappings with conditions flagged active
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename T> struct ActiveSelect : public Cond__Oper {
      public:
        T& collection;
        ActiveSelect(T& p) : collection(p) {}
        bool operator()(object_t* o)  const   {
          if ( (o->flags & Condition::ACTIVE) )  {
            collection.emplace(collection.end(),o);
            return true;
          }
          return false;
        }
      };

      /// Helper to select keyed objects from a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template<typename collection_type> class KeyedSelect : public Cond__Oper  {
        cond_t::key_type key;
        collection_type& collection;
      public:
        KeyedSelect(cond_t::key_type k, collection_type& p) : key(k), collection(p) {  }
        bool operator()(object_t* o) const {
          if ( o->hash == key )  {
            collection.emplace(collection.end(),o);
            return true;
          }
          return false;
        }
      };

      /// Helper to select condition objects by hash key from a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class KeyFind : public Cond__Oper  {
        cond_t::key_type hash;
      public:
        KeyFind(cond_t::key_type h) : hash(h)  {                   }
        bool operator()(const object_t* o) const       {  return o->hash == hash;      }
      };

      /// Helper to wrap another object and make it copyable
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename OPER> class OperatorWrapper : public Cond__Oper  {
      public:
        typedef OPER operator_t;
        typedef OperatorWrapper<operator_t> wrapper_t;
        operator_t& oper;
      public:
        OperatorWrapper(operator_t& o) : oper(o)       {                               }
        bool operator()(object_t* o) const             { return oper(o);               }
        bool operator()(const cond_t& o) const         { return oper(o.ptr());         }
        bool operator()(const mapentry_t& o) const     { return oper(o.second.ptr());  }
        bool operator()(const ptr_mapentry_t& o) const { return oper(o.second);        }
      };

      /// Arbitrary wrapper for user defined conditions operators
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename OPER> class ConditionsOperation : public Cond__Oper  {
      public:
        typedef OPER operator_t;
        typedef ConditionsOperation<operator_t> wrapper_t;
        operator_t oper;
      public:
        ConditionsOperation(const operator_t& o) : oper(o)     {                       }
        bool operator()(object_t* o) const             { return oper(o);               }
        bool operator()(const cond_t& o) const         { return oper(o.ptr());         }
        bool operator()(const mapentry_t& o) const     { return oper(o.second.ptr());  }
        bool operator()(const ptr_mapentry_t& o) const { return oper(o.second);        }
      };
    
      /// Helper function to create a OperatorWrapper<T> object from the argument type
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename oper_type> OperatorWrapper<oper_type> operatorWrapper(oper_type& oper) {
        return OperatorWrapper<oper_type>(oper);
      }

      /// Helper to create functor to remove objects from a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename P> inline ConditionsOperation<PoolRemove<P> > poolRemove(P& pool)
      { return ConditionsOperation<PoolRemove<P> >(PoolRemove<P>(pool));  }

      /// Helper to create functor to insert objects into a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename P> inline ConditionsOperation<PoolSelect<P> > poolSelect(P& pool)
      { return ConditionsOperation<PoolSelect<P> >(PoolSelect<P>(pool));  }

      /// Helper to create functor to collect conditions using a ConditionsSelect base class
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename T> inline MapConditionsSelect<T> mapConditionsSelect(T& collection)
      {  return MapConditionsSelect<T>(collection);      }

      /// Helper to create functor to select objects from a conditions pool into a sequential container
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename C> inline 
      ConditionsOperation<SequenceSelect<C> > sequenceSelect(C& coll) {
        typedef SequenceSelect<C> operator_t;
        return ConditionsOperation<operator_t>(operator_t(coll));
      }

      /// Helper to create functor to select objects from a conditions pool into a mapped container
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename C> inline 
      ConditionsOperation<SequenceSelect<C> > mapSelect(C& coll) {
        typedef MapSelect<C> operator_t;
        return ConditionsOperation<operator_t>(operator_t(coll));
      }

      /// Helper to select active objects from a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename C> inline ConditionsOperation<ActiveSelect<C> > activeSelect(C& coll)
      { return ConditionsOperation<ActiveSelect<C> >(ActiveSelect<C>(coll)); }

      /// Helper to create functor to select keyed objects from a conditions pool
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      template <typename C> inline 
      ConditionsOperation<KeyedSelect<C> > keyedSelect(Condition::key_type key, C& coll)
      { return ConditionsOperation<KeyedSelect<C> >(KeyedSelect<C>(key, coll)); }

      /// Helper to create functor to find conditions objects by hash key
      /** 
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      ConditionsOperation<KeyFind> inline  keyFind(Condition::key_type key)
      { return ConditionsOperation<KeyFind>(KeyFind(key)); }

    }        /* End namespace Operators            */
  }          /* End namespace cond           */
}            /* End namespace dd4hep               */
#endif       /* DDCOND_CONDITIONSSELECTORS_H       */
