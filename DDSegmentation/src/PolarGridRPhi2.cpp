/*
 * PolarGridRPhi2.cpp
 *
 *  Created on: Sept 16, 2014
 *      Author: Marko Petric
 */

#include "DDSegmentation/PolarGridRPhi2.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;

/// default constructor using an encoding string
PolarGridRPhi2::PolarGridRPhi2(const string& cellEncoding) :
		PolarGrid(cellEncoding) {
	// define type and description
	_type = "PolarGridRPhi2";
	_description = "Polar RPhi segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("grid_r_values", "Vector or R values", _gridRValues, std::vector<double>(), SegmentationParameter::LengthUnit, true);
	registerParameter("grid_size_phi", "Cell size in Phi", _gridSizePhi, 1., SegmentationParameter::AngleUnit);
	registerParameter("offset_r", "Cell offset in R", _offsetR, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_phi", "Cell offset in Phi", _offsetPhi, 0., SegmentationParameter::AngleUnit, true);
	registerIdentifier("identifier_r", "Cell ID identifier for R", _rId, "r");
	registerIdentifier("identifier_phi", "Cell ID identifier for Phi", _phiId, "phi");
}

/// destructor
PolarGridRPhi2::~PolarGridRPhi2() {

}

/// determine the position based on the cell ID
Vector3D PolarGridRPhi2::position(const CellID& cellID) const {
	_decoder->setValue(cellID);
	Vector3D position;
	double R = binToPosition((*_decoder)[_rId].value(), _gridRValues[0], _offsetR);
	double phi = binToPosition((*_decoder)[_phiId].value(), _gridSizePhi, _offsetPhi);
	
	position.X = R * cos(phi);
	position.Y = R * sin(phi);
 	
	return position;
}

/// determine the cell ID based on the position
  CellID PolarGridRPhi2::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& volumeID) const {
	_decoder->setValue(volumeID);
	double phi = atan2(localPosition.Y,localPosition.X);
	double R = sqrt( localPosition.X * localPosition.X + localPosition.Y * localPosition.Y );

	(*_decoder)[_rId] = positionToBin(R, _gridRValues[0], _offsetR);
	(*_decoder)[_phiId] = positionToBin(phi, _gridSizePhi, _offsetPhi);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(PolarGridRPhi2)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
