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
#include <cmath>
#include <algorithm>

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

/// Default constructor used by derived classes passing an existing decoder
Segmentation::Segmentation(BitField64* decoder) :
		_name("Segmentation"), _type("Segmentation"), _decoder(decoder), _ownsDecoder(false) {
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

/// Determine the volume ID from the full cell ID by removing all local fields
VolumeID Segmentation::volumeID(const CellID& cellID) const {
	map<string, StringParameter>::const_iterator it;
	_decoder->setValue(cellID);
	for (it = _indexIdentifiers.begin(); it != _indexIdentifiers.end(); ++it) {
		string identifier = it->second->typedValue();
		(*_decoder)[identifier] = 0;
	}
	return _decoder->getValue();
}

/// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
void Segmentation::neighbours(const CellID& cellID, std::set<CellID>& neighbours) const {
	map<string, StringParameter>::const_iterator it;
	for (it = _indexIdentifiers.begin(); it != _indexIdentifiers.end(); ++it) {
		string identifier = it->second->typedValue();
		_decoder->setValue(cellID);
		int currentValue = (*_decoder)[identifier];
		// add both neighbouring cell IDs, don't add out of bound indices
		try {
			(*_decoder)[identifier] = currentValue - 1;
			neighbours.insert(_decoder->getValue());
		} catch (runtime_error& e) {
			// nothing to do
		}
		try {
			(*_decoder)[identifier] = currentValue + 1;
			neighbours.insert(_decoder->getValue());
		} catch (runtime_error& e) {
			// nothing to do
		}
	}
}

/// Set the underlying decoder
void Segmentation::setDecoder(BitField64* decoder) {
	if (_ownsDecoder and _decoder != 0) {
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

/// Add a cell identifier to this segmentation. Used by derived classes to define their required identifiers
void Segmentation::registerIdentifier(const string& name, const string& description, string& identifier,
		const string& defaultValue) {
	StringParameter idParameter = new TypedSegmentationParameter<string>(name, description, identifier, defaultValue,
			SegmentationParameter::NoUnit, true);
	_parameters[name] = idParameter;
	_indexIdentifiers[name] = idParameter;
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
	return int(floor((position + 0.5 * cellSize - offset) / cellSize));
}

/// Helper method to convert a bin number to a 1D position given a vector of binBoundaries
double Segmentation::binToPosition(CellID bin, std::vector<double> const& cellBoundaries, double offset) {
  return (cellBoundaries[bin+1] + cellBoundaries[bin])*0.5 + offset;
}
/// Helper method to convert a 1D position to a cell ID given a vector of binBoundaries
int Segmentation::positionToBin(double position, std::vector<double> const& cellBoundaries, double offset) {

  // include the lower edge to the segmentation
  if(fabs(position - cellBoundaries.front()) < 1e-12) return 0;

  // include the upper edge of the last bin to the segmentation
  if(fabs(position - cellBoundaries.back())  < 1e-12) return int(cellBoundaries.size()-2);

  // hits outside cannot be treated
  if(position < cellBoundaries.front()) throw std::runtime_error("Hit Position is outside of segmentation");
  if(position > cellBoundaries.back() ) throw std::runtime_error("Hit Position is outside of segmentation");


  std::vector<double>::const_iterator bin = std::upper_bound(cellBoundaries.begin(),
							     cellBoundaries.end(),
							     position-offset);

  // need to reduce found bin by one, because upper_bound works that way, lower_bound would not help
  return bin - cellBoundaries.begin() - 1 ;

}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
