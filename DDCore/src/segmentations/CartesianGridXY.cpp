/*
 * CartesianGridXY.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXY.h"

namespace dd4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
CartesianGridXY::CartesianGridXY(const std::string& cellEncoding) :
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

/// Default constructor used by derived classes passing an existing decoder
CartesianGridXY::CartesianGridXY(BitField64* decode) :
		CartesianGrid(decode)
{
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

std::vector<double> CartesianGridXY::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
  return {_gridSizeX, _gridSizeY};
#else
  std::vector<double> cellDims(2,0.0);
  cellDims[0] = _gridSizeX;
  cellDims[1] = _gridSizeY;
  return cellDims;
#endif
}

REGISTER_SEGMENTATION(CartesianGridXY)

} /* namespace DDSegmentation */
} /* namespace dd4hep */
