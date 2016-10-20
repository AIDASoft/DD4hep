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

#ifndef DD4HEP_DETCONDITIONS_H
#define DD4HEP_DETCONDITIONS_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    // Forward declarations
    class DetElementObject;
  } /* End namespace Geometry      */


  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Handle class describing the access to DetElement dependent conditions.
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class DetConditions: public Handle<Geometry::DetElementObject>  {
    public:
      /// Internal object type
      typedef Geometry::DetElementObject Object;
      /// Definition of the base handle type
      typedef Handle<Object> RefObject;

    public:

      /// Default constructor
      DetConditions() : RefObject() {  }

      /// Constructor to hold handled object
      DetConditions(Object* object_ptr) : RefObject(object_ptr) { }

      /// Templated constructor for handle conversions
      template <typename Q> DetConditions(const Handle<Q>& e) : RefObject(e) {}

      /// Constructor to copy handle
      DetConditions(const DetConditions& e) : RefObject(e) { }

      /// Constructor to copy handle
      DetConditions(const Geometry::DetElement& e) : RefObject(e) { }

#ifdef __MAKECINT__
      /// Constructor to copy handle
      DetConditions(const Ref_t& e)
        : RefObject(e) {
      }
#endif
      /// Additional data accessor
      Object& _data() const {
        return object<Object>();
      }
      /// Assignment operator
      DetConditions& operator=(const DetConditions& e) {
        m_element = e.m_element;
        return *this;
      }
      /// Check if conditions are at all present
      bool hasConditions()  const;
      /// Access to the conditions information
      Container conditions() const;
      /// Access to condition objects from a given pool
      Condition get(const std::string& key,  const UserPool& pool);
      /// Access to condition objects from a given pool
      Condition get(Condition::key_type key, const UserPool& pool);
      /// Access to condition objects. Only conditions in the pool are accessed.
      Condition get(const std::string& key,  const Condition::iov_type& iov);
      /// Access to condition objects. Only conditions in the pool are accessed.
      Condition get(Condition::key_type key, const Condition::iov_type& iov);
    };

  } /* End namespace Geometry      */
} /* End namespace DD4hep        */

#endif    /* DD4HEP_DETCONDITIONS_H      */
