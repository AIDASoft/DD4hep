/*
 * Segmentation.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/Segmentation.h"

#include <stdexcept>

namespace DD4hep {
namespace DDSegmentation {

using std::string;
using std::vector;

/// default constructor using an encoding string
template<> Segmentation::Segmentation(std::string cellEncoding) {
	_decoder = new BitField64(cellEncoding);
	_ownsDecoder = true;
	_type = "segmentation";
	_isLocal = true;
}

/// default constructor using an encoding string
template<> Segmentation::Segmentation(const std::string& cellEncoding) {
	_decoder = new BitField64(cellEncoding);
	_ownsDecoder = true;
	_type = "segmentation";
	_isLocal = true;
}

/// default constructor using an encoding string
template<> Segmentation::Segmentation(const char* cellEncoding) {
	_decoder = new BitField64(cellEncoding);
	_ownsDecoder = true;
	_type = "segmentation";
	_isLocal = true;
}

/// default constructor using an existing decoder
template<> Segmentation::Segmentation(BitField64* decoder) {
	_decoder = decoder;
	_ownsDecoder = false;
	_type = "segmentation";
	_isLocal = true;
}

/// destructor
Segmentation::~Segmentation() {
	if (_ownsDecoder and _decoder != 0) {
		delete _decoder;
	}
}

/// determine the cell ID based on the position
long64 Segmentation::getCellID(const vector<double>& position) const {
	if (position.size() != 3) {
		throw std::runtime_error("Vector size has to be 3!");
	}
	return getCellID(position[0], position[1], position[2]);
}

/// determine the cell ID based on the position
long64 Segmentation::getCellID(const double* position) const {
	return getCellID(position[0], position[1], position[2]);
}

/// set the underlying decoder
void Segmentation::setDecoder(BitField64* decoder) {
	if (_ownsDecoder and _decoder != 0) {
		delete _decoder;
	}
	_decoder = decoder;
	_ownsDecoder = false;
}

/// access the set of parameters for this segmentation
Parameters Segmentation::parameters() const {
	return Parameters();
}

/// helper method to convert a bin number to a 1D position
double Segmentation::binToPosition(long64 bin, double cellSize, double offset) const {
	return bin * cellSize + offset;
}

/// helper method to convert a 1D position to a cell ID
int Segmentation::positionToBin(double position, double cellSize, double offset) const {
	if (cellSize == 0.) {
		throw std::runtime_error("Invalid cell size: 0.0");
	}
	return int((position + 0.5 * cellSize - offset)/cellSize);
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
