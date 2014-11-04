/*
 * Subdetector.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: cgrefe
 */

#include "DDRec/API/Subdetector.h"

namespace DD4hep {
namespace DDRec {

bool Subdetector::isBarrel() const {
	return true;
}

bool Subdetector::isEndcap() const {
	return false;
}

double Subdetector::getRMin() const {
	return 100.;
}

double Subdetector::getRMax() const {
	return 200.;
}

double Subdetector::getZMin() const {
	return 300.;
}

double Subdetector::getZMax() const {
	return 400.;
}

int Subdetector::getNSides() const {
	return 8;
}

} /* namespace DDRec */
} /* namespace DD4hep */
