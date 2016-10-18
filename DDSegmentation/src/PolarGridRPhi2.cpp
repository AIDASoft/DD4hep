/*
 * PolarGridRPhi2.cpp
 *
 *  Created on: March 17, 2015
 *      Author: Marko Petric, Andre Sailer
 */

#include "DDSegmentation/PolarGridRPhi2.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
PolarGridRPhi2::PolarGridRPhi2(const std::string& cellEncoding) :
		PolarGrid(cellEncoding) {
	// define type and description
	_type = "PolarGridRPhi2";
	_description = "Polar RPhi segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("grid_r_values", "Vector of R values", _gridRValues, std::vector<double>(), SegmentationParameter::LengthUnit, true);
	registerParameter("grid_phi_values", "Cell size in Phi", _gridPhiValues, std::vector<double>(), SegmentationParameter::AngleUnit);
	registerParameter("offset_r", "Cell offset in R", _offsetR, double(0.), SegmentationParameter::LengthUnit, true);
	registerParameter("offset_phi", "Cell offset in Phi", _offsetPhi, double(0.), SegmentationParameter::AngleUnit, true);
	registerIdentifier("identifier_r", "Cell ID identifier for R", _rId, "r");
	registerIdentifier("identifier_phi", "Cell ID identifier for Phi", _phiId, "phi");
}

/// Default constructor used by derived classes passing an existing decoder
PolarGridRPhi2::PolarGridRPhi2(BitField64* decoder) :
		PolarGrid(decoder) {
	// define type and description
	_type = "PolarGridRPhi2";
	_description = "Polar RPhi segmentation in the local XY-plane";

	// register all necessary parameters
	registerParameter("grid_r_values", "Vector of R values", _gridRValues, std::vector<double>(), SegmentationParameter::LengthUnit, true);
	registerParameter("grid_phi_values", "Cell size in Phi", _gridPhiValues, std::vector<double>(), SegmentationParameter::AngleUnit);
	registerParameter("offset_r", "Cell offset in R", _offsetR, double(0.), SegmentationParameter::LengthUnit, true);
	registerParameter("offset_phi", "Cell offset in Phi", _offsetPhi, double(0.), SegmentationParameter::AngleUnit, true);
	registerIdentifier("identifier_r", "Cell ID identifier for R", _rId, "r");
	registerIdentifier("identifier_phi", "Cell ID identifier for Phi", _phiId, "phi");
}

/// destructor
PolarGridRPhi2::~PolarGridRPhi2() {

}

/// determine the position based on the cell ID
Vector3D PolarGridRPhi2::position(const CellID& cID) const {
	_decoder->setValue(cID);
	Vector3D cellPosition;
	const int rBin = (*_decoder)[_rId].value();
	double R = binToPosition(rBin, _gridRValues, _offsetR);
	double phi = binToPosition((*_decoder)[_phiId].value(), _gridPhiValues[rBin], _offsetPhi+_gridPhiValues[rBin]*0.5);

	if ( phi < _offsetPhi) {
	  phi += 2*M_PI;
	}
	
	cellPosition.X = R * cos(phi);
	cellPosition.Y = R * sin(phi);
 	
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID PolarGridRPhi2::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	_decoder->setValue(vID);
	double phi = atan2(localPosition.Y,localPosition.X);
	double R = sqrt( localPosition.X * localPosition.X + localPosition.Y * localPosition.Y );

	const int rBin = positionToBin(R, _gridRValues, _offsetR);
	(*_decoder)[_rId] = rBin;

	if ( phi < _offsetPhi) {
	  phi += 2*M_PI;
	}
	const int pBin = positionToBin(phi, _gridPhiValues[rBin], _offsetPhi+_gridPhiValues[rBin]*0.5);
	(*_decoder)[_phiId] = pBin;

	return _decoder->getValue();
}


std::vector<double> PolarGridRPhi2::cellDimensions(const CellID& cID) const {

  _decoder->setValue(cID);

  const int rBin = (*_decoder)[_rId].value();
  const double rCenter = binToPosition(rBin, _gridRValues, _offsetR);

  const double rPhiSize = _gridPhiValues[rBin]*rCenter;
  const double rSize = _gridRValues[rBin+1]-_gridRValues[rBin];

#if __cplusplus >= 201103L
  return {rSize, rPhiSize};
#else
  std::vector<double> cellDims(2,0.0);
  cellDims[0] = rSize;
  cellDims[1] = rPhiSize;
  return cellDims;
#endif
}

REGISTER_SEGMENTATION(PolarGridRPhi2)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
