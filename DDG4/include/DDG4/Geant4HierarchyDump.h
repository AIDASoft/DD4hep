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

#ifndef DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H
#define DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H

// Geant 4 include files
#include "G4VPhysicalVolume.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
  }

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Helper to dump Geant4 volume hierarchy
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HierarchyDump {
    public:
      typedef Geometry::LCDD LCDD;
      LCDD& m_lcdd;

    public:
      /// Initializing Constructor
      Geant4HierarchyDump(LCDD& lcdd);
      /// Standard destructor
      virtual ~Geant4HierarchyDump();
      /// Dump the volume hierarchy as it is known to geant 4
      virtual void dump(const std::string& indent, const G4VPhysicalVolume* vol) const;
    };
  }
}

#endif  // DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H
