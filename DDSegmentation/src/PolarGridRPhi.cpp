/*
 * PolarGridRPhi.cpp
 *
 *  Created on: Sept 16, 2014
 *      Author: Marko Petric
 */

#include "DDSegmentation/PolarGridRPhi.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
PolarGridRPhi::PolarGridRPhi(const std::string& cellEncoding) :
		PolarGrid(cellEncoding) {
	// define type and description
	_type = "PolarGridRPhi";
	_description = "Polar RPhi segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("grid_size_r", "Cell size in R", _gridSizeR, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_phi", "Cell size in Phi", _gridSizePhi, 1., SegmentationParameter::AngleUnit);
	registerParameter("offset_r", "Cell offset in R", _offsetR, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_phi", "Cell offset in Phi", _offsetPhi, 0., SegmentationParameter::AngleUnit, true);
	registerIdentifier("identifier_r", "Cell ID identifier for R", _rId, "r");
	registerIdentifier("identifier_phi", "Cell ID identifier for Phi", _phiId, "phi");
}

/// destructor
PolarGridRPhi::~PolarGridRPhi() {

}

/// determine the position based on the cell ID
Vector3D PolarGridRPhi::position(const CellID& cID) const {
	_decoder->setValue(cID);
	Vector3D cellPosition;
	double R = binToPosition((*_decoder)[_rId].value(), _gridSizeR, _offsetR);
	double phi = binToPosition((*_decoder)[_phiId].value(), _gridSizePhi, _offsetPhi);
	
	cellPosition.X = R * cos(phi);
	cellPosition.Y = R * sin(phi);
 	
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID PolarGridRPhi::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	_decoder->setValue(vID);
	double phi = atan2(localPosition.Y,localPosition.X);
	double R = sqrt( localPosition.X * localPosition.X + localPosition.Y * localPosition.Y );

	(*_decoder)[_rId] = positionToBin(R, _gridSizeR, _offsetR);
	(*_decoder)[_phiId] = positionToBin(phi, _gridSizePhi, _offsetPhi);
	return _decoder->getValue();
}

REGISTER_SEGMENTATION(PolarGridRPhi)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
