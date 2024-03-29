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
#include <DD4hep/WaferGridXY.h>
#include <DDSegmentation/WaferGridXY.h>

using namespace dd4hep;

/// determine the position based on the cell ID
Position WaferGridXY::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID WaferGridXY::cellID(const Position& local,
                                   const Position& global,
                                   const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in X
double WaferGridXY::gridSizeX() const {
  return access()->implementation->gridSizeX();
}

/// access the grid size in Y
double WaferGridXY::gridSizeY() const {
  return access()->implementation->gridSizeY();
}

/// access the coordinate offset in X
double WaferGridXY::offsetX() const {
  return access()->implementation->offsetX();
}

/// access the coordinate offset in Y
double WaferGridXY::offsetY() const {
  return access()->implementation->offsetY();
}
/// access the coordinate waferOffset for inGroup in X
double WaferGridXY::waferOffsetX(int inGroup, int inWafer) const  {
  return access()->implementation->waferOffsetX(inGroup,inWafer);
}

/// access the coordinate waferOffset for inGroup in Y
double WaferGridXY::waferOffsetY(int inGroup, int inWafer) const  {
  return access()->implementation->waferOffsetY(inGroup,inWafer);
}

/// access the field name used for X
const std::string& WaferGridXY::fieldNameX() const {
  return access()->implementation->fieldNameX();
}

/// access the field name used for Y
const std::string& WaferGridXY::fieldNameY() const {
  return access()->implementation->fieldNameY();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
*/
std::vector<double> WaferGridXY::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
