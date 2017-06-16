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
#ifndef DD4HEP_WORLD_H
#define DD4HEP_WORLD_H

// Framework include files
#include "DD4hep/DetElement.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

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
    /// Default constructor
    World() = default;
    /// Copy from handle
    World(const World& w) = default;
    /// Copy from named handle
    World(const Handle<WorldObject>& o) : Handle<WorldObject>(o) {}
    /// Copy from pointer
    World(Object* p) : Handle<WorldObject>(p) {}
    /// Templated constructor for handle conversions
    template <typename Q> World(const Handle<Q>& e) : Handle<WorldObject>(e) {}

    /// Assignment operator
    World& operator=(const World& sd)  = default;
#ifndef __CINT__
    Detector& detectorDescription() const;
#endif
  };
} /* End namespace dd4hep            */
#endif    /* DD4HEP_WORLD_H          */
