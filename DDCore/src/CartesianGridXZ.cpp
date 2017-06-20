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
#include "DD4hep/CartesianGridXZ.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianGridXZ.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianGridXZ::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianGridXZ::cellID(const Position& local,
                               const Position& global,
                               const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
double CartesianGridXZ::gridSizeX() const {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Z
double CartesianGridXZ::gridSizeZ() const {
  return access()->implementation->gridSizeZ();
}

/// set the grid size in X
void CartesianGridXZ::setGridSizeX(double cellSize) const   {
  access()->implementation->setGridSizeX(cellSize);
}

/// set the grid size in Z
void CartesianGridXZ::setGridSizeZ(double cellSize) const   {
  access()->implementation->setGridSizeZ(cellSize);
}

/// access the coordinate offset in X
double CartesianGridXZ::offsetX() const {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Z
double CartesianGridXZ::offsetZ() const {
  return access()->implementation->offsetZ();
}

/// set the coordinate offset in X
void CartesianGridXZ::setOffsetX(double offset) const   {
  access()->implementation->setOffsetX(offset);
}

/// set the coordinate offset in Z
void CartesianGridXZ::setOffsetZ(double offset) const   {
  access()->implementation->setOffsetZ(offset);
}

/// access the field name used for X
const string& CartesianGridXZ::fieldNameX() const {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Z
const string& CartesianGridXZ::fieldNameZ() const {
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
vector<double> CartesianGridXZ::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
