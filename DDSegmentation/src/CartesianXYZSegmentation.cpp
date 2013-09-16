/*
 * CartesianXYZSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianXYZSegmentation.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;
using std::vector;

CartesianXYZSegmentation::CartesianXYZSegmentation(const string& cellEncoding, double cellSizeX, double cellSizeY, double cellSizeZ,
		double offsetX, double offsetY, double offsetZ) :
		CartesianXYSegmentation(cellEncoding, cellSizeX, cellSizeY, offsetX, offsetY), _cellSizeZ(cellSizeZ), _offsetZ(offsetZ) {

}

CartesianXYZSegmentation::CartesianXYZSegmentation(const BitField64& decoder, double cellSizeX, double cellSizeY, double cellSizeZ,
		double offsetX, double offsetY, double offsetZ) :
		CartesianXYSegmentation(decoder, cellSizeX, cellSizeY, offsetX, offsetY), _cellSizeZ(cellSizeZ), _offsetZ(offsetZ) {

}

CartesianXYZSegmentation::~CartesianXYZSegmentation() {

}

vector<double> CartesianXYZSegmentation::getLocalPosition(const long64& cellID) const {
	_decoder.setValue(cellID);
	vector<double> localPosition;
	localPosition[0] = binToPosition(_decoder["x"].value(), _cellSizeX, _offsetX);
	localPosition[1] = binToPosition(_decoder["y"].value(), _cellSizeY, _offsetY);
	localPosition[2] = binToPosition(_decoder["z"].value(), _cellSizeZ, _offsetZ);
	return localPosition;
}

long64 CartesianXYZSegmentation::getCellID(double x, double y, double z) const {
	_decoder.reset();
	_decoder["x"] = positionToBin(x, _cellSizeX, _offsetX);
	_decoder["y"] = positionToBin(y, _cellSizeY, _offsetY);
	_decoder["z"] = positionToBin(z, _cellSizeZ, _offsetZ);
	return _decoder.getValue();
}

} /* namespace Segmentation */
} /* namespace DD4hep */
