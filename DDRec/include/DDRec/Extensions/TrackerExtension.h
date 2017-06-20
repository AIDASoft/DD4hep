/*
 * TrackerExtension.h
 *
 * Abstract extension used by the Tracker class.
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef rec_TRACKEREXTENSION_H_
#define rec_TRACKEREXTENSION_H_

namespace dd4hep {
namespace rec {

class [[gnu::deprecated(" unmaintained code ")]] TrackerExtension {
public:
	TrackerExtension();
	virtual ~TrackerExtension();
};

} /* namespace rec */
} /* namespace dd4hep */
#endif /* rec_TRACKEREXTENSION_H_ */
