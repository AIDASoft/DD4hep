/*
 * Layer.cpp
 *
 *  Created on: Apr 18, 2013
 *      Author: Christian Grefe, CERN
 */

#include "LayerStack.h"
#include "Exceptions.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Shapes.h"

#include <sstream>

namespace DD4hep {
namespace Geometry {

using std::stringstream;

LayerStack::LayerStack(const DetElement& det) : det(det) {

}

LayerStack::LayerStack(const LayerStack& layerStack, const DetElement& det) : det(det) {

}

LayerStack::~LayerStack() {

}

double LayerStack::getThickness(int layerIndex, int moduleIndex) const {
	double thickness = 0.;
	for (int sliceIndex = 0; sliceIndex < getNumberOfSlices(layerIndex, moduleIndex); sliceIndex++) {
		thickness += Box(getSlice(layerIndex, moduleIndex, sliceIndex+1).volume().solid())->GetDZ();
	}
	return thickness;
}

double LayerStack::getRadiationLengths(int layerIndex, int moduleIndex) const {
	double radiationLengths = 0.;
	for (int sliceIndex = 0; sliceIndex < getNumberOfSlices(layerIndex, moduleIndex); sliceIndex++) {
		radiationLengths += Box(getSlice(layerIndex, moduleIndex, sliceIndex+1).volume().solid())->GetDZ() / getSlice(layerIndex, moduleIndex).volume().material().radLength();
	}
	return radiationLengths;
}

double LayerStack::getInteractionLengths(int layerIndex, int moduleIndex) const {
	double interactionLengths = 0.;
	for (int sliceIndex = 0; sliceIndex < getNumberOfSlices(layerIndex, moduleIndex); sliceIndex++) {
		interactionLengths += Box(getSlice(layerIndex, moduleIndex, sliceIndex+1).volume().solid())->GetDZ() / getSlice(layerIndex, moduleIndex).volume().material().intLength();
	}
	return interactionLengths;
}

double LayerStack::getTotalThickness(int moduleIndex) const {
	double thickness = 0.;
	for (int layerIndex = 0; layerIndex < getNumberOfLayers(); layerIndex++) {
		thickness += getThickness(layerIndex+1, moduleIndex);
	}
	return thickness;
}

double LayerStack::getTotalInteractionLengths(int moduleIndex) const {
	double interactionLengths = 0.;
	for (int layerIndex = 0; layerIndex < getNumberOfLayers(); layerIndex++) {
		interactionLengths += getInteractionLengths(layerIndex+1, moduleIndex);
	}
	return interactionLengths;
}

double LayerStack::getTotalRadiationLengths(int moduleIndex) const {
	double radiationLengths = 0.;
	for (int layerIndex = 0; layerIndex < getNumberOfLayers(); layerIndex++) {
		radiationLengths += getRadiationLengths(layerIndex+1, moduleIndex);
	}
	return radiationLengths;
}

PolyhedralCalorimeterLayerStack::PolyhedralCalorimeterLayerStack(const DetElement& det) : LayerStack(det) {

}

PolyhedralCalorimeterLayerStack::PolyhedralCalorimeterLayerStack(const PolyhedralCalorimeterLayerStack& layerStack, const DetElement& det) : LayerStack(layerStack, det) {

}

PolyhedralCalorimeterLayerStack::~PolyhedralCalorimeterLayerStack() {

}

int PolyhedralCalorimeterLayerStack::getNumberOfLayers() const {
	stringstream moduleName;
	moduleName << "stave1";
	DetElement module = det.child(moduleName.str());
	if (not module.isValid()) {
		std::cerr << "Invalid module name " << moduleName.str() << std::endl;
		return 0;
	}
	return module.children().size();
}

int PolyhedralCalorimeterLayerStack::getNumberOfModules(int layerIndex) const {
	return det.children().size();
}

DetElement PolyhedralCalorimeterLayerStack::getModule(int layerIndex, int moduleIndex) const {
	stringstream moduleName;
	moduleName << "stave" << moduleIndex;
	DetElement module = det.child(moduleName.str());
	if (not module.isValid()) {
		std::cerr << "Invalid module name " << moduleName.str() << std::endl;
		return DetElement();
	}
	stringstream layerName;
	layerName << "layer" << layerIndex;
	DetElement layer = module.child(layerName.str());
	if (not layer.isValid()) {
		std::cerr << "Invalid layer name " << layerName.str() << std::endl;
		return DetElement();
	}
	return layer;
}

int PolyhedralCalorimeterLayerStack::getNumberOfSlices(int layerIndex, int moduleIndex) const {
	return getModule(layerIndex, moduleIndex).children().size();
}

DetElement PolyhedralCalorimeterLayerStack::getSlice(int layerIndex, int moduleIndex, int sliceIndex) const {
	stringstream sliceName;
	sliceName << "slice" << sliceIndex;
	DetElement slice = getModule(layerIndex, moduleIndex).child(sliceName.str());
	if (not slice.isValid()) {
		std::cerr << "Invalid slice name " << sliceName.str() << std::endl;
		return DetElement();
	}
	return getModule(layerIndex, moduleIndex).child(sliceName.str());
}

int PolyhedralCalorimeterLayerStack::getNumberOfSensors(int layerIndex, int moduleIndex) const {
	int nSensors = 0;
	for (int sliceIndex = 0; sliceIndex < getNumberOfSlices(layerIndex, moduleIndex); sliceIndex++) {
		if (getSlice(layerIndex, moduleIndex, sliceIndex+1).volume().isSensitive()) {
			nSensors++;
		}
	}
	return nSensors;
}

DetElement PolyhedralCalorimeterLayerStack::getSensor(int layerIndex, int moduleIndex, int sensorIndex) const {
	int nSensors = 0;
	for (int sliceIndex = 0; sliceIndex < getNumberOfSlices(layerIndex, moduleIndex); sliceIndex++) {
		DetElement slice = getSlice(layerIndex, moduleIndex, sliceIndex+1);
		if (slice.volume().isSensitive()) {
			nSensors++;
		}
		if (nSensors == sensorIndex) {
			return slice;
		}
	}
	throw OutsideGeometryException("No sensor with index " + sensorIndex);
}

} /* namespace Geometry */
} /* namespace DD4hep */
