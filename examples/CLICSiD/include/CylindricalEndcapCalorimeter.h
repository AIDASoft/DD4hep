/*
 * CylindricalEndcapCalorimeter.h
 *
 *  Created on: Apr 17, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CYLINDRICALENDCAPCALORIMETER_H_
#define CYLINDRICALENDCAPCALORIMETER_H_

#include "CylindricalCalorimeter.h"
#include "EndcapDetector.h"

namespace DD4hep {
namespace Geometry {

class CylindricalEndcapCalorimeter: public CylindricalCalorimeter, public EndcapDetector {
public:
	CylindricalEndcapCalorimeter(const DetElement& e) : DetElement(e), CylindricalCalorimeter(e), EndcapDetector(e) {}
	virtual ~CylindricalEndcapCalorimeter() {}
};

} /* namespace Geometry */
} /* namespace DD4hep */
#endif /* CYLINDRICALENDCAPCALORIMETER_H_ */
