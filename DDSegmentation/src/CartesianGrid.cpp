/*
 * CartesianGrid.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGrid.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;

/// default constructor using an encoding string
template<> CartesianGrid::CartesianGrid(const string& cellEncoding) :
		Segmentation(cellEncoding) {

}

/// default constructor using an encoding string
template<> CartesianGrid::CartesianGrid(string cellEncoding) :
		Segmentation(cellEncoding) {

}

/// default constructor using an encoding string
template<> CartesianGrid::CartesianGrid(const char* cellEncoding) :
		Segmentation(cellEncoding) {

}

/// default constructor using an existing decoder
template<> CartesianGrid::CartesianGrid(BitField64* decoder) :
		Segmentation(decoder) {

}

/// destructor
CartesianGrid::~CartesianGrid() {

}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
