/*
 * EndcapDetector.h
 *
 *  Created on: Apr 16, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef ENDCAPDETECTOR_H_
#define ENDCAPDETECTOR_H_

#include "DD4hep/Detector.h"

namespace DD4hep {
namespace Geometry {

class EndcapDetector: virtual public DetElement {
public:
	EndcapDetector(const DetElement& e) : DetElement(e) {}
	virtual ~EndcapDetector() {}

	bool isEndcap() const {
		return true;
	}
};

} /* namespace Geometry */
} /* namespace DD4hep */
#endif /* ENDCAPDETECTOR_H_ */
