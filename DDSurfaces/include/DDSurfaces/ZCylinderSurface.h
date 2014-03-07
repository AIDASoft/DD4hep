/*
 * ZCylinderSurface.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_ZCYLINDERSURFACE_H_
#define DDSurfaces_ZCYLINDERSURFACE_H_

#include "DDSurfaces/ISurface.h"

namespace DDSurfaces {

class ZCylinderBoundary;

class ZCylinderSurface: public ISurface {
public:
	/// Default constructor
	ZCylinderSurface();

	/// Destructor
	virtual ~ZCylinderSurface();

	/// Checks if the given point lies within the surface
	bool isInsideBoundaries(const Vector3D& point) const;

	/// Access to the normal direction at the given point
	Vector3D getNormal(const Vector3D& point) const;

	/// Access to the measurement directions at the given point
	Measurement measurement(const Vector3D& point) const;

protected:
	ZCylinderBoundary m_boundary;
};

} /* namespace DDSurfaces */

#endif /* DDSurfaces_ZCYLINDERSURFACE_H_ */
