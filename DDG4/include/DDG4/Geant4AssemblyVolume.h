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

// Disable diagnostics for ROOT dictionaries
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif

#define private public
#include "G4AssemblyVolume.hh"
#undef private

#ifdef __clang__
#pragma clang diagnostic pop
#endif

// ROOT includes
#include "TGeoNode.h"

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
    class Geant4AssemblyVolume : public G4AssemblyVolume {
      
    public:

      std::vector<const TGeoNode*> m_entries;
      typedef std::vector<const TGeoNode*> Chain;

      /// Default constructor with initialization
      Geant4AssemblyVolume() {
      }

      /// Default destructor
      ~Geant4AssemblyVolume()   {
      }

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
