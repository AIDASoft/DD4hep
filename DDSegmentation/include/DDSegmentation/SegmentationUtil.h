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

#include "TVector3.h"

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
inline double magFromXYZ(const Vector3D& position) {
	return std::sqrt(position.X * position.X + position.Y * position.Y + position.Z * position.Z);
}

/// calculates the radius in the xy-plane from Cartesian coordinates
inline double radiusFromXYZ(const Vector3D& position) {
	return std::sqrt(position.X * position.X + position.Y * position.Y);
}

/// calculates the polar angle theta from Cartesian coordinates
inline double thetaFromXYZ(const Vector3D& position) {
	return std::acos(position.Z / radiusFromXYZ(position));
}

/// calculates the azimuthal angle phi from Cartesian coordinates
inline double phiFromXYZ(const Vector3D& position) {
	return std::atan2(position.Y, position.X);
}

/////////////////////////////////////////////////////////////
/// Conversions from cylindrical to Cartesian coordinates ///
/////////////////////////////////////////////////////////////

/// calculates the Cartesian position from cylindrical coordinates
inline Vector3D positionFromRPhiZ(double r, double phi, double z) {
	return Vector3D(r * std::cos(phi), r * std::sin(phi), z);
}

/// calculates the radius in xyz from cylindrical coordinates
inline double magFromRPhiZ(double r, double /* phi */, double z) {
	return std::sqrt(r * r + z * z);
}

/// calculates x from cylindrical coordinates
inline double xFromRPhiZ(double r, double phi, double /* z */) {
	return r * std::cos(phi);
}

/// calculates y from cylindrical coordinates
inline double yFromRPhiZ(double r, double phi, double /* z */) {
	return r * std::sin(phi);
}

/// calculates the polar angle theta from cylindrical coordinates
inline double thetaFromRPhiZ(double r, double /* phi */, double z) {
	return r * std::atan(z / r);
}

/////////////////////////////////////////////////////////////
/// Conversions from spherical to Cartesian coordinates   ///
/////////////////////////////////////////////////////////////

/// calculates the Cartesian position from spherical coordinates
inline Vector3D positionFromRThetaPhi(double r, double theta, double phi) {
	return Vector3D(r * std::cos(phi), r * std::sin(phi), r * std::tan(theta));
}

/// calculates the Cartesian position from spherical coordinates
inline Vector3D positionFromMagThetaPhi(double mag, double theta, double phi) {
	double r = mag * sin(theta);
	return Vector3D(r * std::cos(phi), r * std::sin(phi), mag * std::cos(theta));
}
/// calculates the Cartesian position from spherical coordinates (r, phi, eta)
inline Vector3D positionFromREtaPhi(double ar, double aeta, double aphi) {
	return Vector3D(ar * std::cos(aphi), ar * std::sin(aphi), ar * std::sinh(aeta));
}

/// calculates the pseudorapidity from Cartesian coordinates
inline double etaFromXYZ(const Vector3D& aposition) {
	TVector3 vec(aposition.X, aposition.Y, aposition.Z);
	return vec.Eta();
}


} /* namespace Util */
} /* namespace DDSegmentation */
} /* namespace DD4hep */

#endif /* DDSegmentation_SEGMENTATIONUTIL_H_ */
