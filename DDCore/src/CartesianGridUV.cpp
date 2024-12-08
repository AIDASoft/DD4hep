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
#include <DD4hep/CartesianGridUV.h>
#include <DDSegmentation/CartesianGridUV.h>

using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianGridUV::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianGridUV::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in U
double CartesianGridUV::gridSizeU() const {
  return access()->implementation->gridSizeU();
}

/// access the grid size in V
double CartesianGridUV::gridSizeV() const {
  return access()->implementation->gridSizeV();
}

/// set the grid size in U
void CartesianGridUV::setGridSizeU(double cellSize) const   {
  access()->implementation->setGridSizeU(cellSize);
}

/// set the grid size in V
void CartesianGridUV::setGridSizeV(double cellSize) const   {
  access()->implementation->setGridSizeV(cellSize);
}

/// access the coordinate offset in U
double CartesianGridUV::offsetU() const {
  return access()->implementation->offsetU();
}

/// access the coordinate offset in V
double CartesianGridUV::offsetV() const {
  return access()->implementation->offsetV();
}

/// access the rotation angle
double CartesianGridUV::gridAngle() const {
  return access()->implementation->gridAngle();
}

/// set the coordinate offset in U
void CartesianGridUV::setOffsetU(double offset) const   {
  access()->implementation->setOffsetU(offset);
}

/// set the coordinate offset in V
void CartesianGridUV::setOffsetV(double offset) const   {
  access()->implementation->setOffsetV(offset);
}

/// access the field name used for U
const std::string& CartesianGridUV::fieldNameU() const {
  return access()->implementation->fieldNameU();
}

/// access the field name used for V
const std::string& CartesianGridUV::fieldNameV() const {
  return access()->implementation->fieldNameV();
}

// Returns a vector<double> of the cellDimensions of the given cell ID
std::vector<double> CartesianGridUV::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
