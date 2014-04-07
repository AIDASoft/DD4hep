#define OBSOLETE
#ifndef OBSOLETE
/*
 * StraightLineSurface.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#include "DDSurfaces/StraightLineSurface.h"

namespace DDSurfaces {

StraightLineSurface::StraightLineSurface() {
	// TODO Auto-generated constructor stub

}

StraightLineSurface::~StraightLineSurface() {
	// TODO Auto-generated destructor stub
}

/// Checks if the given point lies within the surface
bool StraightLineSurface::isInsideBoundaries(const Vector3D& point) const {
	// TODO
	return false;
}

/// Access to the normal direction at the given point
Vector3D StraightLineSurface::getNormal(const Vector3D& point) const {
	// TODO
	return Vector3D();

}

/// Access to the measurement directions at the given point
MeasurementDirections StraightLineSurface::measurement(const Vector3D& point) const {
	// TODO
	return MeasurementDirections();
}

} /* namespace DDSurfaces */

#endif
