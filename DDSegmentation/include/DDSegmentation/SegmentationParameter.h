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
#include <typeinfo>

namespace DD4hep {
namespace DDSegmentation {

/// Helper class to extract type names
template <typename TYPE> struct TypeName {
	static const char* name() {
		return typeid(TYPE).name();
	}
};

/// Specialization for int type
template <> struct TypeName<int> {
	static const char* name() {
		return "int";
	}
};

/// Specialization for float type
template <> struct TypeName<float> {
	static const char* name() {
		return "float";
	}
};

/// Specialization for double type
template <> struct TypeName<double> {
	static const char* name() {
		return "double";
	}
};

/// Specialization for string type
template <> struct TypeName<std::string> {
	static const char* name() {
		return "string";
	}
};

/// Class to hold a segmentation parameter with its description
class SegmentationParameter {
public:
	/// Defines the parameter unit type (useful to convert to default set of units)
	enum UnitType {NoUnit, LengthUnit, AngleUnit};
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
	/// Access to the unit type
	UnitType unitType() const {
		return _unitType;
	}
	/// Access to the parameter type
	virtual std::string type() const = 0;
	/// Access to the parameter value in string representation
	virtual std::string value() const = 0;
	/// Set the parameter value in string representation
	virtual void setValue(const std::string& value) = 0;
	/// Access to the parameter default value in string representation
	virtual std::string defaultValue() const = 0;
	/// Check if this parameter is optional
	bool isOptional() const {
		return _isOptional;
	}
	/// Printable version
	std::string toString() const {
		std::stringstream s;
		s << _name << " = " << value();
		if (not _description.empty()) {
			s << " (" << _description << ")";
		}
		return s.str();
	}
protected:
	/// Default constructor used by derived classes
	SegmentationParameter(const std::string& name, const std::string& description, UnitType unitType = NoUnit, bool isOptional = false) :
			_name(name), _description(description), _unitType(unitType), _isOptional(isOptional) {
	}
	/// The parameter name
	std::string _name;
	/// The parameter description
	std::string _description;
	/// The unit type
	UnitType _unitType;
	/// Store if parameter is optional
	bool _isOptional;
};

template<typename TYPE> class TypedSegmentationParameter: public SegmentationParameter {
public:
	/// Default constructor
	TypedSegmentationParameter(const std::string& name, const std::string& description, TYPE& value,
			const TYPE& defaultValue, SegmentationParameter::UnitType unitType = SegmentationParameter::NoUnit, bool isOptional = false) :
			SegmentationParameter(name, description, unitType, isOptional), _value(value), _defaultValue(defaultValue) {
		_value = defaultValue;
	}

	/// Access to the parameter value
	const TYPE& typedValue() const {
		return _value;
	}

	/// Set the parameter value
	void setTypedValue(const TYPE& value) {
		_value = value;
	}

	/// Access to the parameter default value
	const TYPE& typedDefaultValue() const {
		return _defaultValue;
	}

	/// Access to the parameter type
	std::string type() const {
		return TypeName<TYPE>::name();
	}

	/// Access to the parameter value in string representation
	std::string value() const {
		std::stringstream s;
		s << _value;
		return s.str();
	}

	/// Set the parameter value in string representation
	void setValue(const std::string& value) {
		std::stringstream s;
		s << value;
		s >> _value;
	}

	/// Access to the parameter default value in string representation
	std::string defaultValue() const {
		std::stringstream s;
		s << _defaultValue;
		return s.str();
	}

protected:
	TYPE& _value;
	TYPE _defaultValue;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATIONPARAMETER_H_ */
