/*
 * TrackerExtension.h
 *
 * Abstract extension used by the Tracker class.
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDRec_TRACKEREXTENSION_H_
#define DDRec_TRACKEREXTENSION_H_

namespace DD4hep {
namespace DDRec {

class TrackerExtension {
public:
	TrackerExtension();
	virtual ~TrackerExtension();
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* DDRec_TRACKEREXTENSION_H_ */
