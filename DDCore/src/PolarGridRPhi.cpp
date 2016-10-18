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
#include "DD4hep/PolarGridRPhi.h"
#include "DDSegmentation/PolarGridRPhi.h"

// C/C++ include files

using namespace std;
using namespace DD4hep::Geometry;

/// Copy Constructor from segmentation base object
PolarGridRPhi::PolarGridRPhi(const Segmentation& e) : Handle<Object>()
{
  m_element = Segmentation::get<Object>(e.ptr());
}

/// determine the position based on the cell ID
Position PolarGridRPhi::position(const CellID& id) const   {
  return Position(access()->position(id));
}

/// determine the cell ID based on the position
DD4hep::CellID PolarGridRPhi::cellID(const Position& local,
                                     const Position& global,
                                     const VolumeID& volID) const
{
  return access()->cellID(local, global, volID);
}

/// access the grid size in R
double PolarGridRPhi::gridSizeR() const  {
  return access()->gridSizeR();
}

/// access the grid size in Phi
double PolarGridRPhi::gridSizePhi() const  {
  return access()->gridSizePhi();
}

/// access the coordinate offset in R
double PolarGridRPhi::offsetR() const  {
  return access()->offsetR();
}

/// access the coordinate offset in Phi
double PolarGridRPhi::offsetPhi() const  {
  return access()->offsetPhi();
}

/// access the field name used for R
const string& PolarGridRPhi::fieldNameR() const  {
  return access()->fieldNameR();
}

/// access the field name used for Phi
const string& PolarGridRPhi::fieldNamePhi() const  {
  return access()->fieldNamePhi();
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
  return access()->cellDimensions(id);
}
