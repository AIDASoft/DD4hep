/*
 * PolyhedralEndcapCalorimeter.h
 *
 *  Created on: Apr 16, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef POLYHEDRALENDCAPCALORIMETER_H_
#define POLYHEDRALENDCAPCALORIMETER_H_

#include "EndcapDetector.h"
#include "PolyhedralCalorimeter.h"

namespace DD4hep {
namespace DDRec {

class PolyhedralEndcapCalorimeter: public EndcapDetector, public PolyhedralCalorimeter {
public:
	PolyhedralEndcapCalorimeter(const DetElement& e) : DetElement(e), EndcapDetector(e), PolyhedralCalorimeter(e) {}
	virtual ~PolyhedralEndcapCalorimeter() {}
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* POLYHEDRALENDCAPCALORIMETER_H_ */
