/*
 * Segmentation.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/Segmentation.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace DD4hep {
namespace DDSegmentation {

using std::cerr;
using std::endl;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

/// Default constructor used by derived classes passing the encoding string
Segmentation::Segmentation(const string& cellEncoding) :
	_name("Segmentation"), _type("Segmentation"), _decoder(new BitField64(cellEncoding)), _ownsDecoder(true) {
}

/// Destructor
Segmentation::~Segmentation() {
	if (_ownsDecoder and _decoder != 0) {
		delete _decoder;
	}
	map<string, SegmentationParameter*>::iterator it;
	for (it = _parameters.begin(); it != _parameters.end(); ++it) {
		SegmentationParameter* p = it->second;
		if (p) {
			delete p;
			p = 0;
		}
	}
}

/// Set the underlying decoder
void Segmentation::setDecoder(BitField64* decoder) {
	if (_ownsDecoder and _decoder != 0) {
		std::cout << _decoder << std::endl;
		delete _decoder;
	}
	_decoder = decoder;
	_ownsDecoder = false;
}

/// Access to parameter by name
SegmentationParameter* Segmentation::parameter(const std::string& parameterName) {
	map<string, SegmentationParameter*>::iterator it;
	it = _parameters.find(parameterName);
	if (it != _parameters.end()) {
		return it->second;
	}
	stringstream s;
	s << "Unknown parameter " << parameterName << " for segmentation type " << _type;
	throw std::runtime_error(s.str());
}
/// Access to parameter by name
const SegmentationParameter* Segmentation::parameter(const std::string& parameterName) const {
	map<string, SegmentationParameter*>::const_iterator it;
	it = _parameters.find(parameterName);
	if (it != _parameters.end()) {
		return it->second;
	}
	stringstream s;
	s << "Unknown parameter " << parameterName << " for segmentation type " << _type;
	throw std::runtime_error(s.str());
}

/// Access to parameter values by name
double Segmentation::parameterValue(const std::string& parameterName) const {
	return this->parameter(parameterName)->value();
}

/// Set the parameter value by name
void Segmentation::setParameterValue(const std::string& parameterName, double value) {
	this->parameter(parameterName)->value() = value;
}

/// Access to all parameters
//Parameters Segmentation::parameters() const {
//	Parameters parameters;
//	map<string, SegmentationParameter*>::const_iterator it;
//	for (it = _parameters.begin(); it != _parameters.end(); ++it) {
//		parameters.push_back(std::make_pair(it->first, it->second->value()));
//	}
//	return parameters;
//}

/// Access to all parameters
vector<SegmentationParameter*> Segmentation::parameters() {
	vector<SegmentationParameter*> parameters;
	map<string, SegmentationParameter*>::iterator it;
	for (it = _parameters.begin(); it != _parameters.end(); ++it) {
		parameters.push_back(it->second);
	}
	return parameters;
}

/// Access to all parameters
vector<const SegmentationParameter*> Segmentation::parameters() const {
	vector<const SegmentationParameter*> parameters;
	map<string, SegmentationParameter*>::const_iterator it;
	for (it = _parameters.begin(); it != _parameters.end(); ++it) {
		parameters.push_back(it->second);
	}
	return parameters;
}

/// Add a parameter to this segmentation. Used by derived classes to define their parameters
void Segmentation::registerParameter(const std::string& name, const std::string& description, double& parameter, double defaultValue, bool isOptional) {
	_parameters[name] = new SegmentationParameter(name, description, parameter, defaultValue, isOptional);
}


/// Helper method to convert a bin number to a 1D position
double Segmentation::binToPosition(long64 bin, double cellSize, double offset) const {
	return bin * cellSize + offset;
}

/// Helper method to convert a 1D position to a cell ID
int Segmentation::positionToBin(double position, double cellSize, double offset) const {
	if (cellSize == 0.) {
		throw std::runtime_error("Invalid cell size: 0.0");
	}
	return int((position + 0.5 * cellSize - offset)/cellSize);
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
