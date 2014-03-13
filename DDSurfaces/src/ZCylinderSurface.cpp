/*
 * ZCylinderSurface.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#include "DDSurfaces/ZCylinderSurface.h"

namespace DDSurfaces {

ZCylinderSurface::ZCylinderSurface() {
	// TODO Auto-generated constructor stub

}

ZCylinderSurface::~ZCylinderSurface() {
	// TODO Auto-generated destructor stub
}

/// Checks if the given point lies within the surface
bool ZCylinderSurface::isInsideBoundaries(const Vector3D& point) const {
	// TODO
	return false;
}

/// Access to the normal direction at the given point
Vector3D ZCylinderSurface::getNormal(const Vector3D& point) const {
	// TODO
	return Vector3D();

}

/// Access to the measurement directions at the given point
MeasurementDirections ZCylinderSurface::measurement(const Vector3D& point) const {
	// TODO
	return MeasurementDirections();
}

} /* namespace DDSurfaces */
