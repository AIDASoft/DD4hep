/*
 * CartesianGrid.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGrid.h"

namespace DD4hep {
  namespace DDSegmentation {

    /// Default constructor used by derived classes passing the encoding string
    CartesianGrid::CartesianGrid(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
    }

    /// Default constructor used by derived classes passing an existing decoder
    CartesianGrid::CartesianGrid(BitField64* decode) : Segmentation(decode) {
    }

    /// Destructor
    CartesianGrid::~CartesianGrid() {
    }

  } /* namespace DDSegmentation */
} /* namespace DD4hep */
