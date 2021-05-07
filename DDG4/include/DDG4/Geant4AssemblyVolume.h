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
#ifndef DDG4_GEANT4ASSEMBLYVOLUME_H
#define DDG4_GEANT4ASSEMBLYVOLUME_H

// ROOT includes
#include "TGeoNode.h"

// Geant4 include files
#include "G4AssemblyVolume.hh"

/// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Forward declarations
    class Geant4GeometryInfo;
    
    /// Hack! Wrapper around G4AssemblyVolume to access protected members.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4AssemblyVolume  {
      
    public:

      typedef std::vector<const TGeoNode*> Chain;
      std::vector<const TGeoNode*> m_entries;
      G4AssemblyVolume*            m_assembly;

      /// Default constructor with initialization
      Geant4AssemblyVolume();
      
      /// Default destructor
      virtual ~Geant4AssemblyVolume();
      
      long placeVolume(const TGeoNode* n, G4LogicalVolume* pPlacedVolume, G4Transform3D& transformation);
      
      long placeAssembly(const TGeoNode* n, Geant4AssemblyVolume* pPlacedVolume, G4Transform3D& transformation);
      
      void imprint(Geant4GeometryInfo& info,
                   const TGeoNode* n,
                   Chain chain,
                   Geant4AssemblyVolume* pAssembly,
                   G4LogicalVolume*  pMotherLV,
                   G4Transform3D&    transformation,
                   G4int copyNumBase,
                   G4bool surfCheck );
    };
  }
}

#endif
