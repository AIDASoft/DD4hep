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
#include "DD4hep/CartesianStripX.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/CartesianStripX.h"

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
Position CartesianStripX::position(const CellID& id) const {
    return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID CartesianStripX::cellID(const Position& local, const Position& global,
                                       const VolumeID& volID) const {
    return access()->implementation->cellID(local, global, volID);
}

/// access the strip size in X
double CartesianStripX::stripSizeX() const { return access()->implementation->stripSizeX(); }

/// set the strip size in X
void CartesianStripX::setStripSizeX(double cellSize) const {
    access()->implementation->setStripSizeX(cellSize);
}

/// access the coordinate offset in X
double CartesianStripX::offsetX() const { return access()->implementation->offsetX(); }

/// set the coordinate offset in X
void CartesianStripX::setOffsetX(double offset) const { access()->implementation->setOffsetX(offset); }

/// access the field name used for X
const string& CartesianStripX::fieldNameX() const { return access()->implementation->fieldNameX(); }

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in y
*/
vector<double> CartesianStripX::cellDimensions(const CellID& id) const {
    return access()->implementation->cellDimensions(id);
}
