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
#include "DD4hep/CartesianGridYZ.h"
#include "DDSegmentation/CartesianGridYZ.h"

// C/C++ include files

using namespace std;
using namespace DD4hep::Geometry;

/// Copy Constructor from segmentation base object
CartesianGridYZ::CartesianGridYZ(const Segmentation& e) : Handle<Object>()
{
  m_element = Segmentation::get<Object>(e.ptr());
}

/// determine the position based on the cell ID
Position CartesianGridYZ::position(const CellID& id) const   {
  return Position(access()->position(id));
}

/// determine the cell ID based on the position
DD4hep::CellID CartesianGridYZ::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const
{
  return access()->cellID(local, global, volID);
}

/// access the grid size in Y
double CartesianGridYZ::gridSizeY() const {
  return access()->gridSizeY();
}

/// access the grid size in Z
double CartesianGridYZ::gridSizeZ() const {
  return access()->gridSizeZ();
}

/// access the coordinate offset in Y
double CartesianGridYZ::offsetY() const {
  return access()->offsetY();
}

/// access the coordinate offset in Z
double CartesianGridYZ::offsetZ() const {
  return access()->offsetZ();
}

/// access the field name used for Y
const string& CartesianGridYZ::fieldNameY() const {
  return access()->fieldNameY();
}

/// access the field name used for Z
const string& CartesianGridYZ::fieldNameZ() const {
  return access()->fieldNameZ();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in y
    -# size in z
*/
vector<double> CartesianGridYZ::cellDimensions(const CellID& id) const  {
  return access()->cellDimensions(id);
}
