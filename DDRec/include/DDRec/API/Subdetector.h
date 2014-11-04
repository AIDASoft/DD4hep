/*
 * Subdetector.h
 *
 * Basic reconstruction interface for generic subdetectors
 *
 *  Created on: Apr 3, 2014
 *      Author: Christian Grefe, CERN
 */

#ifndef SUBDETECTOR_H_
#define SUBDETECTOR_H_

#include "DD4hep/Detector.h"

namespace DD4hep {
namespace DDRec {

class Subdetector: public virtual Geometry::DetElement {
public:
	Subdetector(const Geometry::DetElement& det) :
		Geometry::DetElement(det) {

	}
	virtual ~Subdetector() {
		// nothing to do
	}

	bool isBarrel() const;

	bool isEndcap() const;

	double getRMin() const;

	double getRMax() const;

	double getZMin() const;

	double getZMax() const;

	int getNSides() const;
};

} /* namespace DDRec */
} /* namespace DD4hep */

#endif /* SUBDETECTOR_H_ */
