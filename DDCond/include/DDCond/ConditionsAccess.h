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
#ifndef DD4HEP_DDCOND_CONDITIONSACCESS_H
#define DD4HEP_DDCOND_CONDITIONSACCESS_H

// Framework include files
#include "DD4hep/Conditions.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class ConditionsManagerObject;

    /// Manager class for condition handles
    /**
     *  Facade of the generic ConditionsManagerObject implementation
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsAccess : public Handle<ConditionsManagerObject> {
    public:

      /// Standard object type
      typedef ConditionsManagerObject Object;

    public:

      /// Default constructor
      ConditionsAccess() : Handle<Object>() {}

      /// Constructor to be used with valid pointer to object
      ConditionsAccess(Object* p) : Handle<Object>(p) {}

      /// Constructor to assing handle of the same type
      ConditionsAccess(const ConditionsAccess& c) : Handle<Object>(c) {}

      /// Constructor to be used assigning from different type
      template <typename Q> ConditionsAccess(const Handle<Q>& e)
        : Handle<Object>(e) {
      }

      /// Default destructor
      ~ConditionsAccess();

      /// Access the used/registered IOV types
      const std::vector<const IOVType*> iovTypesUsed() const;

      /// Access IOV by its name
      const IOVType* iovType (const std::string& iov_name) const;

      /// Clean conditions, which are above the age limit.
      void clean(const IOVType* typ, int max_age);

      /// Retrieve a condition given a Detector Element and the conditions name
      Condition get(Condition::key_type key, const Condition::iov_type& req_validity);
      /// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
      Condition get(const std::string& path, const Condition::iov_type& req_validity);
      /// Retrieve a condition given a Detector Element and the conditions name
      RangeConditions getRange(Condition::key_type key, const Condition::iov_type& req_validity);
      /// Retrieve a condition given the conditions path = <Detector Element path>.<conditions name>
      RangeConditions getRange(const std::string& path, const Condition::iov_type& req_validity);
    };

  }       /* End namespace Geometry                 */
}         /* End namespace DD4hep                   */
#endif    /* DD4HEP_DDCOND_CONDITIONSACCESS_H       */
