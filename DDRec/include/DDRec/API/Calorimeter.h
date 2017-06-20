/*
 * Calorimeter.h
 *
 * Basic reconstruction interface for calorimeters
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef rec_CALORIMETER_H_
#define rec_CALORIMETER_H_

#include "DDRec/API/LayeredSubdetector.h"
#include "DDRec/API/Subdetector.h"

namespace dd4hep {
namespace rec {

class Calorimeter: public LayeredSubdetector, public Subdetector {
public:
	Calorimeter(const DetElement& det) :
		DetElement(det), LayeredSubdetector(det), Subdetector(det) {

	}

	virtual ~Calorimeter() {}
};

} /* namespace rec */
} /* namespace dd4hep */
#endif /* rec_CALORIMETER_H_ */
