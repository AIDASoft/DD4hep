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
#include <iomanip>

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
Segmentation::Segmentation(BitField64* newDecoder) :
		_name("Segmentation"), _type("Segmentation"), _decoder(newDecoder), _ownsDecoder(false) {
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
VolumeID Segmentation::volumeID(const CellID& cID) const {
	map<string, StringParameter>::const_iterator it;
	_decoder->setValue(cID);
	for (it = _indexIdentifiers.begin(); it != _indexIdentifiers.end(); ++it) {
		string identifier = it->second->typedValue();
		(*_decoder)[identifier] = 0;
	}
	return _decoder->getValue();
}

/// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
void Segmentation::neighbours(const CellID& cID, std::set<CellID>& cellNeighbours) const {
	map<string, StringParameter>::const_iterator it;
	for (it = _indexIdentifiers.begin(); it != _indexIdentifiers.end(); ++it) {
		string identifier = it->second->typedValue();
		_decoder->setValue(cID);
		int currentValue = (*_decoder)[identifier];
		// add both neighbouring cell IDs, don't add out of bound indices
		try {
			(*_decoder)[identifier] = currentValue - 1;
			cellNeighbours.insert(_decoder->getValue());
		} catch (runtime_error& e) {
			// nothing to do
		}
		try {
			(*_decoder)[identifier] = currentValue + 1;
			cellNeighbours.insert(_decoder->getValue());
		} catch (runtime_error& e) {
			// nothing to do
		}
	}
}

/// Set the underlying decoder
void Segmentation::setDecoder(BitField64* newDecoder) {
	if (_ownsDecoder and _decoder != 0) {
		delete _decoder;
	}
	_decoder = newDecoder;
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
	Parameters pars;
	map<string, Parameter>::const_iterator it;
	for (it = _parameters.begin(); it != _parameters.end(); ++it) {
		pars.push_back(it->second);
	}
	return pars;
}

/// Set all parameters from an existing set of parameters
void Segmentation::setParameters(const Parameters& pars) {
	Parameters::const_iterator it;
	for (it = pars.begin(); it != pars.end(); ++it) {
		Parameter p = *it;
		parameter(p->name())->value() = p->value();
	}
}

/// Add a cell identifier to this segmentation. Used by derived classes to define their required identifiers
void Segmentation::registerIdentifier(const string& idName, const string& idDescription, string& identifier,
		const string& defaultValue) {
	StringParameter idParameter =
	  new TypedSegmentationParameter<string>(idName, idDescription, identifier, defaultValue,
						 SegmentationParameter::NoUnit, true);
	_parameters[idName]       = idParameter;
	_indexIdentifiers[idName] = idParameter;
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

  // include the lower edge to the segmentation, deal with numerical issues
  if(fabs(position/cellBoundaries.front()-1.0) < 3e-12) return 0;

  // include the upper edge of the last bin to the segmentation, deal with numerical issues
  if(fabs(position/cellBoundaries.back()-1.0)  < 3e-12) return int(cellBoundaries.size()-2);

  // hits outside cannot be treated
  if(position < cellBoundaries.front()) {
    std::stringstream err;
    err << std::setprecision(20) << std::scientific;
    err << "Hit Position (" << position << ") is below the acceptance"
	<< " (" << cellBoundaries.front() << ") "
	<< "of the segmentation";
    throw std::runtime_error(err.str());
  }
  if(position > cellBoundaries.back() ) {
    std::stringstream err;
    err << std::setprecision(20) << std::scientific;
    err << "Hit Position (" << position << ") is above the acceptance"
	<< " (" << cellBoundaries.back() << ") "
	<< "of the segmentation";
    throw std::runtime_error(err.str());
  }


  std::vector<double>::const_iterator bin = std::upper_bound(cellBoundaries.begin(),
							     cellBoundaries.end(),
							     position-offset);

  // need to reduce found bin by one, because upper_bound works that way, lower_bound would not help
  return bin - cellBoundaries.begin() - 1 ;

}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
