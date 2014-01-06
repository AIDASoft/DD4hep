/*
 * CartesianGridXYZ.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXYZ.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;

/// default constructor using an encoding string
CartesianGridXYZ::CartesianGridXYZ(const string& cellEncoding) :
		CartesianGridXY(cellEncoding) {
	// define type and description
	_type = "CartesianGridXYZ";
	_description = "Cartesian segmentation in the local coordinates";

	// register all necessary parameters
	registerParameter("grid_size_z", "Cell size in Z", _gridSizeZ, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_z", "Cell offset in Z", _offsetZ, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_z", "Cell ID identifier for Z", _zId, "z");
}

/// destructor
CartesianGridXYZ::~CartesianGridXYZ() {

}

/// determine the position based on the cell ID
Position CartesianGridXYZ::position(const CellID& cellID) const {
	_decoder->setValue(cellID);
	Position position;
	position.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	position.Y = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	position.Z = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return position;
}

/// determine the cell ID based on the position
CellID CartesianGridXYZ::cellID(const Position& localPosition, const Position& globalPosition, const VolumeID& volumeID) const {
	_decoder->setValue(volumeID);
	(*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX);
	(*_decoder)[_yId] = positionToBin(localPosition.Y, _gridSizeY, _offsetY);
	(*_decoder)[_zId] = positionToBin(localPosition.Z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(CartesianGridXYZ)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
