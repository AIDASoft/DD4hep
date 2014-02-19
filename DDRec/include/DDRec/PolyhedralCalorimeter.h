/*
 * PolyhedralCalorimeter.h
 *
 *  Created on: Apr 17, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef POLYHEDRALCALORIMETER_H_
#define POLYHEDRALCALORIMETER_H_

#include "PolyhedralDetector.h"
#include "Calorimeter.h"

namespace DD4hep {
namespace DDRec {

class PolyhedralCalorimeter: public PolyhedralDetector, public Calorimeter {
public:
	PolyhedralCalorimeter(const DetElement& e) : DetElement(e), Calorimeter(e), PolyhedralDetector(e) {}
	virtual ~PolyhedralCalorimeter() {}
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* POLYHEDRALCALORIMETER_H_ */
