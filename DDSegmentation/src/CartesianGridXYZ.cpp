/*
 * CartesianGridXYZ.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXYZ.h"

namespace DD4hep {
namespace DDSegmentation {

using std::make_pair;
using std::string;
using std::vector;

/// default constructor using an encoding string
template<> CartesianGridXYZ::CartesianGridXYZ(const string& cellEncoding, double gridSizeX, double gridSizeY, double gridSizeZ,
		double offsetX, double offsetY, double offsetZ, string xField, string yField, string zField) :
		CartesianGridXY(cellEncoding, gridSizeX, gridSizeY, offsetX, offsetY, xField, yField), _gridSizeZ(gridSizeZ), _offsetZ(
				offsetZ), _zId(zField) {
	_type = "grid_xyz";
}

/// default constructor using an encoding string
template<> CartesianGridXYZ::CartesianGridXYZ(string cellEncoding, double gridSizeX, double gridSizeY, double gridSizeZ,
		double offsetX, double offsetY, double offsetZ, string xField, string yField, string zField) :
		CartesianGridXY(cellEncoding, gridSizeX, gridSizeY, offsetX, offsetY, xField, yField), _gridSizeZ(gridSizeZ), _offsetZ(
				offsetZ), _zId(zField) {
	_type = "grid_xyz";
}

/// default constructor using an encoding string
template<> CartesianGridXYZ::CartesianGridXYZ(const char* cellEncoding, double gridSizeX, double gridSizeY, double gridSizeZ,
		double offsetX, double offsetY, double offsetZ, string xField, string yField, string zField) :
		CartesianGridXY(cellEncoding, gridSizeX, gridSizeY, offsetX, offsetY, xField, yField), _gridSizeZ(gridSizeZ), _offsetZ(
				offsetZ), _zId(zField) {
	_type = "grid_xyz";
}

/// default constructor using an existing decoder
CartesianGridXYZ::CartesianGridXYZ(BitField64* decoder, double gridSizeX, double gridSizeY, double gridSizeZ,
		double offsetX, double offsetY, double offsetZ, string xField, string yField, string zField) :
		CartesianGridXY(decoder, gridSizeX, gridSizeY, offsetX, offsetY, xField, yField), _gridSizeZ(gridSizeZ), _offsetZ(
				offsetZ), _zId(zField) {
	_type = "grid_xyz";
}

/// destructor
CartesianGridXYZ::~CartesianGridXYZ() {

}

/// determine the local position based on the cell ID
vector<double> CartesianGridXYZ::getLocalPosition(const long64& cellID) const {
	_decoder->setValue(cellID);
	vector<double> localPosition(3);
	localPosition[0] = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	localPosition[1] = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	localPosition[2] = binToPosition((*_decoder)[_zId].value(), _gridSizeZ, _offsetZ);
	return localPosition;
}

/// determine the cell ID based on the local position
long64 CartesianGridXYZ::getCellID(double x, double y, double z) const {
	_decoder->reset();
	(*_decoder)[_xId] = positionToBin(x, _gridSizeX, _offsetX);
	(*_decoder)[_yId] = positionToBin(y, _gridSizeY, _offsetY);
	(*_decoder)[_zId] = positionToBin(z, _gridSizeZ, _offsetZ);
	return _decoder->getValue();
}

/// access the set of parameters for this segmentation
Parameters CartesianGridXYZ::parameters() const {
	Parameters parameters;
	parameters.push_back(make_pair("grid_size_x", _gridSizeX));
	parameters.push_back(make_pair("grid_size_y", _gridSizeY));
	parameters.push_back(make_pair("grid_size_z", _gridSizeZ));
	return parameters;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
