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
#ifndef DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H
#define DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H

// Geant 4 include files
#include "G4VPhysicalVolume.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Helper to dump Geant4 volume hierarchy
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4HierarchyDump {
    public:
      Detector& m_detDesc;

    public:
      /// Initializing Constructor
      Geant4HierarchyDump(Detector& description);
      /// Standard destructor
      virtual ~Geant4HierarchyDump();
      /// Dump the volume hierarchy as it is known to geant 4
      virtual void dump(const std::string& indent, const G4VPhysicalVolume* vol) const;
    };
  }
}

#endif  // DD4HEP_DDG4_GEANT4HIERARCHYDUMP_H
