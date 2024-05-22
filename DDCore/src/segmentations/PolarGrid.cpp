//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
/*
 * PolarGrid.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: Marko Petric
 */

#include <DDSegmentation/PolarGrid.h>

namespace dd4hep {
  namespace DDSegmentation {

    /// Default constructor used by derived classes passing the encoding string
    PolarGrid::PolarGrid(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
    }
    /// Default constructor used by derived classes passing an existing decoder
    PolarGrid::PolarGrid(const BitFieldCoder* decode)	: Segmentation(decode) {
    }

    /// Destructor
    PolarGrid::~PolarGrid() {
    }

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
