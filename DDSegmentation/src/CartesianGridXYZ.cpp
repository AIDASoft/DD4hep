/*
 * CartesianGridXYZ.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXYZ.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
CartesianGridXYZ::CartesianGridXYZ(const std::string& cellEncoding) :
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
Vector3D CartesianGridXYZ::position(const CellID& cID) const {
	_decoder->setValue(cID);
	Vector3D cellPosition;
	cellPosition.X = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	cellPosition.Y = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	cellPosition.Z = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID CartesianGridXYZ::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	_decoder->setValue(vID);
	(*_decoder)[_xId] = positionToBin(localPosition.X, _gridSizeX, _offsetX);
	(*_decoder)[_yId] = positionToBin(localPosition.Y, _gridSizeY, _offsetY);
	(*_decoder)[_zId] = positionToBin(localPosition.Z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

std::vector<double> CartesianGridXYZ::cellDimensions(const CellID&) const {
#ifdef DD4HEP_USE_CXX11
  return {_gridSizeX, _gridSizeY, _gridSizeZ};
#else
  std::vector<double> cellDimensions(3,0.0);
  cellDimensions[0] = _gridSizeX;
  cellDimensions[1] = _gridSizeY;
  cellDimensions[2] = _gridSizeZ;
  return cellDimensions;
#endif
}

REGISTER_SEGMENTATION(CartesianGridXYZ)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
