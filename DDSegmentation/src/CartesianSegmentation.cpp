/*
 * CartesianSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CartesianSegmentation.h"

namespace DD4hep {
namespace DDSegmentation {

using std::string;

CartesianSegmentation::CartesianSegmentation(const string& cellEncoding) :
		Segmentation(cellEncoding) {

}

CartesianSegmentation::CartesianSegmentation(const BitField64& decoder) :
		Segmentation(decoder) {

}

CartesianSegmentation::~CartesianSegmentation() {

}

double CartesianSegmentation::binToPosition(long64 bin, double cellSize, double offset) const {
	return bin * cellSize + offset;
}

int CartesianSegmentation::positionToBin(double position, double cellSize, double offset) const {
	return int((position + 0.5 * cellSize - offset)/cellSize);
}

} /* namespace Segmentation */
} /* namespace DD4hep */
