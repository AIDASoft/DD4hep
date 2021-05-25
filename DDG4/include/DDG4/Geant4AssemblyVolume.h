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

/// ROOT includes
#include "TGeoNode.h"

/// Geant4 include files
#include "G4AssemblyVolume.hh"

/// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Forward declarations
    class Geant4Converter;
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
      std::vector<Geant4AssemblyVolume*> m_places;
      G4AssemblyVolume*            m_assembly;

    public:
      /// Default constructor with initialization
      Geant4AssemblyVolume();
      /// Inhibit move construction
      Geant4AssemblyVolume(Geant4AssemblyVolume&& copy) = delete;
      /// Inhibit copy construction
      Geant4AssemblyVolume(const Geant4AssemblyVolume& copy) = delete;
      /// Inhibit move assignment
      Geant4AssemblyVolume& operator=(Geant4AssemblyVolume&& copy) = delete;
      /// Inhibit copy assignment
      Geant4AssemblyVolume& operator=(const Geant4AssemblyVolume& copy) = delete;
      /// Default destructor
      virtual ~Geant4AssemblyVolume();
      /// Place logical daughter volume into the assembly
      long placeVolume(const TGeoNode* n, G4LogicalVolume* pPlacedVolume, G4Transform3D& transformation);
      /// Place daughter assembly into the assembly      
      long placeAssembly(const TGeoNode* n, Geant4AssemblyVolume* pPlacedVolume, G4Transform3D& transformation);
      /// Expand all daughter placements and expand the contained assemblies to imprints
      void imprint(const Geant4Converter& cnv,
                   const TGeoNode*        n,
                   Chain                  chain,
                   Geant4AssemblyVolume*  pAssembly,
                   G4LogicalVolume*       pMotherLV,
                   G4Transform3D&         transformation,
                   G4int                  copyNumBase,
                   G4bool                 surfCheck );
    };
  }
}
#endif
