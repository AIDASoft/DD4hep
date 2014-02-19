/*
 * LayerStack.h
 *
 *  Created on: Apr 18, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef LAYERSTACK_H_
#define LAYERSTACK_H_

#include "DD4hep/Detector.h"

#include <vector>

namespace DD4hep {
namespace DDRec {

class LayerStack {
public:
	LayerStack(const DetElement& det);
	LayerStack(const LayerStack& layerStack, const DetElement& det);
	virtual ~LayerStack();
	virtual int getNumberOfLayers() const = 0;
	virtual int getNumberOfModules(int layerIndex = 1) const = 0;
	virtual DetElement getModule(int layerIndex = 1, int moduleIndex = 1) const = 0;
	double getThickness(int layerIndex = 1, int moduleIndex = 1) const;
	double getRadiationLengths(int layerIndex = 1, int moduleIndex = 1) const;
	double getInteractionLengths(int layerIndex = 1, int moduleIndex = 1) const;
	double getTotalThickness(int moduleIndex = 1) const;
	double getTotalInteractionLengths(int moduleIndex = 1) const;
	double getTotalRadiationLengths(int moduleIndex = 1) const;
	virtual int getNumberOfSlices(int layerIndex = 1, int moduleIndex = 1) const = 0;
	virtual DetElement getSlice(int layerIndex = 1, int moduleIndex = 1, int sliceIndex = 1) const = 0;
	virtual int getNumberOfSensors(int layerIndex = 1, int moduleIndex = 1) const = 0;
	virtual DetElement getSensor(int layerIndex = 1, int moduleIndex = 1, int sensorIndex = 1) const = 0;

protected:
	DetElement det;
};

class PolyhedralCalorimeterLayerStack: public LayerStack {
public:
	PolyhedralCalorimeterLayerStack(const DetElement& det);
	PolyhedralCalorimeterLayerStack(const PolyhedralCalorimeterLayerStack& layerStack, const DetElement& det);
	virtual ~PolyhedralCalorimeterLayerStack();
	int getNumberOfLayers() const;
	int getNumberOfModules(int layerIndex = 1) const;
	DetElement getModule(int layerIndex = 1, int moduleIndex = 1) const;
	int getNumberOfSlices(int layerIndex = 1, int moduleIndex = 1) const;
	DetElement getSlice(int layerIndex = 1, int moduleIndex = 1, int sliceIndex = 1) const;
	int getNumberOfSensors(int layerIndex = 1, int moduleIndex = 1) const;
	DetElement getSensor(int layerIndex = 1, int moduleIndex = 1, int sensorIndex = 1) const;
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* LAYERSTACK_H_ */
