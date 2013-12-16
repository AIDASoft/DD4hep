/*
 * CartesianGridXY.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXZ.h"

namespace DD4hep {
namespace DDSegmentation {

using std::make_pair;
using std::string;
using std::vector;

/// default constructor using an encoding string
CartesianGridXZ::CartesianGridXZ(const string& cellEncoding) :
	CartesianGrid(cellEncoding) {
	// define type and description
	_type = "CartesianGridXY";
	_description = "Cartesian segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("gridSizeX", "Cell size in X", _gridSizeX, 1.);
	registerParameter("gridSizeZ", "Cell size in Z", _gridSizeZ, 1.);
	registerParameter("offsetX", "Cell offset in X", _offsetX, 0., true);
	registerParameter("offsetZ", "Cell offset in Z", _offsetZ, 0., true);
	_xId = "x";
	_zId = "y";
}

/// destructor
CartesianGridXZ::~CartesianGridXZ() {

}

/// determine the position based on the cell ID
Position CartesianGridXZ::position(const CellID& cellID) const {
	_decoder->setValue(cellID);
	vector<double> localPosition(3);
	Position position;
	position.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	position.Z = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return position;
}

/// determine the cell ID based on the position
CellID CartesianGridXZ::cellID(const Position& localPosition, const Position& globalPosition, const VolumeID& volumeID) const {
	_decoder->reset();
	(*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX);
	(*_decoder)[_zId] = positionToBin(localPosition.Z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(CartesianGridXZ)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
