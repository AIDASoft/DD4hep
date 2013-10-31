/*
 * CartesianGridXY.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXZ.h"

namespace DD4hep {
namespace DDSegmentation {

using std::make_pair;
using std::string;
using std::vector;

/// default constructor using an encoding string
template<> CartesianGridXZ::CartesianGridXZ(const string& cellEncoding, double gridSizeX, double gridSizeZ, double offsetX,
		double offsetZ, const string& xField, const string& zField) :
		CartesianGrid(cellEncoding), _gridSizeX(gridSizeX), _gridSizeZ(gridSizeZ), _offsetX(offsetX), _offsetZ(offsetZ), _xId(
				xField), _zId(zField) {
	_type = "grid_xz";
}

/// default constructor using an encoding string
template<> CartesianGridXZ::CartesianGridXZ(string cellEncoding, double gridSizeX, double gridSizeZ, double offsetX,
		double offsetZ, const string& xField, const string& zField) :
		CartesianGrid(cellEncoding), _gridSizeX(gridSizeX), _gridSizeZ(gridSizeZ), _offsetX(offsetX), _offsetZ(offsetZ), _xId(
				xField), _zId(zField) {
	_type = "grid_xz";
}

/// default constructor using an encoding string
template<> CartesianGridXZ::CartesianGridXZ(const char* cellEncoding, double gridSizeX, double gridSizeZ, double offsetX,
		double offsetZ, const string& xField, const string& zField) :
		CartesianGrid(cellEncoding), _gridSizeX(gridSizeX), _gridSizeZ(gridSizeZ), _offsetX(offsetX), _offsetZ(offsetZ), _xId(
				xField), _zId(zField) {
	_type = "grid_xz";
}

/// default constructor using an existing decoder
template<> CartesianGridXZ::CartesianGridXZ(BitField64* decoder, double gridSizeX, double gridSizeZ, double offsetX,
		double offsetZ, const string& xField, const string& zField) :
		CartesianGrid(decoder), _gridSizeX(gridSizeX), _gridSizeZ(gridSizeZ), _offsetX(offsetX), _offsetZ(offsetZ), _xId(
				xField), _zId(zField) {
	_type = "grid_xz";
}

/// destructor
CartesianGridXZ::~CartesianGridXZ() {

}

/// determine the position based on the cell ID
vector<double> CartesianGridXZ::getPosition(const long64& cellID) const {
	_decoder->setValue(cellID);
	vector<double> localPosition(3);
	localPosition[0] = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	localPosition[1] = 0.;
	localPosition[2] = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return localPosition;
}

/// determine the cell ID based on the position
long64 CartesianGridXZ::getCellID(double x, double y, double z) const {
	_decoder->reset();
	(*_decoder)[_xId] = positionToBin(x, _gridSizeX, _offsetX);
	(*_decoder)[_zId] = positionToBin(z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

/// access the set of parameters for this segmentation
Parameters CartesianGridXZ::parameters() const {
	Parameters parameters;
	parameters.push_back(make_pair("grid_size_x", _gridSizeX));
	parameters.push_back(make_pair("grid_size_z", _gridSizeZ));
	parameters.push_back(make_pair("offset_x", _offsetX));
	parameters.push_back(make_pair("offset_z", _offsetZ));
	return parameters;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
