/*
 * CartesianGridXZ.cpp
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
	_type = "CartesianGridXZ";
	_description = "Cartesian segmentation in the local XZ-plane";

	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_z", "Cell size in Z", _gridSizeZ, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_z", "Cell offset in Z", _offsetZ, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_z", "Cell ID identifier for Z", _zId, "z");
}

/// destructor
CartesianGridXZ::~CartesianGridXZ() {

}

/// determine the position based on the cell ID
Vector3D CartesianGridXZ::position(const CellID& cellID) const {
	_decoder->setValue(cellID);
	vector<double> localPosition(3);
	Vector3D position;
	position.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	position.Z = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return position;
}

/// determine the cell ID based on the position
CellID CartesianGridXZ::cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const {
	_decoder->setValue(volumeID);
	(*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX);
	(*_decoder)[_zId] = positionToBin(localPosition.Z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(CartesianGridXZ)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
