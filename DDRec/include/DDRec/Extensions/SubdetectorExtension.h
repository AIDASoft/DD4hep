/*
 * LayeringExtension.h
 *
 * Abstract extension used by the LayeredSubdetector class.
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDRec_SUBDETECTOREXTENSION_H_
#define DDRec_SUBDETECTOREXTENSION_H_

#include "DD4hep/Detector.h"

namespace DD4hep {
namespace DDRec {

/**
 * Class describing general parameters of a subdetector.
 */
class  [[gnu::deprecated(" unmaintained code ")]] SubdetectorExtension {
public:
	/// Destructor
	virtual ~SubdetectorExtension() {
	}

	/// Is this a barrel detector
	virtual bool isBarrel() const = 0;

	/// Is this an endcap detector
	virtual bool isEndcap() const = 0;

	/// Access to the inner radius
	virtual double getRMin() const = 0;

	/// Access to the outer radius
	virtual double getRMax() const = 0;

	/// Access to the lower z extent
	virtual double getZMin() const = 0;

	/// Access to the upper z extent
	virtual double getZMax() const = 0;

	/// Access to the number of sides
	/* Describes the number of corners for a polygon.
	 * Returns 0 in case of a circular shape
	 */
	virtual int getNSides() const = 0;

protected:
	SubdetectorExtension() {
	}
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* DDRec_SUBDETECTOREXTENSION_H_ */
