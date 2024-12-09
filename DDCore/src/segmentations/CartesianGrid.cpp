//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  Created: Jun 28, 2013
//  Author:  Christian Grefe, CERN
//
//==========================================================================

/// Framework include files
#include <DDSegmentation/CartesianGrid.h>

namespace dd4hep {
  
  namespace DDSegmentation {

    /// Default constructor used by derived classes passing the encoding string
    CartesianGrid::CartesianGrid(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
    }

    /// Default constructor used by derived classes passing an existing decoder
    CartesianGrid::CartesianGrid(const BitFieldCoder* decode) : Segmentation(decode) {
    }

    /// Destructor
    CartesianGrid::~CartesianGrid() {
    }

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
