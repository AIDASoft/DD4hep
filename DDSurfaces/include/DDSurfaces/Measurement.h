/*
 * Measurement.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_MEASUREMENT_H_
#define DDSurfaces_MEASUREMENT_H_

#include "DDSurfaces/Vector3D.h"

namespace DDSurfaces {

/** Container class for measurement directions */
class Measurement {
public:
	/// Default constructor
	Measurement(const Vector3D& u = Vector3D(), const Vector3D& v = Vector3D()) :
		m_u(u), m_v(v) {
	}

	/// Copy constructor
	Measurement(const Measurement& measurement) :
		m_u(measurement.u()), m_v(measurement.v()) {
	}

	/// Destructor
	virtual ~Measurement() {}

	/// Access to the U measurement direction
	const Vector3D& u() const {
		return m_u;
	}

	/// Access to the V measurement direction
	const Vector3D& v() const {
		return m_v;
	}

protected:
	Vector3D m_u;
	Vector3D m_v;
};

} /* namespace DDSurfaces */

#endif /* DDSurfaces_MEASUREMENT_H_ */
