/*
 * CylindricalSegmentation.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CylindricalSegmentation.h"

#include <sstream>
#include <stdexcept>

namespace DD4hep {
namespace DDSegmentation {

using std::map;
using std::runtime_error;
using std::string;
using std::stringstream;

/// default constructor using an encoding string
template<> CylindricalSegmentation::CylindricalSegmentation(const string& cellEncoding, const string& layerField) :
		Segmentation(cellEncoding), _layerID(layerField) {
	_isLocal = false;
}

/// default constructor using an encoding string
template<> CylindricalSegmentation::CylindricalSegmentation(string cellEncoding, const string& layerField) :
		Segmentation(cellEncoding), _layerID(layerField) {
	_isLocal = false;
}

/// default constructor using an encoding string
template<> CylindricalSegmentation::CylindricalSegmentation(const char* cellEncoding, const string& layerField) :
		Segmentation(cellEncoding), _layerID(layerField) {
	_isLocal = false;
}

/// default constructor using an existing decoder
template<> CylindricalSegmentation::CylindricalSegmentation(BitField64* decoder, const string& layerField) :
		Segmentation(decoder), _layerID(layerField) {
	_isLocal = false;
}


/// destructor
CylindricalSegmentation::~CylindricalSegmentation() {

}

/// determine the radius based on the cell ID
double CylindricalSegmentation::getRadius(const long64& cellID) const {
	_decoder->setValue(cellID);
	int layer = (*_decoder)[_layerID];
	map<int, double>::const_iterator itMap = _layerRadiusMap.find(layer);
	if (itMap == _layerRadiusMap.end()) {
		stringstream message;
		message << "getRadius(): Invalid layer index " << layer << " from cell ID " << cellID;
		throw runtime_error(message.str());
	}
	return itMap->second;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
