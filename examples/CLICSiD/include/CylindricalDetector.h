/*
 * CylindricalDetector.h
 *
 *  Created on: Apr 16, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CYLINDRICALDETECTOR_H_
#define CYLINDRICALDETECTOR_H_

#include "DD4hep/Detector.h"
#include "DD4hep/Shapes.h"

namespace DD4hep {
namespace Geometry {

class CylindricalDetector: virtual public DetElement {
public:
	CylindricalDetector(const DetElement& e) : DetElement(e) {}
	virtual ~CylindricalDetector() {}

	double getRMin() const {
		return getCylinder()->GetRmin();
	}
	double getRMax() const {
		return getCylinder()->GetRmax();
	}
	double getZMin() const {
		return -getCylinder()->GetDZ() / 2.;
	}
	double getZMax() const {
		return getCylinder()->GetDZ() / 2.;
	}

protected:
	 Tube getCylinder() const {
		return Tube(volume().solid());
	}
};

} /* namespace Geometry */
} /* namespace DD4hep */
#endif /* CYLINDRICALDETECTOR_H_ */
