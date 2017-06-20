/*
 * Tracker.h
 *
 * Basic reconstruction interface for tracking detectors
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef reconstruction_TRACKER_H_
#define reconstruction_TRACKER_H_

#include "DDRec/API/LayeredSubdetector.h"
#include "DDRec/Extensions/TrackerExtension.h"
#include "DD4hep/DetElement.h"

namespace dd4hep {
namespace rec {

class [[gnu::deprecated(" unmaintained code ")]] Tracker: public LayeredSubdetector {
public:
	Tracker(const DetElement& det) :
	DetElement(det) {
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

} /* namespace rec */
} /* namespace dd4hep */
#endif /* reconstruction_TRACKER_H_ */
