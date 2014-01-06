/*
 * ProjectiveCylinder.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/ProjectiveCylinder.h"
#include "DDSegmentation/SegmentationUtil.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace DD4hep {
namespace DDSegmentation {

using std::string;
using Util::thetaFromXYZ;
using Util::phiFromXYZ;

/// default constructor using an encoding string
ProjectiveCylinder::ProjectiveCylinder(const string& cellEncoding) :
	CylindricalSegmentation(cellEncoding) {
	// define type and description
	_type = "ProjectiveCylinder";
	_description = "Projective segmentation in the global coordinates";

	// register all necessary parameters
	registerParameter("theta_bins", "Number of bins theta", _thetaBins, 1.);
	registerParameter("phi_bins", "Number of bins phi", _phiBins, 1.);
	registerParameter("offset_theta", "Angular offset in theta", _offsetTheta, 0., SegmentationParameter::AngleUnit, true);
	registerParameter("offset_phi", "Angular offset in phi", _offsetPhi, 0., SegmentationParameter::AngleUnit, true);
	registerIdentifier("identifier_theta", "Cell ID identifier for theta", _thetaID, "theta");
	registerIdentifier("identifier_phi", "Cell ID identifier for phi", _phiID, "phi");
}

/// destructor
ProjectiveCylinder::~ProjectiveCylinder() {

}

/// determine the local based on the cell ID
Position ProjectiveCylinder::position(const long64& cellID) const {
	return Util::positionFromRThetaPhi(1.0, theta(cellID), phi(cellID));
}

/// determine the cell ID based on the position
CellID ProjectiveCylinder::cellID(const Position& localPosition, const Position& globalPosition, const VolumeID& volumeID) const {
	_decoder->setValue(volumeID);
	double theta = thetaFromXYZ(globalPosition);
	double phi = phiFromXYZ(globalPosition);
	(*_decoder)[_thetaID] = positionToBin(theta, M_PI / (double) _thetaBins, _offsetTheta);
	(*_decoder)[_phiID] = positionToBin(phi, 2 * M_PI / (double) _phiBins, _offsetPhi);
	return _decoder->getValue();
}

/// determine the polar angle theta based on the cell ID
double ProjectiveCylinder::theta(const long64& cellID) const {
	int thetaIndex = (*_decoder)[_thetaID].value();
	return M_PI * ((double) thetaIndex + 0.5) / (double) _thetaBins;
}
/// determine the azimuthal angle phi based on the cell ID
double ProjectiveCylinder::phi(const long64& cellID) const {
	int phiIndex = (*_decoder)[_phiID].value();
	return 2. * M_PI * ((double) phiIndex + 0.5) / (double) _phiBins;
}

REGISTER_SEGMENTATION(ProjectiveCylinder)

} /* namespace DDSegmentation */
} /* namespace DD4hep */
