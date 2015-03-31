/*
 * SegmentationFactory.cpp
 *
 *  Created on: Dec 15, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/SegmentationFactory.h"

namespace DD4hep {
namespace DDSegmentation {

using std::map;
using std::vector;

/// Default constructor. Takes the class name as argument and takes care of registration with the factory
SegmentationCreatorBase::SegmentationCreatorBase(const std::string& name) {
	SegmentationFactory::instance()->registerSegmentation(name, this);
}

/// Initialize the global factory instance
SegmentationFactory* SegmentationFactory::_instance = 0;

/// Access to the global factory instance
SegmentationFactory* SegmentationFactory::instance() {
	if (not _instance) {
		_instance = new SegmentationFactory();
	}
	return _instance;
}

/// Create a new segmentation object with the given type name. Returns NULL if type name is unknown.
Segmentation* SegmentationFactory::create(const std::string& name, const std::string& identifier) const {
	map<std::string, SegmentationCreatorBase*>::const_iterator it;
	it = _segmentations.find(name);
	if (it != _segmentations.end()) {
		return it->second->create(identifier);
	}
	return 0;
}

/// Access to the list of registered segmentations
vector<std::string> SegmentationFactory::registeredSegmentations() const {
	vector<std::string> segmentationNames;
	map<std::string, SegmentationCreatorBase*>::const_iterator it;
	for (it = _segmentations.begin(); it != _segmentations.end(); ++ it) {
		segmentationNames.push_back(it->first);
	}
	return segmentationNames;
}

/// Registers a new SegmentationCreator with the factory
void SegmentationFactory::registerSegmentation(const std::string& name, SegmentationCreatorBase* creator) {
	_segmentations[name] = creator;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
