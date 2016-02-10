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
#include "DD4hep/Conditions.h"
#include "DD4hep/ComponentProperties.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class Entry;
    class ConditionsDataLoader;

    /// Conditions internal namespace
    namespace Interna  {
      class ConditionsManagerObject;
    }

    /// Manager class for condition handles
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsManager : public Handle<Interna::ConditionsManagerObject> {
    public:

      /// Standard object type
      typedef Interna::ConditionsManagerObject Object;
      typedef ConditionsDataLoader             Loader;
      typedef std::vector<IOVType*>            IOVTypes;
      typedef std::map<DetElement,Container>   DetectorConditions;
      typedef std::map<IOVType*,Container>     TypeConditions;

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
      template <typename Q> ConditionsManager(const ConditionsManager& c)
        : Handle<Object>(c) {
      }

      /// Constructor to be used assigning from different type
      template <typename Q> ConditionsManager(const Handle<Q>& e)
        : Handle<Object>(e) {
      }

      /// Default destructor
      ~ConditionsManager();

      /// Access to properties
      Property& operator[](const std::string& property_name) const;

      /// Access to the property manager
      PropertyManager& properties()  const;

      /// Access the conditions loader
      Handle<Loader> loader()  const;

      /// Access the availible/known IOV types
      const IOVTypes& iovTypes()  const;

      /// Register new IOV type if it does not (yet) exist.
      /** Returns (false,pointer) if IOV existed and
       *  (true,pointer) if new IOV was registered to the manager.
       */
      std::pair<bool, const IOVType*> registerIOVType(size_t iov_type, const std::string& iov_name);
      
      /// Access IOV by its type
      const IOVType* iovType (size_t iov_type) const;

      /// Access IOV by its name
      const IOVType* iovType (const std::string& iov_name) const;

      /// Lock the internal data structures. This disables calls to "register", etc.
      void lock();
      
      /// Unlock the internal data structures. This enables calls to "register", etc.
      void unlock();

      /// Age conditions, which are no longer used and to be removed eventually
      void age();

      /// Clean conditions, which are above the age limit.
      void clean();

      /// Full cleanup of all managed conditions.
      void clear();

      /// Push all pending updates to the conditions store. 
      /** Note:
       *  This does not yet make the new conditions availible to the clients
       */
      void pushUpdates();

      /// Prepare all updates to the clients with the defined IOV
      void prepare(const IOV& required_validity);
      
      /// Enable all updates to the clients with the defined IOV
      void enable(const IOV& required_validity);
      
      /// Retrieve a condition given a Detector Element and the conditions name
      Condition get(DetElement detector,
                    const std::string& condition_name,
                    const IOV& req_validity);

      /// Retrieve a condition given a Detector Element path and the conditions name
      Condition get(const std::string& detector_path,
                    const std::string& condition_name,
                    const IOV& req_validity);

      /// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
      Condition get(const std::string& conditions_path,
                    const IOV& req_validity);

      /// Retrieve a condition given a Detector Element and the conditions name
      RangeConditions getRange(DetElement detector,
                               const std::string& condition_name,
                               const IOV& req_range_validity);

      /// Retrieve a condition given a Detector Element path and the conditions name
      RangeConditions getRange(const std::string& detector_path, 
                               const std::string& condition_name,
                               const IOV& req_range_validity);

      /// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
      RangeConditions getRange(const std::string& conditions_path,
                               const IOV& req_range_validity);
    };

  }       /* End namespace Geometry                 */
}         /* End namespace DD4hep                   */
#endif    /* DDCOND_CONDITIONSMANAGER_H    */
