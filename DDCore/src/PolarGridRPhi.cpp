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
#include "DD4hep/PolarGridRPhi.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/PolarGridRPhi.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;

/// determine the position based on the cell ID
Position PolarGridRPhi::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
dd4hep::CellID PolarGridRPhi::cellID(const Position& local,
                                     const Position& global,
                                     const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in R
double PolarGridRPhi::gridSizeR() const  {
  return access()->implementation->gridSizeR();
}

/// access the grid size in Phi
double PolarGridRPhi::gridSizePhi() const  {
  return access()->implementation->gridSizePhi();
}

/// access the coordinate offset in R
double PolarGridRPhi::offsetR() const  {
  return access()->implementation->offsetR();
}

/// access the coordinate offset in Phi
double PolarGridRPhi::offsetPhi() const  {
  return access()->implementation->offsetPhi();
}

/// set the coordinate offset in R
void PolarGridRPhi::setOffsetR(double offset) const  {
  access()->implementation->setOffsetR(offset);
}

/// set the coordinate offset in Phi
void PolarGridRPhi::setOffsetPhi(double offset) const  {
  access()->implementation->setOffsetPhi(offset);
}

/// set the grid size in R
void PolarGridRPhi::setGridSizeR(double cellSize) const  {
  access()->implementation->setGridSizeR(cellSize);
}

/// set the grid size in Phi
void PolarGridRPhi::setGridSizePhi(double cellSize) const  {
  access()->implementation->setGridSizePhi(cellSize);
}

/// access the field name used for R
const string& PolarGridRPhi::fieldNameR() const  {
  return access()->implementation->fieldNameR();
}

/// access the field name used for Phi
const string& PolarGridRPhi::fieldNamePhi() const  {
  return access()->implementation->fieldNamePhi();
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

    Returns a vector of the cellDimensions of the given cell ID
    \param cellID is ignored as all cells have the same dimension
    \return vector<double> size 2:
    -# size in x
    -# size in z
*/
vector<double> PolarGridRPhi::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
