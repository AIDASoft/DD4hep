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
#include <DD4hep/Volumes.h>
#include <DD4hep/Printout.h>
#include <DDG4/Geant4Helpers.h>
#include <DDG4/Geant4PlacementParameterisation.h>

// Geant4 include files
#include <G4Transform3D.hh>

/// Initializing constructor
dd4hep::sim::Geant4PlacementParameterisation::Geant4PlacementParameterisation(PlacedVolume pv)
  : G4VPVParameterisation(), m_placement(pv), m_params(*pv.data()->params)
{
  G4Transform3D tr;
  auto& dim = m_dimensions;
  g4Transform(m_params.start, m_start.delta);
  m_start.translation = m_start.delta.getTranslation();
  
  g4Transform(m_params.trafo1D.first, tr);
  dim.emplace_back(Dimension(tr, m_params.trafo1D.second));

  m_have_rotation  = false;
  m_have_rotation |= !dim.back().delta.getRotation().isIdentity();
  m_num_cells      = m_params.trafo1D.second;
  if ( m_params.trafo2D.second > 0 )   {
    g4Transform(m_params.trafo2D.first, tr);
    dim.emplace_back(Dimension(tr, m_params.trafo2D.second));
    m_have_rotation |= !dim.back().delta.getRotation().isIdentity();
    m_num_cells     *= m_params.trafo2D.second;
  }
  if ( m_params.trafo3D.second > 0 )   {
    g4Transform(m_params.trafo3D.first, tr);
    dim.emplace_back(Dimension(tr, m_params.trafo3D.second));
    m_have_rotation |= !dim.back().delta.getRotation().isIdentity();
    m_num_cells     *= m_params.trafo3D.second;
  }
  if ( m_have_rotation )    {
    auto callback = std::bind(&Geant4PlacementParameterisation::operator(),
			      this, std::placeholders::_1);
    if ( dim.size() == 1 )
      generate_placements(m_start.delta,
			  dim[0].delta, dim[0].count, callback);
    else if ( dim.size() == 2 )
      generate_placements(m_start.delta,
			  dim[0].delta, dim[0].count,
			  dim[1].delta, dim[1].count, callback);
    else if ( dim.size() == 3 )
      generate_placements(m_start.delta,
			  dim[0].delta, dim[0].count,
			  dim[1].delta, dim[1].count,
			  dim[2].delta, dim[2].count, callback);
  }
}

/// Access number of replicas
std::size_t dd4hep::sim::Geant4PlacementParameterisation::count()  const   {
  return m_num_cells;
}

/// Callback to store resulting rotation
void dd4hep::sim::Geant4PlacementParameterisation::operator()(const G4Transform3D& transform)   {
  this->m_translations.emplace_back(transform.getTranslation());
  if ( this->m_have_rotation )   {
    G4RotationMatrix rot = transform.inverse().getRotation();
    this->m_rotations.emplace_back(rot);
  }
}

/// G4VPVParameterisation overload: Callback to place sub-volumes
void dd4hep::sim::Geant4PlacementParameterisation::ComputeTransformation(const G4int copy, G4VPhysicalVolume *pv) const  {
  const auto& dim = m_dimensions;
  std::size_t nd  = dim.size();
  if ( !m_have_rotation )    {
    G4ThreeVector tra = m_start.translation;
    if ( nd >= 1 )   {
      std::size_t d1 = (nd == 1) ? copy : (nd >= 2 ? copy%dim[1].count : 0);
      tra = tra + (dim[0].translation * d1);
    }
    if ( nd >= 2 )   {
      std::size_t d2 = (nd == 2) ? copy / dim[0].count : (nd >= 3 ? copy%dim[2].count / dim[0].count : 0);
      tra = tra + (dim[1].translation * d2);
    }
    if ( nd >= 3 )   {
      std::size_t d3 = (nd == 3) ? copy / (dim[0].count*dim[1].count) : 0;
      tra = tra + (dim[2].translation * d3);
    }
    pv->SetTranslation(tra);
    return;
  }
  G4RotationMatrix& rot = m_rotations.at(copy);
  pv->SetTranslation(m_translations.at(copy));
  pv->SetRotation(&rot);
}
