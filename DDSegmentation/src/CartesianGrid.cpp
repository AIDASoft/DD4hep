/*
 * CartesianGrid.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGrid.h"

#include <stdexcept>

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
CartesianGrid::CartesianGrid(BitField64* decoder) :
		Segmentation(decoder) {

}

/// destructor
CartesianGrid::~CartesianGrid() {

}

/// helper method to convert a bin number to a 1D position
double CartesianGrid::binToPosition(long64 bin, double cellSize, double offset) const {
	return bin * cellSize + offset;
}

/// helper method to convert a 1D position to a cell ID
int CartesianGrid::positionToBin(double position, double cellSize, double offset) const {
	if (cellSize == 0.) {
		throw std::runtime_error("Invalid cell size: 0.0");
	}
	return int((position + 0.5 * cellSize - offset)/cellSize);
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
