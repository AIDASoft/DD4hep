/*
 * BarrelDetector.h
 *
 *  Created on: Apr 3, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef BARRELDETECTOR_H_
#define BARRELDETECTOR_H_

#include "DD4hep/Detector.h"

namespace DD4hep {

  namespace DDRec {
    class BarrelDetector: virtual public Geometry::DetElement {

    public:
      typedef Geometry::DetElement DetElement;
      BarrelDetector(const DetElement& e) : DetElement(e) {};
      virtual ~BarrelDetector() {}

      bool isBarrel() const {
	return true;
      }
    };

  } /* namespace DDRec */
} /* namespace DD4hep */
#endif /* BARRELDETECTOR_H_ */
