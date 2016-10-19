/*
 * TiledLayerSegmentation.cpp
 *
 *  Created on: Mar 10, 2014
 *      Author: cgrefe
 */

#include "DDSegmentation/TiledLayerSegmentation.h"

// C/C++ includes
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace DD4hep {
namespace DDSegmentation {

using std::find;
using std::runtime_error;
using std::stringstream;
using std::vector;

TiledLayerSegmentation::TiledLayerSegmentation(const std::string& cellEncoding) :
		Segmentation(cellEncoding) {
	_type = "TiledLayerSegmentation";
	_description = "Cartesian segmentation using optimal tiling depending on the layer dimensions";

	// register all necessary parameters
	registerParameter("grid_size_x", "Default cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Default cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerIdentifier("identifier_x", "Cell encoding identifier for X", _identifierX, "x");
	registerIdentifier("identifier_y", "Cell encoding identifier for Y", _identifierY, "y");
	registerParameter("identifier_layer", "Cell encoding identifier for layer", _identifierLayer, std::string("layer"),
			SegmentationParameter::NoUnit, true);
	registerParameter("layer_identifiers", "List of valid layer identifiers", _layerIndices, vector<int>(),
			SegmentationParameter::NoUnit, true);
	registerParameter("x_dimensions", "List of layer x dimensions", _layerDimensionsX, vector<double>(),
			SegmentationParameter::NoUnit, true);
	registerParameter("y_dimensions", "List of layer y dimensions", _layerDimensionsY, vector<double>(),
			SegmentationParameter::NoUnit, true);

}

/// Default constructor used by derived classes passing an existing decoder
TiledLayerSegmentation::TiledLayerSegmentation(BitField64* decode) :	Segmentation(decode) {
	_type = "TiledLayerSegmentation";
	_description = "Cartesian segmentation using optimal tiling depending on the layer dimensions";

	// register all necessary parameters
	registerParameter("grid_size_x", "Default cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Default cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerIdentifier("identifier_x", "Cell encoding identifier for X", _identifierX, "x");
	registerIdentifier("identifier_y", "Cell encoding identifier for Y", _identifierY, "y");
	registerParameter("identifier_layer", "Cell encoding identifier for layer", _identifierLayer, std::string("layer"),
			SegmentationParameter::NoUnit, true);
	registerParameter("layer_identifiers", "List of valid layer identifiers", _layerIndices, vector<int>(),
			SegmentationParameter::NoUnit, true);
	registerParameter("x_dimensions", "List of layer x dimensions", _layerDimensionsX, vector<double>(),
			SegmentationParameter::NoUnit, true);
	registerParameter("y_dimensions", "List of layer y dimensions", _layerDimensionsY, vector<double>(),
			SegmentationParameter::NoUnit, true);

}

TiledLayerSegmentation::~TiledLayerSegmentation() {
}

/// access the actual grid size in X for a given layer
double TiledLayerSegmentation::layerGridSizeX(int layerIndex) const {
	// should be cached in a map if calculateOptimalCellSize is expensive
	return calculateOptimalCellSize(_gridSizeX, layerDimensions(layerIndex).x);
}

/// access the actual grid size in Y for a given layer
double TiledLayerSegmentation::layerGridSizeY(int layerIndex) const {
	// should be cached in a map if calculateOptimalCellSize is expensive
	return calculateOptimalCellSize(_gridSizeY, layerDimensions(layerIndex).y);
}

/// set the dimensions of the given layer
void TiledLayerSegmentation::setLayerDimensions(int layerIndex, double x, double y) {
	// a bit clumsy since we use three vectors instead of a map<int, LayerDimensions>
	if (_layerIndices.size() != _layerDimensionsX.size() or _layerIndices.size() != _layerDimensionsY.size()) {
		throw runtime_error(
				"TiledLayerSegmentation::setLayerDimensions: inconsistent size of layer parameter vectors.");
	}
	vector<int>::iterator it = find(_layerIndices.begin(), _layerIndices.end(), layerIndex);
	if (it == _layerIndices.end()) {
		_layerIndices.push_back(layerIndex);
		_layerDimensionsX.push_back(x);
		_layerDimensionsY.push_back(y);
	} else {
		size_t index = it - _layerIndices.begin();
		_layerDimensionsX[index] = x;
		_layerDimensionsY[index] = y;
	}
}

/// access to the dimensions of the given layer
TiledLayerSegmentation::LayerDimensions TiledLayerSegmentation::layerDimensions(int layerIndex) const {
	// a bit clumsy since we use three vectors instead of a map<int, LayerDimensions>
	if (_layerIndices.size() != _layerDimensionsX.size() or _layerIndices.size() != _layerDimensionsY.size()) {
		throw runtime_error(
				"TiledLayerSegmentation::layerDimensions: inconsistent size of layer parameter vectors.");
	}
	vector<int>::const_iterator it = find(_layerIndices.begin(), _layerIndices.end(), layerIndex);
	if (it == _layerIndices.end()) {
		stringstream message;
		message << "TiledLayerSegmentation::layerDimensions: invalid layer index " << layerIndex;
		throw runtime_error(message.str());
	} else {
		size_t index = it - _layerIndices.begin();
		return LayerDimensions(_layerDimensionsX[index], _layerDimensionsY[index]);
	}
}

/// determine the position based on the cell ID
Vector3D TiledLayerSegmentation::position(const CellID& cID) const {
	_decoder->setValue(cID);
	int layerIndex = (*_decoder)[_identifierLayer];
	double cellSizeX = layerGridSizeX(layerIndex);
	double cellSizeY = layerGridSizeY(layerIndex);
	LayerDimensions dimensions = layerDimensions(layerIndex);
	double offsetX = calculateOffset(cellSizeX, dimensions.x);
	double offsetY = calculateOffset(cellSizeY, dimensions.y);
	double localX = binToPosition((*_decoder)[_identifierX], cellSizeX, offsetX);
	double localY = binToPosition((*_decoder)[_identifierY], cellSizeY, offsetY);
	return Vector3D(localX, localY, 0.);
}
/// determine the cell ID based on the position
  CellID TiledLayerSegmentation::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */,
		const VolumeID& vID) const {
	_decoder->setValue(vID);
	int layerIndex = (*_decoder)[_identifierLayer];
	double cellSizeX = layerGridSizeX(layerIndex);
	double cellSizeY = layerGridSizeY(layerIndex);
	LayerDimensions dimensions = layerDimensions(layerIndex);
	double offsetX = calculateOffset(cellSizeX, dimensions.x);
	double offsetY = calculateOffset(cellSizeY, dimensions.y);
	(*_decoder)[_identifierX] = positionToBin(localPosition.x(), cellSizeX, offsetX);
	(*_decoder)[_identifierY] = positionToBin(localPosition.y(), cellSizeY, offsetY);
	return _decoder->getValue();
}

/// helper method to calculate optimal cell size based on total size
  double TiledLayerSegmentation::calculateOptimalCellSize(double /* nominalCellSize */, double /* totalSize */) {
	// TODO: implement algorithm to calculate optimal cell size
	return 1.;
}

/// helper method to calculate offset of bin 0 based on the total size
double TiledLayerSegmentation::calculateOffset(double /* cellSize */, double /* totalSize */) {
	// TODO: implement algorithm to calculate placement of bin 0
	return 0.;
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
