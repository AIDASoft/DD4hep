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
}

/// default constructor using an encoding string
template<> Segmentation::Segmentation(const std::string& cellEncoding) {
	_decoder = new BitField64(cellEncoding);
	_ownsDecoder = true;
	_type = "segmentation";
}

/// default constructor using an encoding string
template<> Segmentation::Segmentation(const char* cellEncoding) {
	_decoder = new BitField64(cellEncoding);
	_ownsDecoder = true;
	_type = "segmentation";
}

/// default constructor using an existing decoder
Segmentation::Segmentation(BitField64* decoder) {
	_decoder = decoder;
	_ownsDecoder = false;
	_type = "segmentation";
}

/// destructor
Segmentation::~Segmentation() {
	if (_ownsDecoder and _decoder != 0) {
		delete _decoder;
	}
}

/// determine the cell ID based on the local position
long64 Segmentation::getCellID(const vector<double>& localPosition) const {
	if (localPosition.size() != 3) {
		throw std::runtime_error("Vector size has to be 3!");
	}
	return getCellID(localPosition[0], localPosition[1], localPosition[2]);
}

/// determine the cell ID based on the local position
long64 Segmentation::getCellID(const double* localPosition) const {
	return getCellID(localPosition[0], localPosition[1], localPosition[2]);
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

} /* namespace DDSegmentation */
} /* namespace DD4hep */
