/*
 * ZPlanarSurface.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_ZPLANARSURFACE_H_
#define DDSurfaces_ZPLANARSURFACE_H_

#include "DDSurfaces/ISurface.h"

namespace DDSurfaces {

struct ZPlanarBoundary {
	;
};

class ZPlanarSurface: public ISurface {
public:
	/// Default constructor
	ZPlanarSurface();

	/// Destructor
	virtual ~ZPlanarSurface();

	/// Checks if the given point lies within the surface
	virtual bool isInsideBoundaries(const Vector3D& point) const;

	/// Access to the normal direction at the given point
	virtual Vector3D getNormal(const Vector3D& point) const;

	/// Access to the measurement directions at the given point
	virtual Measurement measurement(const Vector3D& point) const;

protected:
	ZPlanarBoundary m_boundary;
};

} /* namespace DDSurfaces */

#endif /* DDSurfaces_ZPLANARSURFACE_H_ */
