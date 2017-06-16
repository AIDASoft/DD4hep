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

namespace dd4hep {
namespace detail {

class SectorBarrelCalorimeter: public BarrelDetector, public PolyhedralCalorimeter {
public:
	SectorBarrelCalorimeter(const DetElement& e) : DetElement(e), BarrelDetector(e), PolyhedralCalorimeter(e) {}
	virtual ~SectorBarrelCalorimeter() {}
};

} /* namespace detail */
} /* namespace dd4hep */
#endif /* SECTORBARRELCALORIMETER_H_ */
