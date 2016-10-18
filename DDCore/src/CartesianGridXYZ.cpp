//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Segmentations.h"
#include "DD4hep/CartesianGridXYZ.h"
#include "DDSegmentation/CartesianGridXYZ.h"

// C/C++ include files

using namespace std;
using namespace DD4hep::Geometry;

/// Copy Constructor from segmentation base object
CartesianGridXYZ::CartesianGridXYZ(const Segmentation& e) : Handle<Object>()
{
  m_element = Segmentation::get<Object>(e.ptr());
}

/// determine the position based on the cell ID
Position CartesianGridXYZ::position(const CellID& id) const   {
  return Position(access()->position(id));
}

/// determine the cell ID based on the position
DD4hep::CellID CartesianGridXYZ::cellID(const Position& local,
                                        const Position& global,
                                        const VolumeID& volID) const
{
  return access()->cellID(local, global, volID);
}

/// access the grid size in X
double CartesianGridXYZ::gridSizeX() const {
  return access()->gridSizeX();
}

/// access the grid size in Y
double CartesianGridXYZ::gridSizeY() const {
  return access()->gridSizeY();
}

/// access the grid size in Z
double CartesianGridXYZ::gridSizeZ() const {
  return access()->gridSizeZ();
}

/// access the coordinate offset in X
double CartesianGridXYZ::offsetX() const {
  return access()->offsetX();
}

/// access the coordinate offset in Y
double CartesianGridXYZ::offsetY() const {
  return access()->offsetY();
}

/// access the coordinate offset in Z
double CartesianGridXYZ::offsetZ() const {
  return access()->offsetZ();
}

/// access the field name used for X
const string& CartesianGridXYZ::fieldNameX() const {
  return access()->fieldNameX();
}

/// access the field name used for Y
const string& CartesianGridXYZ::fieldNameY() const {
  return access()->fieldNameY();
}

/// access the field name used for Z
const string& CartesianGridXYZ::fieldNameZ() const {
  return access()->fieldNameZ();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
    -# size in z
*/
vector<double> CartesianGridXYZ::cellDimensions(const CellID& id) const  {
  return access()->cellDimensions(id);
}
