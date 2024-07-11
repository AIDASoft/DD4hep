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
#include <DD4hep/CylindricalGridPhiZ.h>
#include <DDSegmentation/CylindricalGridPhiZ.h>

using namespace dd4hep;

/// determine the position based on the cell ID
Position CylindricalGridPhiZ::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CylindricalGridPhiZ::cellID(const Position& local,
                               const Position& global,
                               const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in phi
double CylindricalGridPhiZ::gridSizePhi() const {
  return access()->implementation->gridSizePhi();
}

/// access the grid size in Z
double CylindricalGridPhiZ::gridSizeZ() const {
  return access()->implementation->gridSizeZ();
}

/// set the grid size in phi
void CylindricalGridPhiZ::setGridSizePhi(double cellSize) const   {
  access()->implementation->setGridSizePhi(cellSize);
}

/// set the grid size in Z
void CylindricalGridPhiZ::setGridSizeZ(double cellSize) const   {
  access()->implementation->setGridSizeZ(cellSize);
}

/// access the coordinate offset in phi
double CylindricalGridPhiZ::offsetPhi() const {
  return access()->implementation->offsetPhi();
}

/// access the coordinate offset in Z
double CylindricalGridPhiZ::offsetZ() const {
  return access()->implementation->offsetZ();
}

/// set the coordinate offset in phi
void CylindricalGridPhiZ::setOffsetPhi(double offset) const   {
  access()->implementation->setOffsetPhi(offset);
}

/// set the coordinate offset in Z
void CylindricalGridPhiZ::setOffsetZ(double offset) const   {
  access()->implementation->setOffsetZ(offset);
}

/// access the field name used for phi
const std::string& CylindricalGridPhiZ::fieldNamePhi() const {
  return access()->implementation->fieldNamePhi();
}

/// access the field name used for Z
const std::string& CylindricalGridPhiZ::fieldNameZ() const {
  return access()->implementation->fieldNameZ();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in z
*/
std::vector<double> CylindricalGridPhiZ::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
