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
#include <DD4hep/CartesianGridXY.h>
#include <DDSegmentation/CartesianGridXY.h>

using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianGridXY::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianGridXY::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
double CartesianGridXY::gridSizeX() const {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Y
double CartesianGridXY::gridSizeY() const {
  return access()->implementation->gridSizeY();
}

/// set the grid size in X
void CartesianGridXY::setGridSizeX(double cellSize) const   {
  access()->implementation->setGridSizeX(cellSize);
}

/// set the grid size in Y
void CartesianGridXY::setGridSizeY(double cellSize) const   {
  access()->implementation->setGridSizeY(cellSize);
}

/// access the coordinate offset in X
double CartesianGridXY::offsetX() const {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Y
double CartesianGridXY::offsetY() const {
  return access()->implementation->offsetY();
}

/// set the coordinate offset in X
void CartesianGridXY::setOffsetX(double offset) const   {
  access()->implementation->setOffsetX(offset);
}

/// set the coordinate offset in Y
void CartesianGridXY::setOffsetY(double offset) const   {
  access()->implementation->setOffsetY(offset);
}

/// access the field name used for X
const std::string& CartesianGridXY::fieldNameX() const {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Y
const std::string& CartesianGridXY::fieldNameY() const {
  return access()->implementation->fieldNameY();
}

// Returns a vector<double> of the cellDimensions of the given cell ID
std::vector<double> CartesianGridXY::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
