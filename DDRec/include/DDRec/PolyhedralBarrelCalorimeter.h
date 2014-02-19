/*
 * PolyhedralBarrelCalorimeter.h
 *
 *  Created on: Apr 3, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef POLYHEDRALBARRELCALORIMETER_H_
#define POLYHEDRALBARRELCALORIMETER_H_

#include "BarrelDetector.h"
#include "PolyhedralCalorimeter.h"

namespace DD4hep {
namespace DDRec {

class PolyhedralBarrelCalorimeter: public BarrelDetector, public PolyhedralCalorimeter {
public:
	PolyhedralBarrelCalorimeter(const DetElement& e) : DetElement(e), BarrelDetector(e), PolyhedralCalorimeter(e) {}
	virtual ~PolyhedralBarrelCalorimeter() {}
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* POLYHEDRALBARRELCALORIMETER_H_ */
