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
namespace SegmentationUtil {

///////////////////////////////////////////////////////////////////////
///           Conventions                                           ///
/// - x, y, z are the Cartesian coordinates                         ///
/// - r is the magnitude in the xy-plane                            ///
/// - rho is the magnitude                                          ///
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
/// Conversions from Cartesian to cylindrical/spherical coordinates ///
///////////////////////////////////////////////////////////////////////


/// calculates the radius in xyz from Cartesian coordinates
template <typename TYPE> TYPE getRhoFromXYZ(TYPE x, TYPE y, TYPE z) {
	return std::sqrt(x * x + y * y + z * z);
}
/// calculates the radius in xyz from Cartesian coordinates
template <typename TYPE> TYPE getRhoFromXYZ(std::vector<TYPE> position) {
	return getRhoFromXYZ(position[0], position[1], position[2]);
}
/// calculates the radius in xyz from Cartesian coordinates
template <typename TYPE> TYPE getRhoFromXYZ(const std::vector<TYPE>& position) {
	return getRhoFromXYZ(position[0], position[1], position[2]);
}
/// calculates the radius in xyz from Cartesian coordinates
template <typename TYPE> TYPE getRhoFromXYZ(TYPE* position) {
	return getRhoFromXYZ(position[0], position[1], position[2]);
}
/// calculates the radius in xyz from Cartesian coordinates
template <typename TYPE> TYPE getRhoFromXYZ(const TYPE* position) {
	return getRhoFromXYZ(position[0], position[1], position[2]);
}


/// calculates the radius in the xy-plane from Cartesian coordinates
template <typename TYPE> TYPE getRadiusFromXYZ(TYPE x, TYPE y, TYPE z) {
	return std::sqrt(x * x + y * y);
}
/// calculates the radius in the xy-plane from Cartesian coordinates
template <typename TYPE> TYPE getRadiusFromXYZ(std::vector<TYPE> position) {
	return getRadiusFromXYZ(position[0], position[1], position[2]);
}
/// calculates the radius in the xy-plane from Cartesian coordinates
template <typename TYPE> TYPE getRadiusFromXYZ(const std::vector<TYPE>& position) {
	return getRadiusFromXYZ(position[0], position[1], position[2]);
}
/// calculates the radius in the xy-plane from Cartesian coordinates
template <typename TYPE> TYPE getRadiusFromXYZ(TYPE* position) {
	return getRadiusFromXYZ(position[0], position[1], position[2]);
}
/// calculates the radius in the xy-plane from Cartesian coordinates
template <typename TYPE> TYPE getRadiusFromXYZ(const TYPE* position) {
	return getRadiusFromXYZ(position[0], position[1], position[2]);
}


/// calculates the polar angle theta from Cartesian coordinates
template <typename TYPE> TYPE getThetaFromXYZ(TYPE x, TYPE y, TYPE z) {
	TYPE r = getRadiusFromXYZ(x, y, z);
	return std::acos(z / r);
}
/// calculates the polar angle theta from Cartesian coordinates
template <typename TYPE> TYPE getThetaFromXYZ(std::vector<TYPE> position) {
	return getThetaFromXYZ(position[0], position[1], position[2]);
}
/// calculates the polar angle theta from Cartesian coordinates
template <typename TYPE> TYPE getThetaFromXYZ(const std::vector<TYPE>& position) {
	return getThetaFromXYZ(position[0], position[1], position[2]);
}
/// calculates the polar angle theta from Cartesian coordinates
template <typename TYPE> TYPE getThetaFromXYZ(TYPE* position) {
	return getThetaFromXYZ(position[0], position[1], position[2]);
}
/// calculates the polar angle theta from Cartesian coordinates
template <typename TYPE> TYPE getThetaFromXYZ(const TYPE* position) {
	return getThetaFromXYZ(position[0], position[1], position[2]);
}


/// calculates the azimuthal angle phi from Cartesian coordinates
template <typename TYPE> TYPE getPhiFromXYZ(TYPE x, TYPE y, TYPE z) {
	return std::atan2(y, x);
}
/// calculates the azimuthal angle phi from Cartesian coordinates
template <typename TYPE> TYPE getPhiFromXYZ(std::vector<TYPE> position) {
	return getPhiFromXYZ(position[0], position[1], position[2]);
}
/// calculates the azimuthal angle phi from Cartesian coordinates
template <typename TYPE> TYPE getPhiFromXYZ(const std::vector<TYPE>& position) {
	return getPhiFromXYZ(position[0], position[1], position[2]);
}
/// calculates the azimuthal angle phi from Cartesian coordinates
template <typename TYPE> TYPE getPhiFromXYZ(TYPE* position) {
	return getPhiFromXYZ(position[0], position[1], position[2]);
}
/// calculates the azimuthal angle phi from Cartesian coordinates
template <typename TYPE> TYPE getPhiFromXYZ(const TYPE* position) {
	return getPhiFromXYZ(position[0], position[1], position[2]);
}


/////////////////////////////////////////////////////////////
/// Conversions from cylindrical to Cartesian coordinates ///
/////////////////////////////////////////////////////////////

/// calculates the Cartesian position from cylindrical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRPhiZ(TYPE r, TYPE phi, TYPE z) {
	std::vector<double> position(3);
	position[0] = getXFromRPhiZ(r, phi, z);
	position[1] = getYFromRPhiZ(r, phi, z);
	position[2] = z;
	return position;
}
/// calculates the Cartesian position from cylindrical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRPhiZ(std::vector<TYPE> position) {
	return getPositionFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from cylindrical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRPhiZ(const std::vector<TYPE>& position) {
	return getPositionFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from cylindrical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRPhiZ(TYPE* position) {
	return getPositionFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from cylindrical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRPhiZ(const TYPE* position) {
	return getPositionFromRPhiZ(position[0], position[1], position[2]);
}


/// calculates the radius in xyz from cylindrical coordinates
template <typename TYPE> TYPE getRhoFromRPhiZ(TYPE r, TYPE phi, TYPE z) {
	return std::sqrt(r * r + z * z);
}
/// calculates the radius in xyz from cylindrical coordinates
template <typename TYPE> TYPE getRhoFromRPhiZ(std::vector<TYPE> position) {
	return getRhoFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the radius in xyz from cylindrical coordinates
template <typename TYPE> TYPE getRhoFromRPhiZ(const std::vector<TYPE>& position) {
	return getRhoFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the radius in xyz from cylindrical coordinates
template <typename TYPE> TYPE getRhoFromRPhiZ(TYPE* position) {
	return getRhoFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the radius in xyz from cylindrical coordinates
template <typename TYPE> TYPE getRhoFromRPhiZ(const TYPE* position) {
	return getRhoFromRPhiZ(position[0], position[1], position[2]);
}


/// calculates x from cylindrical coordinates
template <typename TYPE> TYPE getXFromRPhiZ(TYPE r, TYPE phi, TYPE z) {
	return r * std::cos(phi);
}
/// calculates x from cylindrical coordinates
template <typename TYPE> TYPE getXFromRPhiZ(std::vector<TYPE> position) {
	return getXFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates x from cylindrical coordinates
template <typename TYPE> TYPE getXFromRPhiZ(const std::vector<TYPE>& position) {
	return getXFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates x from cylindrical coordinates
template <typename TYPE> TYPE getXFromRPhiZ(TYPE* position) {
	return getXFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates x from cylindrical coordinates
template <typename TYPE> TYPE getXFromRPhiZ(const TYPE* position) {
	return getXFromRPhiZ(position[0], position[1], position[2]);
}


/// calculates y from cylindrical coordinates
template <typename TYPE> TYPE getYFromRPhiZ(TYPE r, TYPE phi, TYPE z) {
	return r * std::sin(phi);
}
/// calculates y from cylindrical coordinates
template <typename TYPE> TYPE getYFromRPhiZ(std::vector<TYPE> position) {
	return getYFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates y from cylindrical coordinates
template <typename TYPE> TYPE getYFromRPhiZ(const std::vector<TYPE>& position) {
	return getYFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates y from cylindrical coordinates
template <typename TYPE> TYPE getYFromRPhiZ(TYPE* position) {
	return getYFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates y from cylindrical coordinates
template <typename TYPE> TYPE getYFromRPhiZ(const TYPE* position) {
	return getYFromRPhiZ(position[0], position[1], position[2]);
}


/// calculates the polar angle theta from cylindrical coordinates
template <typename TYPE> TYPE getThetaFromRPhiZ(TYPE r, TYPE phi, TYPE z) {
	return r * std::atan(z / r);
}
/// calculates the polar angle theta from cylindrical coordinates
template <typename TYPE> TYPE getThetaFromRPhiZ(std::vector<TYPE> position) {
	return getThetaFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the polar angle theta from cylindrical coordinates
template <typename TYPE> TYPE getThetaFromRPhiZ(const std::vector<TYPE>& position) {
	return getThetaFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the polar angle theta from cylindrical coordinates
template <typename TYPE> TYPE getThetaFromRPhiZ(TYPE* position) {
	return getThetaFromRPhiZ(position[0], position[1], position[2]);
}
/// calculates the polar angle theta from cylindrical coordinates
template <typename TYPE> TYPE getThetaFromRPhiZ(const TYPE* position) {
	return getThetaFromRPhiZ(position[0], position[1], position[2]);
}


/////////////////////////////////////////////////////////////
/// Conversions from spherical to Cartesian coordinates   ///
/////////////////////////////////////////////////////////////

/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRThetaPhi(TYPE r, TYPE theta, TYPE phi) {
	std::vector<double> position(3);
	position[0] = r * std::cos(phi);
	position[1] = r * std::sin(phi);
	position[2] = r * std::tan(theta);
	return position;
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRThetaPhi(std::vector<TYPE> position) {
	return getPositionFromRThetaPhi(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRThetaPhi(const std::vector<TYPE>& position) {
	return getPositionFromRThetaPhi(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRThetaPhi(TYPE* position) {
	return getPositionFromRThetaPhi(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRThetaPhi(const TYPE* position) {
	return getPositionFromRThetaPhi(position[0], position[1], position[2]);
}


/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRhoThetaPhi(TYPE rho, TYPE theta, TYPE phi) {
	std::vector<double> position(3);
	double r = rho * sin(theta);
	position[0] = r * std::cos(phi);
	position[1] = r * std::sin(phi);
	position[2] = rho * std::cos(theta);
	return position;
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRhoThetaPhi(std::vector<TYPE> position) {
	return getPositionFromRhoThetaPhi(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRhoThetaPhi(const std::vector<TYPE>& position) {
	return getPositionFromRhoThetaPhi(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRhoThetaPhi(TYPE* position) {
	return getPositionFromRhoThetaPhi(position[0], position[1], position[2]);
}
/// calculates the Cartesian position from spherical coordinates
template <typename TYPE> std::vector<TYPE> getPositionFromRhoThetaPhi(const TYPE* position) {
	return getPositionFromRhoThetaPhi(position[0], position[1], position[2]);
}


} /* namespace SegmentationUtil */
} /* namespace DDSegmentation */
} /* namespace DD4hep */

#endif /* DDSegmentation_SEGMENTATIONUTIL_H_ */
