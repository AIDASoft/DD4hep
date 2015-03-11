/*
 * SegmentationUtil.h
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_SEGMENTATIONUTIL_H_
#define DDSegmentation_SEGMENTATIONUTIL_H_

#include <cmath>
#include <vector>

namespace DD4hep {
namespace DDSegmentation {
namespace Util {

///////////////////////////////////////////////////////////////////////
///           Conventions                                           ///
/// - x, y, z are the Cartesian coordinates                         ///
/// - r is the magnitude in the xy-plane                            ///
/// - mag is the magnitude                                          ///
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
/// Conversions from Cartesian to cylindrical/spherical coordinates ///
///////////////////////////////////////////////////////////////////////


/// calculates the radius in xyz from Cartesian coordinates
double magFromXYZ(const Vector3D& position) {
	return std::sqrt(position.X * position.X + position.Y * position.Y + position.Z * position.Z);
}

/// calculates the radius in the xy-plane from Cartesian coordinates
double radiusFromXYZ(const Vector3D& position) {
	return std::sqrt(position.X * position.X + position.Y * position.Y);
}

/// calculates the polar angle theta from Cartesian coordinates
double thetaFromXYZ(const Vector3D& position) {
	return std::acos(position.Z / radiusFromXYZ(position));
}

/// calculates the azimuthal angle phi from Cartesian coordinates
double phiFromXYZ(const Vector3D& position) {
	return std::atan2(position.Y, position.X);
}

/////////////////////////////////////////////////////////////
/// Conversions from cylindrical to Cartesian coordinates ///
/////////////////////////////////////////////////////////////

/// calculates the Cartesian position from cylindrical coordinates
Vector3D positionFromRPhiZ(double r, double phi, double z) {
	return Vector3D(r * std::cos(phi), r * std::sin(phi), z);
}

/// calculates the radius in xyz from cylindrical coordinates
double magFromRPhiZ(double r, double /* phi */, double z) {
	return std::sqrt(r * r + z * z);
}

/// calculates x from cylindrical coordinates
double xFromRPhiZ(double r, double phi, double /* z */) {
	return r * std::cos(phi);
}

/// calculates y from cylindrical coordinates
double yFromRPhiZ(double r, double phi, double /* z */) {
	return r * std::sin(phi);
}

/// calculates the polar angle theta from cylindrical coordinates
double thetaFromRPhiZ(double r, double /* phi */, double z) {
	return r * std::atan(z / r);
}

/////////////////////////////////////////////////////////////
/// Conversions from spherical to Cartesian coordinates   ///
/////////////////////////////////////////////////////////////

/// calculates the Cartesian position from spherical coordinates
Vector3D positionFromRThetaPhi(double r, double theta, double phi) {
	return Vector3D(r * std::cos(phi), r * std::sin(phi), r * std::tan(theta));
}

/// calculates the Cartesian position from spherical coordinates
Vector3D positionFromMagThetaPhi(double mag, double theta, double phi) {
	double r = mag * sin(theta);
	return Vector3D(r * std::cos(phi), r * std::sin(phi), mag * std::cos(theta));
}

} /* namespace Util */
} /* namespace DDSegmentation */
} /* namespace DD4hep */

#endif /* DDSegmentation_SEGMENTATIONUTIL_H_ */
