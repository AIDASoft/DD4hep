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
using SegmentationUtil::getPositionFromRThetaPhi;

/// default constructor using an encoding string
template<> ProjectiveCylinder::ProjectiveCylinder(const string& cellEncoding, int thetaBins, int phiBins,
		double offsetTheta, double offsetPhi, const string& thetaField, const string& phiField,
		const string& layerField) :
		CylindricalSegmentation(cellEncoding, layerField), _thetaBins(thetaBins), _phiBins(phiBins), _offsetTheta(
				offsetTheta), _offsetPhi(offsetPhi), _thetaID(thetaField), _phiID(phiField) {
	_type = "projective_cylinder";
}

/// default constructor using an encoding string
template<> ProjectiveCylinder::ProjectiveCylinder(string cellEncoding, int thetaBins, int phiBins, double offsetTheta,
		double offsetPhi, const string& thetaField, const string& phiField, const string& layerField) :
		CylindricalSegmentation(cellEncoding, layerField), _thetaBins(thetaBins), _phiBins(phiBins), _offsetTheta(
				offsetTheta), _offsetPhi(offsetPhi), _thetaID(thetaField), _phiID(phiField) {
	_type = "projective_cylinder";
}

/// default constructor using an encoding string
template<> ProjectiveCylinder::ProjectiveCylinder(const char* cellEncoding, int thetaBins, int phiBins,
		double offsetTheta, double offsetPhi, const string& thetaField, const string& phiField,
		const string& layerField) :
		CylindricalSegmentation(cellEncoding, layerField), _thetaBins(thetaBins), _phiBins(phiBins), _offsetTheta(
				offsetTheta), _offsetPhi(offsetPhi), _thetaID(thetaField), _phiID(phiField) {
	_type = "projective_cylinder";
}

/// default constructor using an existing decoder
template<> ProjectiveCylinder::ProjectiveCylinder(BitField64* decoder, int thetaBins, int phiBins, double offsetTheta,
		double offsetPhi, const string& thetaField, const string& phiField, const string& layerField) :
		CylindricalSegmentation(decoder, layerField), _thetaBins(thetaBins), _phiBins(phiBins), _offsetTheta(
				offsetTheta), _offsetPhi(offsetPhi), _thetaID(thetaField), _phiID(phiField) {
	_type = "projective_cylinder";
}

/// destructor
ProjectiveCylinder::~ProjectiveCylinder() {

}

/// determine the local based on the cell ID
std::vector<double> ProjectiveCylinder::getPosition(const long64& cellID) const {
	double r = getRadius(cellID);
	double theta = getTheta(cellID);
	double phi = getPhi(cellID);
	return getPositionFromRThetaPhi(r, theta, phi);
}

/// determine the cell ID based on the position
virtual long64 ProjectiveCylinder::getCellID(double x, double y, double z) const {
	// TODO
	return 0;
}

/// determine the polar angle theta based on the cell ID
double ProjectiveCylinder::getTheta(const long64& cellID) const {
	int thetaIndex = (*_decoder)[_thetaID].value();
	return M_PI * ((double) thetaIndex + 0.5) / (double) _thetaBins;
}
/// determine the azimuthal angle phi based on the cell ID
double ProjectiveCylinder::getPhi(const long64& cellID) const {
	int phiIndex = (*_decoder)[_phiID].value();
	return 2. * M_PI * ((double) phiIndex + 0.5) / (double) _phiBins;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
