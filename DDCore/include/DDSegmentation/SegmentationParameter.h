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
#include <vector>

namespace dd4hep {
namespace DDSegmentation {


/// Helper method to split string into tokens
inline std::vector<std::string> splitString(const std::string& s, char delimiter = ' ') {
	std::vector<std::string> elements;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		elements.push_back(item);
	}
	return elements;
}

/// Helper class to extract type names
template<typename TYPE> struct TypeName {
	static const char* name() {
		return typeid(TYPE).name();
	}
};

/// Specialization for int type
template<> struct TypeName<int> {
	static const char* name() {
		return "int";
	}
};

/// Specialization for float type
template<> struct TypeName<float> {
	static const char* name() {
		return "float";
	}
};

/// Specialization for double type
template<> struct TypeName<double> {
	static const char* name() {
		return "double";
	}
};

/// Specialization for string type
template<> struct TypeName<std::string> {
	static const char* name() {
		return "string";
	}
};

/// Specialization for int vector type
template<> struct TypeName<std::vector<int> > {
	static const char* name() {
		return "intvec";
	}
};

/// Specialization for float vector type
template<> struct TypeName<std::vector<float> > {
	static const char* name() {
		return "floatvec";
	}
};

/// Specialization for double vector type
template<> struct TypeName<std::vector<double> > {
	static const char* name() {
		return "doublevec";
	}
};

/// Specialization for string vector type
template<> struct TypeName<std::vector<std::string> > {
	static const char* name() {
		return "stringvec";
	}
};

/// Class to hold a segmentation parameter with its description
class SegmentationParameter {
public:
	/// Defines the parameter unit type (useful to convert to default set of units)
	enum UnitType {
		NoUnit, LengthUnit, AngleUnit
	};
	/// Default destructor
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
	SegmentationParameter(const std::string& nam, const std::string& desc, UnitType unitTyp = NoUnit,
			bool isOpt = false) :
			_name(nam), _description(desc), _unitType(unitTyp), _isOptional(isOpt) {
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
#if defined(G__ROOT) || defined(__CLANG__) || defined(__ROOTCLING__)
  /// Empty constructor for ROOT persistency
  TypedSegmentationParameter()
    : SegmentationParameter("","",SegmentationParameter::NoUnit,false), _value(0), _defaultValue()  {}
#endif
	/// Default constructor
	TypedSegmentationParameter(const std::string& nam, const std::string& desc, TYPE& val,
			const TYPE& default_Value, SegmentationParameter::UnitType unitTyp = SegmentationParameter::NoUnit,
			bool isOpt = false) :
			SegmentationParameter(nam, desc, unitTyp, isOpt), _value(&val), _defaultValue(default_Value) {
		*_value = default_Value;
	}
	/// Default destructor
	virtual ~TypedSegmentationParameter() {	}

	/// Access to the parameter value
	const TYPE& typedValue() const {
		return *_value;
	}

	/// Set the parameter value
	void setTypedValue(const TYPE& val) {
		*_value = val;
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
		s << *_value;
		return s.str();
	}

	/// Set the parameter value in string representation
	void setValue(const std::string& val) {
		std::stringstream s;
		s << val;
		s >> *_value;
	}

	/// Access to the parameter default value in string representation
	std::string defaultValue() const {
		std::stringstream s;
		s << _defaultValue;
		return s.str();
	}

protected:
	TYPE* _value = 0;   //!
	TYPE  _defaultValue;
};

template<typename TYPE> class TypedSegmentationParameter<std::vector<TYPE> > : public SegmentationParameter {
public:
#if defined(G__ROOT) || defined(__CLANG__) || defined(__ROOTCLING__)
  /// Empty constructor for ROOT persistency
  TypedSegmentationParameter()
    : SegmentationParameter("","",SegmentationParameter::NoUnit,false), _value(0), _defaultValue()  {}
#endif
	/// Default constructor
	TypedSegmentationParameter(const std::string& nam, const std::string& desc, std::vector<TYPE>& val,
			const std::vector<TYPE>& defaultVal, SegmentationParameter::UnitType unitTyp =
					SegmentationParameter::NoUnit, bool isOpt = false) :
			SegmentationParameter(nam, desc, unitTyp, isOpt), _value(&val), _defaultValue(defaultVal) {
		*_value = defaultVal;
	}
	/// Default destructor
	virtual ~TypedSegmentationParameter() {	}

	/// Access to the parameter value
	const std::vector<TYPE>& typedValue() const {
		return *_value;
	}

	/// Set the parameter value
	void setTypedValue(const std::vector<TYPE>& val) {
		*_value = val;
	}

	/// Access to the parameter default value
	const std::vector<TYPE>& typedDefaultValue() const {
		return _defaultValue;
	}

	/// Access to the parameter type
	std::string type() const {
		std::stringstream s;
		s << TypeName<TYPE>::name() << "vec";
		return s.str() ;
	}

	/// Access to the parameter value in string representation
	std::string value() const {
		std::stringstream s;
		for (const auto& it : *_value )  {
			s << it;
			s << " ";
		}
		return s.str();
	}

	/// Set the parameter value in string representation
	void setValue(const std::string& val) {
		std::vector<std::string> elements = splitString(val);
		_value->clear();
		for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
			if (not it->empty()) {
				TYPE entry;
				std::stringstream s;
				s << *it;
				s >> entry;
				_value->push_back(entry);
			}
		}
	}

	/// Access to the parameter default value in string representation
	std::string defaultValue() const {
		std::stringstream s;
		typename std::vector<TYPE>::const_iterator it = _defaultValue.begin();
		for (; it != _defaultValue.end(); ++it) {
			s << *it;
			s << " ";
		}
		return s.str();
	}

protected:
	std::vector<TYPE>* _value = 0;   //!
	std::vector<TYPE>  _defaultValue;

};

} /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_SEGMENTATIONPARAMETER_H_ */
