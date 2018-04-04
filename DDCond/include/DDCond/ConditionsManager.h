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
#ifndef DDCOND_CONDITIONSMANAGER_H
#define DDCOND_CONDITIONSMANAGER_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/ComponentProperties.h"

// C/C++ include files
#include <set>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class UserPool;
    class ConditionsPool;
    class ConditionsSlice;
    class ConditionsCleanup;
    class ConditionsIOVPool;
    class ConditionsDataLoader;
    class ConditionsManagerObject;
    class ConditionUpdateUserContext;
    
    /// Manager class for condition handles
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsManager : public Handle<ConditionsManagerObject> {
    public:

      /// Standard object type
      typedef ConditionsManagerObject  Object;
      typedef ConditionsDataLoader     Loader;
      typedef std::vector<IOVType>     IOVTypes;

      /// Result of a prepare call to the conditions manager
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Result  {
      public:
        size_t selected = 0;
        size_t loaded   = 0;
        size_t computed = 0;
        size_t missing  = 0;
        Result() = default;
        Result(const Result& result) = default;
        Result& operator=(const Result& result) = default;
        size_t total() const { return selected+computed+loaded; }
        /// Add results
        Result& operator +=(const Result& result);
        /// Subtract results
        Result& operator -=(const Result& result);
      };

    public:

      /// Static accessor if installed as an extension
      /**
       * Note:
       *
       * There may very well be several instances of a 
       * conditions manager. Do not think this is a sort of
       * 'static constructor'. This would be a big mistake!.
       */
      template <typename T> static ConditionsManager from(T& host);

      /// Initializing constructor
      ConditionsManager(Detector& description);

      /// Default constructor
      ConditionsManager() = default;

      /// Constructor to be used with valid pointer to object
      ConditionsManager(Object* p) : Handle<Object>(p) {}

      /// Constructor to assing handle of the same type
      ConditionsManager(const ConditionsManager& c) = default;

      /// Constructor to be used assigning from different type
      template <typename Q> ConditionsManager(const Handle<Q>& e) : Handle<Object>(e) {}

      /// Default destructor
      ~ConditionsManager() = default;

      /// Assignment operator
      ConditionsManager& operator=(const ConditionsManager& c) = default;
      
      /// Initialize the object after having set the properties
      ConditionsManager& initialize();

      /// Access the detector description
      Detector& detectorDescription()  const;
      
      /// Access to properties
      Property& operator[](const std::string& property_name) const;

      /// Access to the property manager
      PropertyManager& properties()  const;

      /// Access the conditions loader
      Loader* loader()  const;

      /// Access the used/registered IOV types
      const std::vector<const IOVType*> iovTypesUsed() const;

      /// Access IOV by its name
      const IOVType* iovType (const std::string& iov_name) const;

      /// Access conditions multi IOV pool by iov type
      ConditionsIOVPool* iovPool(const IOVType& type)  const;

      /// Create IOV from string
      void fromString(const std::string& iov_str, IOV& iov)  const;

      /// Register new IOV type if it does not (yet) exist.
      /** Returns (false,pointer) if IOV existed and
       *  (true,pointer) if new IOV was registered to the manager.
       */
      std::pair<bool, const IOVType*> registerIOVType(size_t iov_type, const std::string& iov_name) const;

      /// Register IOV with type and key
      ConditionsPool* registerIOV(const IOV& iov) const;

      /// Register IOV with type and key
      ConditionsPool* registerIOV(const IOVType& typ, IOV::Key key) const;

      /// Register IOV with type and key
      ConditionsPool* registerIOV(const std::string& iov_rep)  const;

      /// Register new condition with the conditions store. Unlocked version, not multi-threaded
      bool registerUnlocked(ConditionsPool& pool, Condition cond) const;
      
      /// Push all pending updates to the conditions store. 
      /** Note:
       *  This does not yet make the new conditions availible to the clients
       */
      void pushUpdates() const;

      /// Invoke cache cleanup with user defined policy
      void clean(const ConditionsCleanup& cleaner)  const;

      /// Clean conditions, which are above the age limit.
      void clean(const IOVType* typ, int max_age) const;

      /// Full cleanup of all managed conditions.
      void clear()  const;
      
      /// Create empty user pool object
      std::unique_ptr<UserPool> createUserPool(const IOVType* iovT)  const;

      /// Prepare all updates to the clients with the defined IOV
      Result prepare(const IOV&                  required_validity,
                     ConditionsSlice&            slice,
                     ConditionUpdateUserContext* ctxt=0)  const;
    };
    
    /// Add results
    inline ConditionsManager::Result&
    ConditionsManager::Result::operator +=(const Result& result)  {
      selected += result.selected;
      loaded   += result.loaded;
      computed += result.computed;
      missing  += result.missing;
      return *this;
    }
    /// Subtract results
    inline ConditionsManager::Result&
    ConditionsManager::Result::operator -=(const Result& result)  {
      selected -= result.selected;
      loaded   -= result.loaded;
      computed -= result.computed;
      missing  -= result.missing;
      return *this;
    }
  }       /* End namespace cond        */
}         /* End namespace dd4hep            */
#endif    /* DDCOND_CONDITIONSMANAGER_H      */
