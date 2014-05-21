/*
 * PolyhedralBarrelCalorimeter.h
 *
 *  Created on: May 19, 2014
 *      Author: Nikiforos Nikiforou, CERN
 */

#ifndef SECTORBARRELCALORIMETER_H_
#define SECTORBARRELCALORIMETER_H_

#include "BarrelDetector.h"
#include "PolyhedralCalorimeter.h"

namespace DD4hep {
namespace Geometry {

class SectorBarrelCalorimeter: public BarrelDetector, public PolyhedralCalorimeter {
public:
	SectorBarrelCalorimeter(const DetElement& e) : DetElement(e), BarrelDetector(e), PolyhedralCalorimeter(e) {}
	virtual ~SectorBarrelCalorimeter() {}
};

} /* namespace Geometry */
} /* namespace DD4hep */
#endif /* SECTORBARRELCALORIMETER_H_ */
