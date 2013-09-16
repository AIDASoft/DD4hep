/*
 * BarrelDetector.h
 *
 *  Created on: Apr 3, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef BARRELDETECTOR_H_
#define BARRELDETECTOR_H_

#include "DD4hep/Detector.h"

namespace DD4hep {

namespace Geometry {
class BarrelDetector: virtual public DetElement {

public:
	BarrelDetector(const DetElement& e) : DetElement(e) {};
	virtual ~BarrelDetector() {}

	bool isBarrel() const {
		return true;
	}
};

} /* namespace Geometry */
} /* namespace DD4hep */
#endif /* BARRELDETECTOR_H_ */
