/*
 * ISurface.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_ISURFACE_H_
#define DDSurfaces_ISURFACE_H_

#include "DDSurfaces/Material.h"
#include "DDSurfaces/Measurement.h"
#include "DDSurfaces/Vector3D.h"

namespace DDSurfaces {

/** Generic surface class used for tracking */
class ISurface {
public:
	/// Destructor
	virtual ~ISurface() {
		if (m_measurement) {
			delete m_measurement;
		}
	}

	/// Checks if the surface has measurement directions
	bool hasMeasurement() const {
		return m_measurement != 0;
	}

	/// Checks if the given point lies within the surface
	bool isInsideBoundaries(const Vector3D& point) const = 0;

	/// Access to the normal direction at the given point
	Vector3D getNormal(const Vector3D& point) const = 0;

	/// Access to the measurement directions at the given point
	Measurement measurement(const Vector3D& point) const = 0;

	/// Access to the material in opposite direction of the normal
	const Material& innerMaterial() const {
		return m_innerMaterial;
	}

	/// Access to the material in direction of the normal
	const Material& outerMaterial() const {
		return m_outerMaterial;
	}

	/// Sets the material in opposite direction of the normal
	void setInnerMaterial(const Material& material) {
		m_innerMaterial = material;
	}

	/// Sets the material in direction of the normal
	void setOuterMaterial(const Material& material) {
		m_outerMaterial = material;
	}

	/// Sets the nominal measurement directions
	void setMeasurement(const Measurement& measurement) {
		m_measurement = new Measurement(measurement);
	}

protected:
	/// Constructor which can be used by derived classes
	ISurface(const Material& innerMaterial=Material(), const Material& outerMaterial=Material()) :
		m_innerMaterial(innerMaterial), m_outerMaterial(outerMaterial), m_measurement(0) {
	}

	Material m_innerMaterial; /// the material in opposite direction of the normal
	Material m_outerMaterial; /// the material in direction of the normal
	Measurement* m_measurement; /// the nominal measurement directions
};

} /* namespace DDSurfaces */

#endif /* DDSurfaces_ISURFACE_H_ */
