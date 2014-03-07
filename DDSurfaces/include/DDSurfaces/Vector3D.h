/*
 * Vector3D.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_VECTOR3D_H_
#define DDSurfaces_VECTOR3D_H_

namespace DDSurfaces {

/** Generic vector class */
class Vector3D {
public:
	/// Default constructor
	Vector3D(double x = 0., double y = 0., double z = 0.) :
		m_x(x), m_y(y), m_z(z) {
	}

	/** Copy constructor from arbitrary foreign vector class
	 *  Requires x(), y(), z() methods to access Cartesian coordinates.
	 */
	template<typename T> Vector3D(const T& vector) :
		m_x(vector.x()), m_y(vector.y()), m_z(vector.z()) {
	}

	/// Destructor
	virtual ~Vector3D() {}

	/// Access to x
	double x() const {
		return m_x;
	}

	/// Access to y
	double y() const {
		return m_y;
	}

	/// Access to z
	double z() const {
		return m_z;
	}

protected:
	double m_x;
	double m_y;
	double m_z;
};

} /* namespace DDSurfaces */

#endif /* VECTOR3D_H_ */
