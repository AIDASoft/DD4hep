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
  auto& dim = m_dimensions;
  G4Transform3D start = g4Transform(m_params.start);
  G4Transform3D tr(g4Transform(m_params.trafo1D.first));
  m_start = {tr, 0UL};
  m_have_rotation = false;
  dim.emplace_back(Dimension(tr, m_params.trafo1D.second));
  m_have_rotation |= dim.back().delta.getRotation().isIdentity();
  if ( m_params.trafo2D.second > 0 )   {
    tr = g4Transform(m_params.trafo2D.first);
    dim.emplace_back(Dimension(tr, m_params.trafo2D.second));
    m_have_rotation |= dim.back().delta.getRotation().isIdentity();
  }
  if ( m_params.trafo3D.second > 0 )   {
    tr = g4Transform(m_params.trafo3D.first);
    dim.emplace_back(Dimension(tr, m_params.trafo3D.second));
    m_have_rotation |= dim.back().delta.getRotation().isIdentity();
  }
  if ( m_have_rotation )    {
    auto callback = std::bind(&Geant4PlacementParameterisation::operator(),
			      *this, std::placeholders::_1);
    if ( dim.size() == 1 )
      generate_placements(start,
			  dim[0].delta, dim[0].count, callback);
    else if ( dim.size() == 2 )
      generate_placements(start,
			  dim[0].delta, dim[0].count,
			  dim[1].delta, dim[1].count, callback);
    else if ( dim.size() == 3 )
      generate_placements(start,
			  dim[0].delta, dim[0].count,
			  dim[1].delta, dim[1].count,
			  dim[2].delta, dim[2].count, callback);
  }
}

/// Access number of replicas
std::size_t dd4hep::sim::Geant4PlacementParameterisation::count()  const   {
  std::size_t ncell = 1;
  for(const auto& d : m_dimensions) ncell *= d.count;
  return ncell;
}

/// Callback to store resulting rotation
void dd4hep::sim::Geant4PlacementParameterisation::operator()(const G4Transform3D& transform)   {
  m_translations.emplace_back(transform.getTranslation());
  if ( this->m_have_rotation )   {
    G4RotationMatrix rot = transform.inverse().getRotation();
    m_rotations.emplace_back(rot);
  }
}

/// G4VPVParameterisation overload: Callback to place sub-volumes
void dd4hep::sim::Geant4PlacementParameterisation::ComputeTransformation(const G4int copy, G4VPhysicalVolume *pv) const  {
  const auto& dim = m_dimensions;
  std::size_t nd = dim.size();
  if ( !m_have_rotation )    {
    G4ThreeVector tra = m_start.translation;
    if ( nd >= 1 )   {
      std::size_t dim1 = nd == 1 ? copy : (nd == 2 ? copy%dim[1].count : (nd == 3 ? copy%(dim[1].count*dim[2].count) : 0));
      tra = dim[0].translation * dim1;
    }
    if ( nd >= 2 )   {
      std::size_t dim2 = nd == 3 ? copy%dim[2].count / dim[0].count : (nd==2 ? copy / dim[0].count : 0);
      tra = tra + dim[1].translation * dim2;
    }
    if ( nd >= 3 )   {
      std::size_t dim3 = nd == 3 ? copy / (dim[0].count*dim[1].count) : 0;
      tra = tra + dim[2].translation * dim3;
    }
    pv->SetTranslation(tra);
    return;
  }
  G4RotationMatrix& rot = m_rotations.at(copy);
  pv->SetTranslation(m_translations.at(copy));
  pv->SetRotation(&rot);
}
