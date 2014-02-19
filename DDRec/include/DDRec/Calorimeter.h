/*
 * Calorimeter.h
 *
 *  Created on: Mar 27, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CALORIMETER_H_
#define CALORIMETER_H_

#include "DD4hep/Detector.h"
#include "LayeredSubdetector.h"

namespace DD4hep {

namespace DDRec {

class Calorimeter: public LayeredSubdetector {

public:
	Calorimeter(const DetElement& e) : DetElement(e), LayeredSubdetector(e) {}
	virtual ~Calorimeter() {}

	virtual bool isCalorimeter() const {
		return true;
	}
};
}
} /* namespace DD4hep */
#endif /* CALORIMETER_H_ */
