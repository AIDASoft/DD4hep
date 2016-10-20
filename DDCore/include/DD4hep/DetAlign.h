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
#ifndef DD4HEP_DETALIGNMENTS_H
#define DD4HEP_DETALIGNMENTS_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Alignments.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    // Forward declarations
    class DetElementObject;
  } /* End namespace Geometry      */


  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {

    /// Handle class describing the access to DetElement dependent alignments.
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class DetAlign: public Handle<Geometry::DetElementObject>  {
    public:
      /// Internal object type
      typedef Geometry::DetElementObject Object;
      /// Definition of the base handle type
      typedef Handle<Object> RefObject;
      /// Pool definition
      typedef Conditions::UserPool UserPool;

    public:

      /// Default constructor
      DetAlign() : RefObject() {  }

      /// Constructor to hold handled object
      DetAlign(Object* object_ptr) : RefObject(object_ptr) { }

      /// Templated constructor for handle conversions
      template <typename Q> DetAlign(const Handle<Q>& e) : RefObject(e) {}

      /// Constructor to copy handle
      DetAlign(const DetAlign& e) : RefObject(e) { }

      /// Constructor to copy handle
      DetAlign(const Geometry::DetElement& e) : RefObject(e) { }

#ifdef __MAKECINT__
      /// Constructor to copy handle
      DetAlign(const Ref_t& e)
        : RefObject(e) {
      }
#endif
      /// Additional data accessor
      Object& _data() const {
        return object<Object>();
      }
      /// Assignment operator
      DetAlign& operator=(const DetAlign& e) {
        m_element = e.m_element;
        return *this;
      }
      /// Access to the constant nominal alignment information
      Alignment nominal() const;
      /// Access to the constant survey alignment information
      Alignment survey() const;
      /// Check if alignments are at all present
      bool hasAlignments()  const;
      /// Access to the alignments information
      Container alignments() const;
      /// Access to alignment objects from a given pool
      Alignment get(const std::string& key,  const UserPool& pool);
      /// Access to alignment objects from a given pool
      Alignment get(Alignment::key_type key, const UserPool& pool);
      /// Access to alignment objects. Only alignments in the pool are accessed.
      Alignment get(const std::string& key,  const Alignment::iov_type& iov);
      /// Access to alignment objects. Only alignments in the pool are accessed.
      Alignment get(Alignment::key_type key, const Alignment::iov_type& iov);
    };

  } /* End namespace Geometry      */
} /* End namespace DD4hep        */
#endif    /* DD4HEP_DETALIGNMENTS_H      */
