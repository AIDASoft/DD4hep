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
#include "DD4hep/NoSegmentation.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DDSegmentation/NoSegmentation.h"

// C/C++ include files

using namespace std;
using namespace DD4hep::Geometry;

Position NoSegmentation::position(const CellID& id) const   {
  return Position(access()->implementation->position(id));
}

/// determine the cell ID based on the position
DD4hep::CellID NoSegmentation::cellID(const Position& local,
                                       const Position& global,
                                       const VolumeID& volID) const
{
  return access()->implementation->cellID(local, global, volID);
}

