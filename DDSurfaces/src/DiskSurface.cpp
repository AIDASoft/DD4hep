/*
 * DiskSurface.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#include "DDSurfaces/DiskSurface.h"

namespace DDSurfaces {

DiskSurface::DiskSurface() {
	// TODO Auto-generated constructor stub

}

DiskSurface::~DiskSurface() {
	// TODO Auto-generated destructor stub
}

/// Checks if the given point lies within the surface
bool DiskSurface::isInsideBoundaries(const Vector3D& point) const {
	// TODO
	return false;
}

/// Access to the normal direction at the given point
Vector3D DiskSurface::getNormal(const Vector3D& point) const {
	// TODO
	return Vector3D();

}

/// Access to the measurement directions at the given point
MeasurementDirections DiskSurface::measurement(const Vector3D& point) const {
	// TODO
	return MeasurementDirections();
}

} /* namespace DDSurfaces */
