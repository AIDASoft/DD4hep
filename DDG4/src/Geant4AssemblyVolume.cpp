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

// Geant4 include files
#include "DD4hep/DetectorTools.h"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ReflectionFactory.hh"

// C/C++ include files
#include <sstream>
#include <string>

// Framework include files
#include "DDG4/Geant4GeometryInfo.h"
#include "DDG4/Geant4AssemblyVolume.h"

using namespace dd4hep::sim;

void Geant4AssemblyVolume::imprint(Geant4GeometryInfo&   info,
                                   const TGeoNode*       parent,
                                   Chain                 chain,
                                   Geant4AssemblyVolume* pParentAssembly,
                                   G4LogicalVolume*      pMotherLV,
                                   G4Transform3D&        transformation,
                                   G4int                 copyNumBase,
                                   G4bool                surfCheck)
{
  static int level=0;
  TGeoVolume* vol = parent->GetVolume();
  unsigned int numberOfDaughters = (copyNumBase == 0) ? pMotherLV->GetNoDaughters() : copyNumBase;

  ++level;

  // We start from the first available index
  //
  numberOfDaughters++;
  ImprintsCountPlus();

  std::vector<G4AssemblyTriplet> triplets = pParentAssembly->fTriplets;
  //cout << " Assembly:" << detail::tools::placementPath(chain) << endl;

  for( unsigned int i = 0; i < triplets.size(); i++ )  {
    const TGeoNode* node = pParentAssembly->m_entries[i];
    Chain new_chain = chain;
    new_chain.emplace_back(node);
    //cout << " Assembly: Entry: " << detail::tools::placementPath(new_chain) << endl;

    G4Transform3D Ta( *(triplets[i].GetRotation()), triplets[i].GetTranslation() );
    if ( triplets[i].IsReflection() )  {
      Ta = Ta * G4ReflectZ3D();
    }
    G4Transform3D Tfinal = transformation * Ta;
    if ( triplets[i].GetVolume() )    {
      // Generate the unique name for the next PV instance
      // The name has format:
      //
      // av_WWW_impr_XXX_YYY_ZZZ
      // where the fields mean:
      // WWW - assembly volume instance number
      // XXX - assembly volume imprint number
      // YYY - the name of a log. volume we want to make a placement of
      // ZZZ - the log. volume index inside the assembly volume
      //
      std::stringstream pvName;
#if 0
      pvName << "av_"
             << GetAssemblyID()
             << "_impr_"
             << GetImprintsCount()
             << "_"
             << triplets[i].GetVolume()->GetName().c_str()
             << "_pv_"
             << i
             << ends;
#endif
      pvName << "AV_"
             << GetAssemblyID()
             << '#'
	     << parent->GetName()
	     << ':'
	     << parent->GetNumber()
             << '#'
             << m_entries[i]->GetName()
             << ':'
             << m_entries[i]->GetNumber()
             << std::ends;
      // Generate a new physical volume instance inside a mother
      // (as we allow 3D transformation use G4ReflectionFactory to
      //  take into account eventual reflection)
      //
#if 0
      printout(INFO,"Geant4Converter","++ Place %svolume %s in assembly.",
	       triplets[i].IsReflection() ? "REFLECTED " : "",
	       detail::tools::placementPath(new_chain).c_str());
#endif
      G4PhysicalVolumesPair pvPlaced
        = G4ReflectionFactory::Instance()->Place( Tfinal,
                                                  pvName.str().c_str(),
                                                  triplets[i].GetVolume(),
                                                  pMotherLV,
                                                  false,
                                                  numberOfDaughters + i,
                                                  surfCheck );

      // Register the physical volume created by us so we can delete it later
      //
      //fPVStore.emplace_back( pvPlaced.first );
      info.g4VolumeImprints[vol].emplace_back(new_chain,pvPlaced.first);
#if 0
      cout << " Assembly:Parent:" << parent->GetName() << " " << node->GetName()
           << " " <<  (void*)node << " G4:" << pvName.str() << " Daughter:"
           << detail::tools::placementPath(new_chain) << endl;
      cout << endl;
#endif

      if ( pvPlaced.second )  {
        G4Exception("Geant4AssemblyVolume::imprint(..)", "GeomVol0003", FatalException,
                    "Fancy construct popping new mother from the stack!");
        //fPVStore.emplace_back( pvPlaced.second );
      }
    }
    else if ( triplets[i].GetAssembly() )  {
      // Place volumes in this assembly with composed transformation
      imprint(info, parent, new_chain, (Geant4AssemblyVolume*)triplets[i].GetAssembly(),
              pMotherLV, Tfinal, i*100+copyNumBase, surfCheck );
    }
    else   {
      --level;
      G4Exception("Geant4AssemblyVolume::imprint(..)", "GeomVol0003", FatalException,
                  "Triplet has no volume and no assembly");
    }
  }
  //cout << "Imprinted assembly level:" << level << " in mother:" << pMotherLV->GetName() << endl;
  --level;
}
