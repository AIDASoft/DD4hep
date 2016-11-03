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
#ifndef DD4HEP_GEOMETRY_CONDITIONDERIVED_H
#define DD4HEP_GEOMETRY_CONDITIONDERIVED_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/objects/ConditionsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Grammar definition for type binding
  class BasicGrammar;

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    class ConditionManager;
    class ConditionResolver;
    class ConditionDependency;
    class ConditionUpdateCall;

    /// ConditionResolver class used by the derived conditions calculation mechanism
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionResolver   {
    public:
      /// Standard destructor
      virtual ~ConditionResolver();
      /// Interface to access conditions by conditions key
      virtual Condition get(const ConditionKey& key) const = 0;
      /// Interface to access conditions by hash value
      virtual Condition get(unsigned int key) const = 0;
      /// Access to the conditions manager
      virtual Ref_t manager() const = 0;
      /// Access to the detector description instance
      virtual Geometry::LCDD& lcdd() const = 0;
      /// Required IOV value for update cycle
      virtual const IOV& requiredValidity()  const = 0;
    };

    /// ConditionUpdateContext class used by the derived conditions calculation mechanism
    /** 
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdateContext  {
    public:
      const ConditionResolver&   resolver;
      const ConditionDependency& dependency;
      Condition::iov_type*       iov;
      void*                      parameter;
      /// Initializing constructor
      ConditionUpdateContext(const ConditionResolver& r,
                             const ConditionDependency& d,
                             void* parameter,
                             Condition::iov_type& iov);
      /// Access to dependency keys
      const ConditionKey& key(size_t which)  const;
      /// Access to condition object by dependency index
      Condition condition(size_t which)  const;
      /// Access to condition object by dependency key
      Condition condition(const ConditionKey& key_value)  const;
      /// Access user parameter
      template<typename Q> Q* param()  const  {
        return static_cast<Q*>(parameter);
      }
      /// Access of other conditions data from the resolver
      template<typename T> T& get(const ConditionKey& key_value)  {
        Condition cond = resolver.get(key_value);
        if ( cond.isValid() )  {
          T& data = cond.get<T>();	    /// Bind data to wanted type
          /// Update result IOV according by and'ing the new iov structure
          iov->iov_intersection(cond.iov());
          return data;
        }
        throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing item:"+key_value.name);
      }
      /// Access of other conditions data from the resolver
      template<typename T> const T& get(const ConditionKey& key_value)  const {
        Condition cond = resolver.get(key_value);
        if ( cond.isValid() )  {
          const T& data = cond.get<T>();  /// Bind data to wanted type
          /// Update result IOV according by and'ing the new iov structure
          iov->iov_intersection(cond.iov());
          return data;
        }
        throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing item:"+key_value.name);
      }
      /// Access of other conditions data from the resolver
      template<typename T> T& get(size_t key_id)  {
        const ConditionKey& key_value = this->key(key_id);
        return this->get<T>(key_value);
      }
      /// Access of other conditions data from the resolver
      template<typename T> const T& get(size_t key_id)  const {
        const ConditionKey& key_value = this->key(key_id);
        return this->get<T>(key_value);
      }
    };

    /// Callback interface
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdateCall  {
    public:
      typedef ConditionUpdateContext Context;
    protected:
      /// Reference count
      int  m_refCount;
      /// Standard destructor
      ConditionUpdateCall() : m_refCount(1)  {                      }
      /// Standard destructor
      virtual ~ConditionUpdateCall();

    public:
      /// Add use count to the object
      ConditionUpdateCall* addRef()   {  ++m_refCount;  return this;  }
      /// Release object. May not be used any longer
      void release()  {  if ( --m_refCount <= 0 ) delete this;        }
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& target, const Context& ctxt) = 0;
    };

    /// Condition dependency definition
    /**
     *  Used by clients to update a condition.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionDependency    {
    protected:
      /// Reference count
      int                                m_refCount;

    public:
      typedef Geometry::DetElement       DetElement;
      /// Defintion of the depencency container
      typedef std::vector<ConditionKey>  Dependencies;
      /// Forward definition of the key type
      typedef Condition::key_type        key_type;

      /// Reference to the target's detector element
      DetElement                         detector;
      /// Key to the condition to be updated
      ConditionKey                       target;
      /// Dependency keys this condition depends on
      Dependencies                       dependencies;
      /// Reference to the update callback. No auto pointer. callback may be shared
      ConditionUpdateCall*               callback;

    protected:
      /// Copy constructor
      ConditionDependency(const ConditionDependency& c);
      /// Assignment operator
      ConditionDependency& operator=(const ConditionDependency& c);
      /// Default destructor
      virtual ~ConditionDependency();

    public:
      /// Initializing constructor
      ConditionDependency(const ConditionKey& tar, const Dependencies deps, ConditionUpdateCall* call);
      /// Initializing constructor used by builder
      ConditionDependency(const ConditionKey& tar, ConditionUpdateCall* call);
      /// Default constructor
      ConditionDependency();
      /// Access the dependency key
      key_type key()  const           {  return target.hash;         }
      /// Add use count to the object
      ConditionDependency* addRef()   {  ++m_refCount; return this;  }
      /// Release object. May not be used any longer
      void release()  {  if ( --m_refCount <= 0 ) delete this;       }
    };

    /// Condition dependency builder
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class DependencyBuilder  {
    protected:
      /// The created dependency
      ConditionDependency* m_dependency;
    public:
      /// Initializing constructor
      DependencyBuilder(const ConditionKey& target, ConditionUpdateCall* call);
      /// Default destructor
      virtual ~DependencyBuilder();
      /// Access underlying object directly
      ConditionDependency* operator->()            {   return m_dependency;   }
      /// Add a new dependency
      void add(const ConditionKey& source);
      /// Release the created dependency and take ownership.
      ConditionDependency* release();
    };

    /// Initializing constructor
    inline ConditionUpdateContext::ConditionUpdateContext(const ConditionResolver& resolv,
                                                          const ConditionDependency& dep,
                                                          void* user_param,
                                                          Condition::iov_type& iov_ref)
      : resolver(resolv), dependency(dep), iov(&iov_ref), parameter(user_param)
    {
    }

    /// Access to dependency keys
    inline const ConditionKey& ConditionUpdateContext::key(size_t which)  const  {
      return dependency.dependencies[which];
    }

    /// Access to condition object by dependency key
    inline Condition ConditionUpdateContext::condition(const ConditionKey& key_value)  const  {
      Condition c = resolver.get(key_value);
      if ( c.isValid() ) return c;
      throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing condition:"+key_value.name);
    }

    /// Access to condition object by dependency index
    inline Condition ConditionUpdateContext::condition(size_t which)  const   {
      const ConditionKey& key_value = this->key(which);
      return this->condition(key_value);
    }

  }       /* End namespace Conditions               */
}         /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITIONDERIVED_H     */
