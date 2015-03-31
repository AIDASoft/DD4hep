/*
 * CartesianGridXY.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXY.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;

/// default constructor using an encoding string
CartesianGridXY::CartesianGridXY(const string& cellEncoding) :
		CartesianGrid(cellEncoding) {
	// define type and description
	_type = "CartesianGridXY";
	_description = "Cartesian segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
}

/// destructor
CartesianGridXY::~CartesianGridXY() {

}

/// determine the position based on the cell ID
Vector3D CartesianGridXY::position(const CellID& cID) const {
	_decoder->setValue(cID);
	Vector3D cellPosition;
	cellPosition.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	cellPosition.Y = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID CartesianGridXY::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	_decoder->setValue(vID);
	(*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX);
	(*_decoder)[_yId] = positionToBin(localPosition.Y, _gridSizeY, _offsetY);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(CartesianGridXY)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
