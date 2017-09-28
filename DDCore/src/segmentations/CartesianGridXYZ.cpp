/*
 * CartesianGridXYZ.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXYZ.h"

namespace dd4hep {
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

/// Default constructor used by derived classes passing an existing decoder
CartesianGridXYZ::CartesianGridXYZ(const BitFieldCoder* decode) :
		CartesianGridXY(decode) {
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
	Vector3D cellPosition;
	cellPosition.X = binToPosition( _decoder->get(cID,_xId ), _gridSizeX, _offsetX);
	cellPosition.Y = binToPosition( _decoder->get(cID,_yId ), _gridSizeY, _offsetY);
	cellPosition.Z = binToPosition( _decoder->get(cID,_zId ), _gridSizeZ, _offsetZ);
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID CartesianGridXYZ::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
        CellID cID = vID ;
	_decoder->set( cID,_xId, positionToBin(localPosition.X, _gridSizeX, _offsetX) );
	_decoder->set( cID,_yId, positionToBin(localPosition.Y, _gridSizeY, _offsetY) );
	_decoder->set( cID,_zId, positionToBin(localPosition.Z, _gridSizeZ, _offsetZ) );
	return cID ;
}

std::vector<double> CartesianGridXYZ::cellDimensions(const CellID&) const {
#if __cplusplus >= 201103L
  return {_gridSizeX, _gridSizeY, _gridSizeZ};
#else
  std::vector<double> cellDims(3,0.0);
  cellDims[0] = _gridSizeX;
  cellDims[1] = _gridSizeY;
  cellDims[2] = _gridSizeZ;
  return cellDims;
#endif
}

REGISTER_SEGMENTATION(CartesianGridXYZ)

} /* namespace DDSegmentation */
} /* namespace dd4hep */
