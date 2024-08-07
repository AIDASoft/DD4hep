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
#include <DD4hep/CartesianGridYZ.h>
#include <DDSegmentation/CartesianGridYZ.h>

using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianGridYZ::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianGridYZ::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in Y
double CartesianGridYZ::gridSizeY() const {
  return access()->implementation->gridSizeY();
}

/// access the grid size in Z
double CartesianGridYZ::gridSizeZ() const {
  return access()->implementation->gridSizeZ();
}

/// set the grid size in Y
void CartesianGridYZ::setGridSizeY(double cellSize) const   {
  access()->implementation->setGridSizeY(cellSize);
}

/// set the grid size in Z
void CartesianGridYZ::setGridSizeZ(double cellSize) const   {
  access()->implementation->setGridSizeZ(cellSize);
}

/// access the coordinate offset in Y
double CartesianGridYZ::offsetY() const {
  return access()->implementation->offsetY();
}

/// access the coordinate offset in Z
double CartesianGridYZ::offsetZ() const {
  return access()->implementation->offsetZ();
}

/// set the coordinate offset in Y
void CartesianGridYZ::setOffsetY(double offset) const   {
  access()->implementation->setOffsetY(offset);
}

/// set the coordinate offset in Z
void CartesianGridYZ::setOffsetZ(double offset) const   {
  access()->implementation->setOffsetZ(offset);
}

/// access the field name used for Y
const std::string& CartesianGridYZ::fieldNameY() const {
  return access()->implementation->fieldNameY();
}

/// access the field name used for Z
const std::string& CartesianGridYZ::fieldNameZ() const {
  return access()->implementation->fieldNameZ();
}

// Returns a vector<double> of the cellDimensions of the given cell ID
std::vector<double> CartesianGridYZ::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
