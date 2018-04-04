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
    class DependencyBuilder;
    class ConditionResolver;
    class ConditionDependency;
    class ConditionUpdateCall;
    class ConditionUpdateContext;
    class ConditionUpdateUserContext;

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
      /// Interface to access conditions by conditions key
      virtual Condition get(const ConditionKey& key, bool throw_if_not) = 0;
      /// Interface to access conditions by hash value
      virtual Condition get(Condition::key_type key) = 0;
      /// Interface to access conditions by hash value
      virtual Condition get(Condition::key_type key, bool throw_if_not) = 0;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(DetElement de) = 0;
      /// Interface to access conditions by hash value of the DetElement (only valid at resolve!)
      virtual std::vector<Condition> get(Condition::detkey_type key) = 0;
    };

    /// ConditionUpdateContext class used by the derived conditions calculation mechanism
    /** 
     *  This is the central object used by the functional callbacks to
     *  build derived conditions. All optionally necessary information
     *  can and must be accessed by this object.
     *
     *  Please node:
     *  1) Be careful when resolving other conditions
     *     These calls are IN GENERAL ONLY VALID AT RESOLVE !
     *     Otherwise the resulting IOV shall be wrong !
     *  2) Only accessing the conditions using the context ensure that the
     *     IOV of the resulting derived condition is correct.
     *     The conditions resolver does not affect the resulting IOV.
     *  3) Though the access to the resolver under certain circumstances
     *     is useful, you should always be aware that to IOV intersection
     *     shall be wrong, since these accessed conditions are not taken 
     *     into account.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionUpdateContext final  {
    public:
      /// Internal reference to the resolver to access other conditions (Be careful)
      ConditionResolver*          resolver;
      /// The dependency to be handled within this context
      const ConditionDependency*  dependency;
      /// The reference to the combined IOV resulting from the cumputation
      IOV*                        iov;
      /// A refernce to the user parameter
      ConditionUpdateUserContext* parameter;

    public:
      /// Initializing constructor
      ConditionUpdateContext(ConditionResolver* r,
                             const ConditionDependency* d,
                             IOV* iov,
                             ConditionUpdateUserContext* parameter);
      
      /// Throw exception on conditions access failure
      void accessFailure(const ConditionKey& key_value)  const;

      /// Access to dependency keys
      const ConditionKey& key(size_t which)  const ;

      /// Access user parameter
      template<typename Q> Q* param()  const  {
        return static_cast<Q*>(parameter);
      }

      /// Access to all conditions of a detector element.
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      std::vector<Condition> conditions(DetElement det)  const    {
        return conditions(det.key());
      }

      /// Access to all conditions of a detector element.
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      std::vector<Condition> conditions(Condition::detkey_type det_key)  const;

      /// Access to condition object by dependency key
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      Condition condition(const ConditionKey& key_value)  const;

      /// Access to condition object by dependency key
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      Condition condition(Condition::key_type key_value)  const;

      /// Access to condition object by dependency key
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      Condition condition(Condition::key_type key_value, bool throw_if_not)  const;

      /// Access of other conditions data from the resolver
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      template<typename T> T& get(const ConditionKey& key_value);

      /// Access of other conditions data from the resolver
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      template<typename T> const T& get(const ConditionKey& key_value)  const;

      /// Access of other conditions data from the resolver
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      template<typename T> T& get(Condition::key_type key_value);

      /// Access of other conditions data from the resolver
      /** Careful: This limits the validity!
       *  ONLY VALID AT RESOLVE !
       *  Otherwise the resulting IOV shall be wrong !
       */
      template<typename T> const T& get(Condition::key_type key_value)  const;
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
                                   ConditionUpdateContext& ctxt) = 0;
      /// Interface to client callback for resolving references or to use data from other conditions
      virtual void resolve(Condition /* c */, ConditionUpdateContext& /* ctxt */)   {}
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
      Condition::key_type key()  const    {  return target.hash;                   }
      /// Access the dependency key
      const char* name()  const           {  return target.name.c_str();           }
      /// Add use count to the object
      ConditionDependency* addRef()       {  ++m_refCount; return this;            }
      /// Release object. May not be used any longer
      void release()                      {  if ( --m_refCount <= 0 ) delete this; }
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
                                                          IOV* i,
                                                          ConditionUpdateUserContext* user_param)
      : resolver(resolv), dependency(dep), iov(i), parameter(user_param)
    {
    }

    /// Access to dependency keys
    inline const ConditionKey&
    ConditionUpdateContext::key(size_t which)  const  {
      return dependency->dependencies[which];
    }

    /// Access of other conditions data from the resolver
    template<typename T> inline T&
    ConditionUpdateContext::get(const ConditionKey& key_value)   {
      Condition cond = condition(key_value);
      if ( cond.isValid() )  {
        return cond.get<T>();	    /// return already bound data to wanted type
      }
      accessFailure(key_value);
      throw std::runtime_error("ConditionUpdateCall");
    }

    /// Access of other conditions data from the resolver
    template<typename T> inline const T&
    ConditionUpdateContext::get(const ConditionKey& key_value)  const  {
      Condition cond = condition(key_value);
      if ( cond.isValid() )  {
        return cond.get<T>();	    /// return already bound data to wanted type
      }
      accessFailure(key_value);
      throw std::runtime_error("ConditionUpdateCall");
    }

    /// Access of other conditions data from the resolver
    template<typename T> inline T&
    ConditionUpdateContext::get(Condition::key_type key_value)    {
      return condition(key_value).get<T>();
    }

    /// Access of other conditions data from the resolver
    template<typename T> inline const T&
    ConditionUpdateContext::get(Condition::key_type key_value)  const   {
      return condition(key_value).get<T>();
    }

  }       /* End namespace cond               */
}         /* End namespace dd4hep                   */
#endif    /* DD4HEP_DDCORE_CONDITIONDERIVED_H     */
