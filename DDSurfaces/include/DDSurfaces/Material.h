/*
 * Material.h
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#ifndef DDSurfaces_MATERIAL_H_
#define DDSurfaces_MATERIAL_H_

namespace DDSurfaces {

/** Container class for material properties used in tracking */
class Material {
public:
	/// Default constructor
	Material(int Z=0, int A=0, double density=0., double radiationLength = 0.) :
			m_Z(Z), m_A(A), m_density(density), m_radiationLength(
					radiationLength) {
	}

	/// Copy constructor
	Material(const Material& material) :
			m_Z(material.Z()), m_A(material.A()), m_density(material.density()), m_radiationLength(
					material.radiationLength()) {
	}

	/// Destructor
	virtual ~Material() {}

	/// Access to the atomic number
	int Z() const {
		return m_Z;
	}

	/// Access to the atomic mass
	int A() const {
		return m_A;
	}

	/// Access to the density
	double density() const {
		return m_density;
	}

	/// Access to the radiation length
	double radiationLength() const {
		// FIXME: needs to calculate X0 to allow lazy initialization
		return m_radiationLength;
	}

protected:
	int m_Z; /// the atomic number
	int m_A; /// the atomic mass
	double m_density; /// the density
	double m_radiationLength; /// the radiation length
};

} /* namespace DDSurfaces */

#endif /* DDSurfaces_MATERIAL_H_ */
