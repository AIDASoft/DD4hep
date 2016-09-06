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
#ifndef DDCOND_CONDITIONSMANAGER_H
#define DDCOND_CONDITIONSMANAGER_H

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ComponentProperties.h"
#include "DDCond/ConditionsDependencyCollection.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class Entry;
    class UserPool;
    class ConditionsPool;
    class ConditionsIOVPool;
    class ConditionDependency;
    class ConditionsDataLoader;
    class ConditionUpdateContext;
    class ConditionsManagerObject;
    
    /// Manager class for condition handles
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsManager : public Handle<ConditionsManagerObject> {
    public:

      /// Standard object type
      typedef ConditionsManagerObject            Object;
      typedef ConditionsDataLoader               Loader;
      typedef std::vector<IOVType>               IOVTypes;
      typedef std::map<IOVType*,Container>       TypeConditions;
      typedef std::map<DetElement,Container>     DetectorConditions;
      typedef ConditionDependency                Dependency;
      typedef ConditionsDependencyCollection     Dependencies;

    public:

      /// Static accessor if installed as an extension
      /**
       * Note:
       *
       * There may very well be several instances of a 
       * conditions manager. Do not think this is a sort of
       * 'static constructor'. This woul dbe a big mistake!.
       */
      template <typename T> static ConditionsManager from(T& host);

      /// Initializing constructor
      ConditionsManager(LCDD& lcdd);

      /// Default constructor
      ConditionsManager() : Handle<Object>() {}

      /// Constructor to be used with valid pointer to object
      ConditionsManager(Object* p) : Handle<Object>(p) {}

      /// Constructor to assing handle of the same type
      ConditionsManager(const ConditionsManager& c) : Handle<Object>(c) {}

      /// Constructor to be used assigning from different type
      template <typename Q> ConditionsManager(const Handle<Q>& e) : Handle<Object>(e) {}

      /// Default destructor
      ~ConditionsManager();

      /// Initialize the object after having set the properties
      ConditionsManager& initialize();

      /// Access to properties
      Property& operator[](const std::string& property_name) const;

      /// Access to the property manager
      PropertyManager& properties()  const;

      /// Access the conditions loader
      Handle<Loader> loader()  const;

      /// Access the used/registered IOV types
      const std::vector<const IOVType*> iovTypesUsed() const;

      /// Access IOV by its name
      const IOVType* iovType (const std::string& iov_name) const;

      /// Access conditions multi IOV pool by iov type
      ConditionsIOVPool* iovPool(const IOVType& type)  const;

      /// Create IOV from string
      void fromString(const std::string& iov_str, IOV& iov);

      /// Register new IOV type if it does not (yet) exist.
      /** Returns (false,pointer) if IOV existed and
       *  (true,pointer) if new IOV was registered to the manager.
       */
      std::pair<bool, const IOVType*> registerIOVType(size_t iov_type, const std::string& iov_name);

      /// Register IOV with type and key
      ConditionsPool* registerIOV(const IOVType& typ, IOV::Key key);

      /// Register new condition with the conditions store. Unlocked version, not multi-threaded
      bool registerUnlocked(ConditionsPool* pool, Condition cond);

      /// Clean conditions, which are above the age limit.
      void clean(const IOVType* typ, int max_age);

      /// Full cleanup of all managed conditions.
      void clear();

      /// Prepare all updates to the clients with the defined IOV
      long prepare(const IOV& required_validity, dd4hep_ptr<UserPool>& user_pool);

      /// Prepare all updates to the clients with the defined IOV
      long prepare(const IOV&            required_validity,
                   dd4hep_ptr<UserPool>& user_pool,
                   const Dependencies&   dependencies,
                   bool                  verify_dependencies=true);
    };

  }       /* End namespace Geometry                 */
}         /* End namespace DD4hep                   */
#endif    /* DDCOND_CONDITIONSMANAGER_H             */
