/*
 * CartesianXYSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianXYSegmentation.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;
using std::vector;

CartesianXYSegmentation::CartesianXYSegmentation(const string& cellEncoding, double cellSizeX, double cellSizeY,
		double offsetX, double offsetY) :
		CartesianSegmentation(cellEncoding), _cellSizeX(cellSizeX), _cellSizeY(cellSizeY), _offsetX(offsetX), _offsetY(
				offsetY) {

}

CartesianXYSegmentation::CartesianXYSegmentation(const BitField64& decoder, double cellSizeX, double cellSizeY,
		double offsetX, double offsetY) :
		CartesianSegmentation(decoder), _cellSizeX(cellSizeX), _cellSizeY(cellSizeY), _offsetX(offsetX), _offsetY(
				offsetY) {

}

CartesianXYSegmentation::~CartesianXYSegmentation() {

}

vector<double> CartesianXYSegmentation::getLocalPosition(const long64& cellID) const {
	_decoder.setValue(cellID);
	vector<double> localPosition(3);
	localPosition[0] = binToPosition(_decoder["x"].value(), _cellSizeX, _offsetX);
	localPosition[1] = binToPosition(_decoder["y"].value(), _cellSizeY, _offsetY);
	localPosition[2] = 0.;
	return localPosition;
}

long64 CartesianXYSegmentation::getCellID(double x, double y, double z) const {
	_decoder.reset();
	_decoder["x"] = positionToBin(x, _cellSizeX, _offsetX);
	_decoder["y"] = positionToBin(y, _cellSizeY, _offsetY);
	return _decoder.getValue();
}

} /* namespace Segmentation */
} /* namespace DD4hep */
