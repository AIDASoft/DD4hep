/*
 * Calorimeter.h
 *
 * Basic reconstruction interface for calorimeters
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDRec_CALORIMETER_H_
#define DDRec_CALORIMETER_H_

#include "DDRec/API/LayeredSubdetector.h"
#include "DDRec/API/Subdetector.h"

namespace DD4hep {
namespace DDRec {

class Calorimeter: public LayeredSubdetector, public Subdetector {
public:
	Calorimeter(const Geometry::DetElement& det) :
		Geometry::DetElement(det), LayeredSubdetector(det), Subdetector(det) {

	}

	virtual ~Calorimeter() {}
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* DDRec_CALORIMETER_H_ */
