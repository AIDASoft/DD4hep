/*
 * PolarGrid.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: Marko Petric
 */

#include "DDSegmentation/PolarGrid.h"

namespace DD4hep {
namespace DDSegmentation {

/// Default constructor used by derived classes passing the encoding string
PolarGrid::PolarGrid(const std::string& cellEncoding) :
		Segmentation(cellEncoding) {
}

/// Destructor
PolarGrid::~PolarGrid() {
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
