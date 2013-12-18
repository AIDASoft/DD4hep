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
using std::runtime_error;
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
Parameter Segmentation::parameter(const std::string& parameterName) const {
	map<string, Parameter>::const_iterator it;
	it = _parameters.find(parameterName);
	if (it != _parameters.end()) {
		return it->second;
	}
	stringstream s;
	s << "Unknown parameter " << parameterName << " for segmentation type " << _type;
	throw std::runtime_error(s.str());
}

/// Access to all parameters
Parameters Segmentation::parameters() const {
	Parameters parameters;
	map<string, Parameter>::const_iterator it;
	for (it = _parameters.begin(); it != _parameters.end(); ++it) {
		parameters.push_back(it->second);
	}
	return parameters;
}

/// Set all parameters from an existing set of parameters
void Segmentation::setParameters(const Parameters& parameters) {
	Parameters::const_iterator it;
	for (it = parameters.begin(); it != parameters.end(); ++it) {
		Parameter p = *it;
		parameter(p->name())->value() = p->value();
	}
}

/// Add a parameter to this segmentation. Used by derived classes to define their parameters
void Segmentation::registerParameter(const std::string& name, const std::string& description, double& parameter, const double& defaultValue, bool isOptional) {
	_parameters[name] = new SegmentationParameter(name, description, parameter, defaultValue, isOptional);
}


/// Helper method to convert a bin number to a 1D position
double Segmentation::binToPosition(long64 bin, double cellSize, double offset) {
	return bin * cellSize + offset;
}

/// Helper method to convert a 1D position to a cell ID
int Segmentation::positionToBin(double position, double cellSize, double offset) {
	if (cellSize == 0.) {
		throw runtime_error("Invalid cell size: 0.0");
	}
	return int((position + 0.5 * cellSize - offset)/cellSize);
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
