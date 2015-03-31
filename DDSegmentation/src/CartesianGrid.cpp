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

/// Destructor
CartesianGrid::~CartesianGrid() {
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
