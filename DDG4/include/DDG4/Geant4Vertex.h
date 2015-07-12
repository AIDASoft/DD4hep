// $Id: $
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
#ifndef DD4HEP_GEANT4VERTEX_H
#define DD4HEP_GEANT4VERTEX_H

// Framework include files
#include "DD4hep/Memory.h"

// C/C++ include files
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Vertex;

    /// Base class to extend the basic vertex class used by DDG4 with user information
    class VertexExtension  {
    public:
      /// Default constructor
      VertexExtension() {}
      /// Default destructor
      virtual ~VertexExtension();
    };

    /// Data structure to store the MC vertex information
    /**
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Vertex {
    public:
      typedef std::set<int> Particles;
      /// Reference counter
      int ref;            //! not persistent
      /// Vertex mask to associate particles from collision
      int mask;
      /// Vertex data
      double x,y,z,time;
      /// The list of outgoing particles
      Particles out;
      /// The list of incoming particles
      Particles in;
      /// User data extension if required
      dd4hep_ptr<VertexExtension> extension;

      /// Default constructor
      Geant4Vertex();
      /// Copy constructor
      Geant4Vertex(const Geant4Vertex& c);
      /// Default destructor
      virtual ~Geant4Vertex();
      /// Assignment operator
      Geant4Vertex& operator=(const Geant4Vertex& c);
      /// Increase reference count
      Geant4Vertex* addRef();
      /// Decrease reference count. Deletes object if NULL
      void release();
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_GEANT4VERTEX_H
