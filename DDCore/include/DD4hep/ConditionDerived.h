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
#ifndef DD4HEP_DDCORE_CONDITIONDERIVED_H
#define DD4HEP_DDCORE_CONDITIONDERIVED_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/detail/ConditionsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Grammar definition for type binding
  class BasicGrammar;

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace cond   {

    /// Forward declarations
    class ConditionResolver;
    class ConditionDependency;
    class ConditionUpdateCall;

    /// ConditionUpdateUserContext class used by the derived conditions calculation mechanism
    /** 
     *  Used to pass user data to the update calls during ConditionsManager::prepare
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdateUserContext  {
    public:
      /// Default destructor
      virtual ~ConditionUpdateUserContext();
    };
    
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
      /// Access to the conditions manager
      virtual Handle<NamedObject> manager() const = 0;
      /// Access to the detector description instance
      virtual Detector& detectorDescription() const = 0;
      /// Required IOV value for update cycle
      virtual const IOV& requiredValidity()  const = 0;
      /// Accessor for the current conditons mapping
      virtual ConditionsMap& conditionsMap() const = 0;
      /// Interface to access conditions by conditions key
      virtual Condition get(const ConditionKey& key) = 0;
      /// Interface to access conditions by hash value
      virtual Condition get(Condition::key_type key) = 0;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(DetElement de) = 0;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(Condition::detkey_type key) = 0;
    };

    /// ConditionUpdateContext class used by the derived conditions calculation mechanism
    /** 
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdateContext  {
    public:
      ConditionResolver*          resolver;
      const ConditionDependency*  dependency;
      ConditionUpdateUserContext* parameter;
      IOV*                        iov;
      /// Initializing constructor
      ConditionUpdateContext(ConditionResolver* r,
                             const ConditionDependency* d,
                             ConditionUpdateUserContext* parameter,
                             IOV* iov);
      /// Throw exception on conditions access failure
      void accessFailure(const ConditionKey& key_value)  const;
      /// Access to dependency keys
      const ConditionKey& key(size_t which)  const;
      /// Access to condition object by dependency index
      Condition condition(size_t which)  const;
      /// Access to condition object by dependency key
      Condition condition(const ConditionKey& key_value)  const;
      /// Access to condition object by dependency key
      Condition conditionByHash(Condition::key_type key_value)  const;
      /// Access user parameter
      template<typename Q> Q* param()  const  {
        return static_cast<Q*>(parameter);
      }
      /// Access of other conditions data from the resolver
      template<typename T> T& get(const ConditionKey& key_value)  {
        Condition cond = resolver->get(key_value);
        if ( cond.isValid() )  {
          T& data = cond.get<T>();	    /// Bind data to wanted type
          /// Update result IOV according by and'ing the new iov structure
          iov->iov_intersection(cond.iov());
          return data;
        }
        accessFailure(key_value);
        throw std::runtime_error("ConditionUpdateCall");
      }
      /// Access of other conditions data from the resolver
      template<typename T> const T& get(const ConditionKey& key_value)  const {
        Condition cond = resolver->get(key_value);
        if ( cond.isValid() )  {
          const T& data = cond.get<T>();  /// Bind data to wanted type
          /// Update result IOV according by and'ing the new iov structure
          iov->iov_intersection(cond.iov());
          return data;
        }
        accessFailure(key_value);
        throw std::runtime_error("ConditionUpdateCall");
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
    protected:
      /// Reference count
      int  m_refCount;
      /// Standard destructor
      ConditionUpdateCall();
      /// No copy constructor
      ConditionUpdateCall(const ConditionUpdateCall& copy) = delete;
      /// Standard destructor
      virtual ~ConditionUpdateCall();
      /// No assignment operator
      ConditionUpdateCall& operator=(const ConditionUpdateCall& copy) = delete;

    public:
      /// Add use count to the object
      ConditionUpdateCall* addRef()   {
        ++m_refCount;
        return this;
      }
      /// Release object. May not be used any longer
      void release()  {
        if ( --m_refCount <= 0 )
          delete this;
      }
      /// Interface to client callback in order to update/create the condition
      virtual Condition operator()(const ConditionKey& target,
                                   const ConditionUpdateContext& ctxt) = 0;
      /// Interface to client callback for resolving references or to use data from other conditions
      virtual void resolve(Condition /* c */, ConditionResolver& /* resolver */)   {}
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
      /// Reference to the target's detector element
      DetElement                         detector;
      /// Key to the condition to be updated
      ConditionKey                       target;
      /// Dependency keys this condition depends on
      std::vector<ConditionKey>          dependencies;
      /// Reference to the update callback. No auto pointer. callback may be shared
      ConditionUpdateCall*               callback;

    protected:
      /// Copy constructor
      ConditionDependency(const ConditionDependency& c) = delete;
      /// Assignment operator
      ConditionDependency& operator=(const ConditionDependency& c) = delete;
      /// Default destructor
      virtual ~ConditionDependency();

    public:
      /// Initializing constructor used by builder
      ConditionDependency(DetElement de, Condition::itemkey_type item_key, ConditionUpdateCall* call);
      /// Initializing constructor used by builder
      ConditionDependency(DetElement de, const std::string& item, ConditionUpdateCall* call);
      /// Default constructor
      ConditionDependency();
      /// Access the dependency key
      Condition::key_type key()  const           {  return target.hash;         }
      /// Access the dependency key
      const char* name()  const                  {  return target.name.c_str(); }
      /// Add use count to the object
      ConditionDependency* addRef()              {  ++m_refCount; return this;  }
      /// Release object. May not be used any longer
      void release()                             {  if ( --m_refCount <= 0 ) delete this; }
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
      DependencyBuilder(DetElement de, Condition::itemkey_type item_key, ConditionUpdateCall* call);
      /// Initializing constructor
      DependencyBuilder(DetElement de, const std::string& item, ConditionUpdateCall* call);
      /// Default destructor
      virtual ~DependencyBuilder();
      /// Access underlying object directly
      ConditionDependency* operator->()  {   return m_dependency; }
      /// Add a new dependency
      void add(const ConditionKey& source_key);
      /// Release the created dependency and take ownership.
      ConditionDependency* release();
    };

    /// Initializing constructor
    inline ConditionUpdateContext::ConditionUpdateContext(ConditionResolver* resolv,
                                                          const ConditionDependency* dep,
                                                          ConditionUpdateUserContext* user_param,
                                                          IOV* i)
      : resolver(resolv), dependency(dep), parameter(user_param), iov(i)
    {
    }

    /// Access to dependency keys
    inline const ConditionKey& ConditionUpdateContext::key(size_t which)  const  {
      return dependency->dependencies[which];
    }

    /// Access to condition object by dependency key
    inline Condition ConditionUpdateContext::condition(const ConditionKey& key_value)  const  {
      Condition c = resolver->get(key_value);
      if ( c.isValid() ) return c;
      throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing condition:"+key_value.name);
    }
   
    /// Access to condition object by dependency index
    inline Condition ConditionUpdateContext::condition(size_t which)  const   {
      const ConditionKey& key_value = this->key(which);
      return this->condition(key_value);
    }

    /// Access to condition object by dependency key
    inline Condition ConditionUpdateContext::conditionByHash(Condition::key_type key_value)  const   {
      Condition c = resolver->get(key_value);
      if ( c.isValid() ) return c;
      throw std::runtime_error("ConditionUpdateCall: Failed to access non-existing condition.");
    }
 
  }       /* End namespace cond               */
}         /* End namespace dd4hep                   */
#endif    /* DD4HEP_DDCORE_CONDITIONDERIVED_H     */
