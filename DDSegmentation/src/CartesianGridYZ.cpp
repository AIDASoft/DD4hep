/* CartesianGridYZ.cpp
 *
 *  @date: Sep 03, 2014
 *  @author: F.Gaede CERN/Desy
 *  @version: $Id$
 *     direkt copy of CartesianGridXY
 *     by Christian Grefe, CERN
 */
#include "DDSegmentation/CartesianGridYZ.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;

/// default constructor using an encoding string
CartesianGridYZ::CartesianGridYZ(const string& cellEncoding) :
		CartesianGrid(cellEncoding) {
	// define type and description
	_type = "CartesianGridYZ";
	_description = "Cartesian segmentation in the local YZ-plane";

	// register all necessary parameters
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_z", "Cell size in Z", _gridSizeZ, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_z", "Cell offset in Z", _offsetZ, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerIdentifier("identifier_z", "Cell ID identifier for Z", _zId, "z");
}

/// destructor
CartesianGridYZ::~CartesianGridYZ() {

}

/// determine the position based on the cell ID
Vector3D CartesianGridYZ::position(const CellID& cellID) const {
	_decoder->setValue(cellID);
	Vector3D position;
	position.Y = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	position.Z = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return position;
}

/// determine the cell ID based on the position
CellID CartesianGridYZ::cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const {
	_decoder->setValue(volumeID);
	(*_decoder)[_yId] = positionToBin(localPosition.Y, _gridSizeY, _offsetY);
	(*_decoder)[_zId] = positionToBin(localPosition.Z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(CartesianGridYZ)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
