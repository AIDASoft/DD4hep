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
#ifndef DD4HEP_WORLD_H
#define DD4HEP_WORLD_H

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class WorldObject;

    /// Handle class to hold the information of the top DetElement object 'world'
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    class World : public Handle<WorldObject>  {
    public:
      /// Internal object type
      typedef WorldObject                  Object;
      /// Definition of the base handle type
      typedef Handle<Object>               RefObject;
      /// Conditions stuff
      typedef Conditions::ConditionsLoader ConditionsLoader;
      /// Alignment stuff
      typedef Alignments::AlignmentsLoader AlignmentsLoader;

    public:
      /// Default constructor
      World() : RefObject() {      }

      /// Copy from named handle
      World(const RefObject& o) : RefObject(o) {}

      /// Copy from handle
      World(const World& w) : RefObject(w) {}

      /// Copy from pointer
      World(Object* p) : RefObject(p) {}

      /// Templated constructor for handle conversions
      template <typename Q> World(const Handle<Q>& e) : RefObject(e) {}

      /// Assignment operator
      World& operator=(const World& sd) {
        m_element = sd.m_element;
        return *this;
      }
#ifndef __CINT__
      LCDD& lcdd() const;
#endif
      /// Access the conditions loading
      ConditionsLoader& conditionsLoader() const;
      /// Access to the alignment loading
      AlignmentsLoader& alignmentsLoader() const;
    };

  } /* End namespace Conditions      */
} /* End namespace DD4hep            */

#endif    /* DD4HEP_WORLD_H          */
