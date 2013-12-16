/*
 * SegmentationParameter.cpp
 *
 *  Created on: Dec 16, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/SegmentationParameter.h"

#include <sstream>

namespace DD4hep {
namespace DDSegmentation {

using std::string;
using std::stringstream;

/// Default constructor
SegmentationParameter::SegmentationParameter(const string& name, const string& description, double& parameter,
		double defaultValue, bool isOptional) :
		_name(name), _description(description), _parameter(parameter), _defaultValue(defaultValue), _isOptional(
				isOptional) {
	_parameter = defaultValue;
}

/// Access to the parameter name
const string& SegmentationParameter::name() const {
	return _name;
}

/// Access to the parameter description
const string& SegmentationParameter::SegmentationParameter::description() const {
	return _description;
}

/// Access to the parameter value
double& SegmentationParameter::value() {
	return _parameter;
}

/// Access to the parameter value
double SegmentationParameter::value() const {
	return _parameter;
}

/// Check if this parameter is optional
bool SegmentationParameter::isOptional() const {
	return _isOptional;
}

/// Printable version
string SegmentationParameter::toString() const {
	stringstream s;
	s << _name << " = " << _parameter;
	if (not _description.empty()) {
		s << " (" << _description << ")";
	}
	return s.str();
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
