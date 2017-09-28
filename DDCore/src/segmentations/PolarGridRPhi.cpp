/*
 * PolarGridRPhi.cpp
 *
 *  Created on: Sept 16, 2014
 *      Author: Marko Petric
 */

#include "DDSegmentation/PolarGridRPhi.h"

namespace dd4hep {
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


/// Default constructor used by derived classes passing an existing decoder
PolarGridRPhi::PolarGridRPhi(const BitFieldCoder* decode) : PolarGrid(decode) {
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
	Vector3D cellPosition;
	double R =   binToPosition(_decoder->get(cID,_rId),   _gridSizeR,   _offsetR);
	double phi = binToPosition(_decoder->get(cID,_phiId), _gridSizePhi, _offsetPhi);
	
	cellPosition.X = R * cos(phi);
	cellPosition.Y = R * sin(phi);
 	
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID PolarGridRPhi::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	double phi = atan2(localPosition.Y,localPosition.X);
	double R = sqrt( localPosition.X * localPosition.X + localPosition.Y * localPosition.Y );
	CellID cID = vID ;
	_decoder->set(cID,_rId  , positionToBin(R, _gridSizeR, _offsetR));
	_decoder->set(cID,_phiId, positionToBin(phi, _gridSizePhi, _offsetPhi));
	return cID;
}

std::vector<double> PolarGridRPhi::cellDimensions(const CellID& cID) const {
  const double rPhiSize = binToPosition(_decoder->get(cID,_rId), _gridSizeR, _offsetR)*_gridSizePhi;
#if __cplusplus >= 201103L
  return {_gridSizeR, rPhiSize};
#else
  std::vector<double> cellDims(2,0.0);
  cellDims[0] = _gridSizeR;
  cellDims[1] = rPhiSize;
  return cellDims;
#endif
}

REGISTER_SEGMENTATION(PolarGridRPhi)

} /* namespace DDSegmentation */
} /* namespace dd4hep */
