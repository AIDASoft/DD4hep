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

/// Default constructor with initialization
Geant4AssemblyVolume::Geant4AssemblyVolume() {
  m_assembly = new G4AssemblyVolume();
}

      
/// Default destructor
Geant4AssemblyVolume::~Geant4AssemblyVolume()   {
  // Do not delete the G4 assembly. This is done by Geant4!
  m_assembly = nullptr;
}

long Geant4AssemblyVolume::placeVolume(const TGeoNode* n,
				       G4LogicalVolume* pPlacedVolume,
				       G4Transform3D& transformation)
{
  size_t id = m_assembly->TotalTriplets();
  m_entries.emplace_back(n);
  m_places.emplace_back(nullptr);
  m_assembly->AddPlacedVolume(pPlacedVolume, transformation);
  return (long)id;
}

long Geant4AssemblyVolume::placeAssembly(const TGeoNode* n,
					 Geant4AssemblyVolume* pPlacedVolume,
					 G4Transform3D& transformation)
{
  size_t id = m_assembly->TotalTriplets();
  m_entries.emplace_back(n);
  m_places.emplace_back(pPlacedVolume);
  m_assembly->AddPlacedAssembly(pPlacedVolume->m_assembly, transformation);
  return (long)id;
}

void Geant4AssemblyVolume::imprint(Geant4GeometryInfo&   info,
                                   const TGeoNode*       parent,
                                   Chain                 chain,
                                   Geant4AssemblyVolume* pParentAssembly,
                                   G4LogicalVolume*      pMotherLV,
                                   G4Transform3D&        transformation,
                                   G4int                 copyNumBase,
                                   G4bool                surfCheck)
{
  struct _Wrap : public G4AssemblyVolume  {
    static void imprintsCountPlus(G4AssemblyVolume* p)
    {  _Wrap* w = (_Wrap*)p; w->ImprintsCountPlus(); }
  };
  TGeoVolume* vol = parent->GetVolume();
  G4AssemblyVolume* par_ass = pParentAssembly->m_assembly;
  unsigned int numberOfDaughters = (copyNumBase == 0) ? pMotherLV->GetNoDaughters() : copyNumBase;

  // We start from the first available index
  numberOfDaughters++;
  _Wrap::imprintsCountPlus(par_ass);

  //cout << " Assembly:" << detail::tools::placementPath(chain) << endl;
  std::vector<G4AssemblyTriplet>::iterator iter = par_ass->GetTripletsIterator();
  for( unsigned int i = 0, n = par_ass->TotalTriplets(); i < n; i++, iter++ )  {
    Chain new_chain = chain;
    const auto& triplet = *iter;
    const TGeoNode* node = pParentAssembly->m_entries[i];
    Geant4AssemblyVolume* avol = pParentAssembly->m_places[i];

    new_chain.emplace_back(node);
    //cout << " Assembly: Entry: " << detail::tools::placementPath(new_chain) << endl;

    G4Transform3D Ta( *(triplet.GetRotation()), triplet.GetTranslation() );
    if ( triplet.IsReflection() )  {
      Ta = Ta * G4ReflectZ3D();
    }
    G4Transform3D Tfinal = transformation * Ta;
    if ( triplet.GetVolume() )    {
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
      pvName << "AV_"
             << m_assembly->GetAssemblyID()
             << '!'
	     << parent->GetName()
	     << '#'
	     << parent->GetNumber()
             << '!'
             << node->GetName()
             << '#'
             << node->GetNumber()
             << std::ends;
      // Generate a new physical volume instance inside a mother
      // (as we allow 3D transformation use G4ReflectionFactory to
      //  take into account eventual reflection)
      //
      G4PhysicalVolumesPair pvPlaced
        = G4ReflectionFactory::Instance()->Place( Tfinal,
                                                  pvName.str().c_str(),
                                                  triplet.GetVolume(),
                                                  pMotherLV,
                                                  false,
                                                  numberOfDaughters + i,
                                                  surfCheck );

      // Register the physical volume created by us so we can delete it later
      //
      //fPVStore.emplace_back( pvPlaced.first );
      info.g4VolumeImprints[vol].emplace_back(new_chain,pvPlaced.first);
#if 0
      printout(INFO,"Geant4Converter","++ Place %svolume %s in assembly.",
	       triplet.IsReflection() ? "REFLECTED " : "",
	       detail::tools::placementPath(new_chain).c_str());
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
    else if ( triplet.GetAssembly() )  {
      // Place volumes in this assembly with composed transformation
      imprint(info, parent, new_chain, avol, pMotherLV, Tfinal, i*100+copyNumBase, surfCheck );
    }
    else   {
      G4Exception("Geant4AssemblyVolume::imprint(..)", "GeomVol0003", FatalException,
                  "Triplet has no volume and no assembly");
    }
  }
}
