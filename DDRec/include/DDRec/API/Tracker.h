/*
 * Tracker.h
 *
 * Basic reconstruction interface for tracking detectors
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDReconstruction_TRACKER_H_
#define DDReconstruction_TRACKER_H_

#include "DDRec/API/LayeredSubdetector.h"
#include "DDRec/Extensions/TrackerExtension.h"
#include "DD4hep/Detector.h"

namespace DD4hep {
namespace DDRec {

class  [[gnu::deprecated(" unmaintained code ")]] Tracker: public LayeredSubdetector {
public:
	Tracker(const Geometry::DetElement& det) :
	Geometry::DetElement(det) {
		getTrackerExtension();
	}

	virtual ~Tracker() {
		// does not own the extension!
	}

protected:
	TrackerExtension* _tracker;

private:
	void getTrackerExtension() {
		_tracker = this->isValid() ? this->extension<TrackerExtension>() : 0;
	}
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* DDReconstruction_TRACKER_H_ */
