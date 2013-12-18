/*
 * SegmentationParameter.h
 *
 * Helper class to hold a segmentation parameter with its description.
 *
 *  Created on: Dec 16, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_SEGMENTATIONPARAMETER_H_
#define DDSegmentation_SEGMENTATIONPARAMETER_H_

#include <sstream>
#include <string>

namespace DD4hep {
namespace DDSegmentation {

/// Class to hold a segmentation parameter with its description
class SegmentationParameter {
public:
	/// Default constructor
	SegmentationParameter(const std::string& name, const std::string& description, double& parameter,
			const double& defaultValue, bool isOptional = false) :
			_name(name), _description(description), _value(parameter), _defaultValue(defaultValue), _isOptional(
					isOptional) {
		_value = defaultValue;
	}
	/// Destructor
	virtual ~SegmentationParameter() {
	}
	/// Access to the parameter name
	const std::string& name() const {
		return _name;
	}
	/// Access to the parameter description
	const std::string& description() const {
		return _description;
	}
	/// Access to the parameter value
	double& value() {
		return _value;
	}
	/// Access to the parameter value
	double value() const {
		return _value;
	}
	/// Set the parameter value
	void setValue(const double& value) {
		_value = value;
	}
	/// Access to the parameter default value
	const double& defaultValue() const {
		return _defaultValue;
	}
	/// Check if this parameter is optional
	bool isOptional() const {
		return _isOptional;
	}
	/// Printable version
	std::string toString() const {
		std::stringstream s;
		s << _name << " = " << _value;
		if (not _description.empty()) {
			s << " (" << _description << ")";
		}
		return s.str();
	}
protected:
	/// The parameter name
	std::string _name;
	/// The parameter description
	std::string _description;
	/// The parameter value
	double& _value;
	/// The parameter default value
	double _defaultValue;
	/// Store if parameter is optional
	bool _isOptional;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATIONPARAMETER_H_ */
