/*
 * Subdetector.h
 *
 * Basic reconstruction interface for generic subdetectors
 *
 *  Created on: Apr 3, 2014
 *      Author: Christian Grefe, CERN
 */

#ifndef SUBDETECTOR_H_
#define SUBDETECTOR_H_

#include "DD4hep/Detector.h"
#include "DDRec/Extensions/SubdetectorExtension.h"

namespace DD4hep {
namespace DDRec {

class Subdetector: public virtual Geometry::DetElement {
public:
	/// Default constructor
	Subdetector(const Geometry::DetElement& det) :
		Geometry::DetElement(det) {
		getSubdetectorExtension();
	}

	/// Destructor
	virtual ~Subdetector() {
		// does not own the extension!
	}

	/// Is this a barrel detector
	bool isBarrel() const {
		return _subdetector->isBarrel();
	}

	/// Is this an endcap detector
	bool isEndcap() const {
		return _subdetector->isEndcap();
	}

	/// Access to the inner radius
	double getRMin() const {
		return _subdetector->getRMin();
	}

	/// Access to the outer radius
	double getRMax() const {
		return _subdetector->getRMax();
	}

	/// Access to the lower z extent
	double getZMin() const {
		return _subdetector->getZMin();
	}

	/// Access to the upper z extent
	double getZMax() const {
		return _subdetector->getZMax();
	}

	/// Access to the number of sides
	/* Describes the number of corners for a polygon.
	 * Returns 0 in case of a circular shape
	 */
	int getNSides() const {
		return _subdetector->getNSides();
	}

protected:
	SubdetectorExtension* _subdetector;

private:
	/// Helper method to retrieve the extension
	void getSubdetectorExtension() {
		_subdetector = this->isValid() ? this->extension<SubdetectorExtension>() : 0;
		if (not _subdetector) {
			throw invalid_detector_element("Found no extension of type \"SubdetectorExtension\"", Geometry::DetElement(*this));
		}
	}
};

} /* namespace DDRec */
} /* namespace DD4hep */

#endif /* SUBDETECTOR_H_ */
