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

// Framework include files
#include <DDG4/Factories.h>
// Geant4 include files
#include <G4ExtendedMaterial.hh>
#include <G4LogicalCrystalVolume.hh>

// Forward declarations
namespace  {  class ChannelingCrystalVolume;  }

/// Namespace example name of the user
namespace dd4hep   {

  /// Class member specialization to create a G4LogicalCrystalVolume
  /** Class member specialization to create a G4LogicalCrystalVolume
   *  
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  template <> G4LogicalVolume* 
  Geant4LogicalVolumeFactory<ChannelingCrystalVolume>::create(dd4hep::Detector& /* description */,
							      Volume      volume,
							      G4VSolid*   solid,
							      G4Material* material)
  {
    G4ExtendedMaterial* mat = dynamic_cast<G4ExtendedMaterial*>(material);
    if ( !mat )   {
      except("ChannelingCrystalVolume", 
	     "====> Material %s is no G4ExtendedMaterial!", material->GetName().c_str());
    }
    auto* ptr = new G4LogicalCrystalVolume(solid, mat, volume.name());
    printout(ALWAYS,"ChannelingCrystalVolume",
	     "====> Created specialize logical volume [G4LogicalCrystalVolume]: %s",
	     volume.name());
    return ptr;
  }
}      // End namespace dd4hep

DECLARE_GEANT4LOGICALVOLUME(ChannelingCrystalVolume)
