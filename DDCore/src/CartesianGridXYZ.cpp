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
#include <DD4hep/CartesianGridXYZ.h>
#include <DDSegmentation/CartesianGridXYZ.h>

using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianGridXYZ::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianGridXYZ::cellID(const Position& local,
                                        const Position& global,
                                        const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
double CartesianGridXYZ::gridSizeX() const {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Y
double CartesianGridXYZ::gridSizeY() const {
  return access()->implementation->gridSizeY();
}

/// access the grid size in Z
double CartesianGridXYZ::gridSizeZ() const {
  return access()->implementation->gridSizeZ();
}

/// set the grid size in X
void CartesianGridXYZ::setGridSizeX(double cellSize) const   {
  access()->implementation->setGridSizeX(cellSize);
}

/// set the grid size in Y
void CartesianGridXYZ::setGridSizeY(double cellSize) const   {
  access()->implementation->setGridSizeY(cellSize);
}

/// set the grid size in Z
void CartesianGridXYZ::setGridSizeZ(double cellSize) const   {
  access()->implementation->setGridSizeZ(cellSize);
}

/// access the coordinate offset in X
double CartesianGridXYZ::offsetX() const {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Y
double CartesianGridXYZ::offsetY() const {
  return access()->implementation->offsetY();
}

/// access the coordinate offset in Z
double CartesianGridXYZ::offsetZ() const {
  return access()->implementation->offsetZ();
}

/// set the coordinate offset in X
void CartesianGridXYZ::setOffsetX(double offset) const   {
  access()->implementation->setOffsetX(offset);
}

/// set the coordinate offset in Y
void CartesianGridXYZ::setOffsetY(double offset) const   {
  access()->implementation->setOffsetY(offset);
}

/// set the coordinate offset in Z
void CartesianGridXYZ::setOffsetZ(double offset) const   {
  access()->implementation->setOffsetZ(offset);
}

/// access the field name used for X
const std::string& CartesianGridXYZ::fieldNameX() const {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Y
const std::string& CartesianGridXYZ::fieldNameY() const {
  return access()->implementation->fieldNameY();
}

/// access the field name used for Z
const std::string& CartesianGridXYZ::fieldNameZ() const {
  return access()->implementation->fieldNameZ();
}

// Returns a vector<double> of the cellDimensions of the given cell ID
std::vector<double> CartesianGridXYZ::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
