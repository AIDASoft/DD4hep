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
#include "DD4hep/PolarGridRPhi2.h"
#include "DDSegmentation/PolarGridRPhi2.h"

// C/C++ include files

using namespace std;
using namespace DD4hep::Geometry;

/// determine the position based on the cell ID
Position PolarGridRPhi2::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
DD4hep::CellID PolarGridRPhi2::cellID(const Position& local,
                                      const Position& global,
                                      const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

/// access the grid size in R
vector<double> PolarGridRPhi2::gridRValues() const  {
  return access()->implementation->gridRValues();
}

/// access the grid size in Phi
vector<double> PolarGridRPhi2::gridPhiValues() const  {
  return access()->implementation->gridPhiValues();
}

/// access the coordinate offset in R
double PolarGridRPhi2::offsetR() const  {
  return access()->implementation->offsetR();
}

/// access the coordinate offset in Phi
double PolarGridRPhi2::offsetPhi() const  {
  return access()->implementation->offsetPhi();
}

/// access the field name used for R
const string& PolarGridRPhi2::fieldNameR() const  {
  return access()->implementation->fieldNameR();
}

/// access the field name used for Phi
const string& PolarGridRPhi2::fieldNamePhi() const  {
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
vector<double> PolarGridRPhi2::cellDimensions(const CellID& id) const  {
  return access()->implementation->cellDimensions(id);
}
