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
#include <XML/XMLElements.h>
#include <DDG4/Factories.h>
#include <DD4hep/ComponentProperties.h>

// Geant4 include files
#include <G4SystemOfUnits.hh>
#include <G4ExtendedMaterial.hh>
#include <G4CrystalExtension.hh>
#include <G4ChannelingMaterialData.hh>
#include <G4ChannelingOptrMultiParticleChangeCrossSection.hh>

// Forward declarations
namespace {  class ChannelingCrystalMaterial;  }

/// Namespace example name of the user
namespace dd4hep   {

  /// Class member specialization to create G4ExtendedMaterial
  /** Class member specialization to create G4ExtendedMaterial
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  template <>  G4Material* 
  Geant4MaterialFactory<ChannelingCrystalMaterial>::create(dd4hep::Detector& /* description */,
							   Material    mat,
							   G4Material* base_material)
  {
    std::string crystal_data, orientation, name = base_material->GetName();
    auto* material = new G4ExtendedMaterial(name+".extended", base_material);
    material->RegisterExtension(std::unique_ptr<G4CrystalExtension>(new G4CrystalExtension(material)));
    auto* ext = (G4CrystalExtension*)material->RetrieveExtension("crystal");
    ext->SetUnitCell(new G4CrystalUnitCell(5.43 * CLHEP::angstrom,
					   5.43 * CLHEP::angstrom,
					   5.43 * CLHEP::angstrom,
					   CLHEP::halfpi,
					   CLHEP::halfpi,
					   CLHEP::halfpi,
					   227));

    material->RegisterExtension(std::unique_ptr<G4ChannelingMaterialData>(new G4ChannelingMaterialData("channeling")));
    auto* channelingData =(G4ChannelingMaterialData*)material->RetrieveExtension("channeling");
    crystal_data = mat.constPropertyRef("Geant4-ignore:crystal_data");
    if ( crystal_data.empty() )   {
      except("ChannelingCrystalMaterial",
	     "====> Extended material: %s misses channeling data file.",
	     name.c_str());
    }
    channelingData->SetFilename(crystal_data);

    /// Set crystal orientation
    orientation = mat.constPropertyRef("Geant4-ignore:crystal_orientation");
    if ( !orientation.empty() )   {
      Direction direction;
      Property  prop(direction);
      prop.str(orientation);
      channelingData->SetBR(direction.X());
    }

    printout(ALWAYS,"ChannelingCrystalMaterial",
	     "====> Created extended material: %s Data: %s Orientation: %s",
	     name.c_str(), crystal_data.c_str(), orientation.c_str());
    return material;
  }
}      // End namespace dd4hep
DECLARE_GEANT4EXTENDEDMATERIAL(ChannelingCrystalMaterial)
