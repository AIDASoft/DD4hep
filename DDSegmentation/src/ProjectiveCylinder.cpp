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

/// default constructor using an encoding string
ProjectiveCylinder::ProjectiveCylinder(const string& cellEncoding) :
	CylindricalSegmentation(cellEncoding) {
	// define type and description
	_type = "ProjectiveCylinder";
	_description = "Projective segmentation in the global coordinates";

	// register all necessary parameters
	registerParameter("thetaBins", "Number of bins theta", _thetaBins, 1.);
	registerParameter("phiBins", "Number of bins phi", _phiBins, 1.);
	registerParameter("offsetTheta", "Angular offset in theta", _offsetTheta, 0., true);
	registerParameter("offsetPhi", "Angular offset in phi", _offsetPhi, 0., true);
	_thetaID = "theta";
	_phiID = "phi";
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
	// TODO
	return 0;
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
