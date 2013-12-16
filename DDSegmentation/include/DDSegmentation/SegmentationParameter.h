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

#include <string>

namespace DD4hep {
namespace DDSegmentation {

/// Class to hold a segmentation parameter with its description
class SegmentationParameter {
public:
	/// Default constructor
	SegmentationParameter(const std::string& name, const std::string& description, double& parameter,
			double defaultValue = 0., bool isOptional = false);
	/// Destructor
	virtual ~SegmentationParameter() {};
	/// Access to the parameter name
	const std::string& name() const;
	/// Access to the parameter description
	const std::string& description() const;
	/// Access to the parameter value
	double& value();
	/// Access to the parameter value
	double value() const;
	/// Check if this parameter is optional
	bool isOptional() const;
	/// Printable version
	std::string toString() const;
protected:
	/// The parameter name
	std::string _name;
	/// The parameter description
	std::string _description;
	/// The parameter value
	double& _parameter;
	/// The parameter default value
	double _defaultValue;
	/// Store if parameter is optional
	bool _isOptional;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATIONPARAMETER_H_ */
