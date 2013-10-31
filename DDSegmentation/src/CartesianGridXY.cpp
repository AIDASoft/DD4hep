/*
 * CartesianGridXY.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianGridXY.h"

namespace DD4hep {
namespace DDSegmentation {

using std::make_pair;
using std::string;
using std::vector;

/// default constructor using an encoding string
template<> CartesianGridXY::CartesianGridXY(const string& cellEncoding, double gridSizeX, double gridSizeY, double offsetX,
		double offsetY, const string& xField, const string& yField) :
		CartesianGrid(cellEncoding), _gridSizeX(gridSizeX), _gridSizeY(gridSizeY), _offsetX(offsetX), _offsetY(offsetY), _xId(
				xField), _yId(yField) {
	_type = "grid_xy";
}

/// default constructor using an encoding string
template<> CartesianGridXY::CartesianGridXY(string cellEncoding, double gridSizeX, double gridSizeY, double offsetX,
		double offsetY, const string& xField, const string& yField) :
		CartesianGrid(cellEncoding), _gridSizeX(gridSizeX), _gridSizeY(gridSizeY), _offsetX(offsetX), _offsetY(offsetY), _xId(
				xField), _yId(yField) {
	_type = "grid_xy";
}

/// default constructor using an encoding string
template<> CartesianGridXY::CartesianGridXY(const char* cellEncoding, double gridSizeX, double gridSizeY, double offsetX,
		double offsetY, const string& xField, const string& yField) :
		CartesianGrid(cellEncoding), _gridSizeX(gridSizeX), _gridSizeY(gridSizeY), _offsetX(offsetX), _offsetY(offsetY), _xId(
				xField), _yId(yField) {
	_type = "grid_xy";
}

/// default constructor using an existing decoder
template<> CartesianGridXY::CartesianGridXY(BitField64* decoder, double gridSizeX, double gridSizeY, double offsetX,
		double offsetY, const string& xField, const string& yField) :
		CartesianGrid(decoder), _gridSizeX(gridSizeX), _gridSizeY(gridSizeY), _offsetX(offsetX), _offsetY(offsetY), _xId(
				xField), _yId(yField) {
	_type = "grid_xy";
}

/// destructor
CartesianGridXY::~CartesianGridXY() {

}

/// determine the position based on the cell ID
vector<double> CartesianGridXY::getPosition(const long64& cellID) const {
	_decoder->setValue(cellID);
	vector<double> localPosition(3);
	localPosition[0] = binToPosition((*_decoder)[_xId].value(), _gridSizeX, _offsetX);
	localPosition[1] = binToPosition((*_decoder)[_yId].value(), _gridSizeY, _offsetY);
	localPosition[2] = 0.;
	return localPosition;
}

/// determine the cell ID based on the position
long64 CartesianGridXY::getCellID(double x, double y, double z) const {
	_decoder->reset();
	(*_decoder)[_xId] = positionToBin(x, _gridSizeX, _offsetX);
	(*_decoder)[_yId] = positionToBin(y, _gridSizeY, _offsetY);
	return _decoder->getValue();
}

/// access the set of parameters for this segmentation
Parameters CartesianGridXY::parameters() const {
	Parameters parameters;
	parameters.push_back(make_pair("grid_size_x", _gridSizeX));
	parameters.push_back(make_pair("grid_size_y", _gridSizeY));
	parameters.push_back(make_pair("offset_x", _offsetX));
	parameters.push_back(make_pair("offset_y", _offsetY));
	return parameters;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
