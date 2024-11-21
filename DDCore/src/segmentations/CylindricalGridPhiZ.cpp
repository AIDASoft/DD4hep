//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
/*
 * CylindricalGridPhiZ.cpp
 *
 *  Created on: Jun 28, 2024
 *      Author: Yann Bedfer, ePIC/Saclay
 */

#include <DDSegmentation/CylindricalGridPhiZ.h>

namespace dd4hep {
namespace DDSegmentation {

using std::make_pair;
using std::vector;

/// default constructor using an encoding string
CylindricalGridPhiZ::CylindricalGridPhiZ(const std::string& cellEncoding) :
	CylindricalSegmentation(cellEncoding) {
	// define type and description
	_type = "CylindricalGridPhiZ";
	_description = "Cylindrical segmentation in the local PhiZ-cylinder";

	// register all necessary parameters
	registerParameter("grid_size_phi", "Cell size in phi", _gridSizePhi, 1., SegmentationParameter::AngleUnit);
	registerParameter("grid_size_z",   "Cell size in Z",   _gridSizeZ,   1., SegmentationParameter::LengthUnit);
	registerParameter("offset_phi", "Cell offset in phi", _offsetPhi,    0., SegmentationParameter::AngleUnit, true);
	registerParameter("offset_z",   "Cell offset in Z",   _offsetZ,      0., SegmentationParameter::LengthUnit, true);
	registerParameter("radius",     "Radius of Cylinder", _radius,       0., SegmentationParameter::LengthUnit);
	registerIdentifier("identifier_phi", "Cell ID identifier for phi", _phiId, "phi");
	registerIdentifier("identifier_z",   "Cell ID identifier for Z",   _zId,   "z");
}

/// Default constructor used by derived classes passing an existing decoder
CylindricalGridPhiZ::CylindricalGridPhiZ(const BitFieldCoder* decode) :
	CylindricalSegmentation(decode) {
	// define type and description
	_type = "CylindricalGridPhiZ";
	_description = "Cylindrical segmentation in the local PhiZ-cylinder";

	// register all necessary parameters
	registerParameter("grid_size_phi", "Cell size in phi", _gridSizePhi, 1., SegmentationParameter::AngleUnit);
	registerParameter("grid_size_z",   "Cell size in Z",   _gridSizeZ,   1., SegmentationParameter::LengthUnit);
	registerParameter("offset_phi", "Cell offset in phi", _offsetPhi,    0., SegmentationParameter::AngleUnit, true);
	registerParameter("offset_z",   "Cell offset in Z",   _offsetZ,      0., SegmentationParameter::LengthUnit, true);
	registerParameter("radius",     "Radius of Cylinder", _radius,       0., SegmentationParameter::LengthUnit);
	registerIdentifier("identifier_phi", "Cell ID identifier for phi", _phiId, "phi");
	registerIdentifier("identifier_z",   "Cell ID identifier for Z",   _zId,   "z");
}

/// destructor
CylindricalGridPhiZ::~CylindricalGridPhiZ() {

}

/// Set the underlying decoder and assign isSigned attribute to phi identifier
void CylindricalGridPhiZ::setDecoder(const BitFieldCoder* newDecoder) {
  this->Segmentation::setDecoder(newDecoder);
  const BitFieldElement* m_phi = &((*_decoder)[_phiId]);
  _phiIsSigned = m_phi->isSigned();
}

/// determine the position based on the cell ID
Vector3D CylindricalGridPhiZ::position(const CellID& cID) const {
	vector<double> localPosition(3);
	Vector3D cellPosition;
	double phi =
	  binToPosition( _decoder->get(cID,_phiId ), _gridSizePhi, _offsetPhi);
	cellPosition.Z =
	  binToPosition( _decoder->get(cID,_zId ),   _gridSizeZ,   _offsetZ);
	const double &R = _radius;
	cellPosition.X = R*cos(phi); cellPosition.Y = R*sin(phi);

	return cellPosition;
}

/// determine the cell ID based on the position
  CellID CylindricalGridPhiZ::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	double phi = atan2(localPosition.Y,localPosition.X);
	double Z = localPosition.Z;
	if (!_phiIsSigned && phi < _offsetPhi) {
	  phi += 2*M_PI;
	}
        CellID cID = vID ;
        _decoder->set( cID,_phiId, positionToBin(phi, _gridSizePhi, _offsetPhi) );
	_decoder->set( cID,_zId,   positionToBin(Z,   _gridSizeZ,   _offsetZ) );

	return cID ;
}

std::vector<double> CylindricalGridPhiZ::cellDimensions(const CellID&) const {
  return {_radius*_gridSizePhi, _gridSizeZ};
}


} /* namespace DDSegmentation */
} /* namespace dd4hep */
