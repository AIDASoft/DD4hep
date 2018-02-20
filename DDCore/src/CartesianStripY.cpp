//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank, D.Blyth
//
//==========================================================================

// Framework include files
#include "DD4hep/CartesianStripY.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianStripY.h"

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianStripY::position(const CellID& id) const {
    return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianStripY::cellID(const Position& local, const Position& global,
                                       const VolumeID& volID) const {
    return access()->implementation->cellID(local, global, volID);
}

/// access the strip size in Y
double CartesianStripY::stripSizeY() const { return access()->implementation->stripSizeY(); }

/// set the strip size in Y
void CartesianStripY::setStripSizeY(double cellSize) const {
    access()->implementation->setStripSizeY(cellSize);
}

/// access the coordinate offset in Y
double CartesianStripY::offsetY() const { return access()->implementation->offsetY(); }

/// set the coordinate offset in Y
void CartesianStripY::setOffsetY(double offset) const { access()->implementation->setOffsetY(offset); }

/// access the field name used for Y
const string& CartesianStripY::fieldNameY() const { return access()->implementation->fieldNameY(); }

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
*/
vector<double> CartesianStripY::cellDimensions(const CellID& id) const {
    return access()->implementation->cellDimensions(id);
}
