/*
 * LayeredSubdetector.h
 *
 *  Created on: Mar 27, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef LAYEREDSUBDETECTOR_H_
#define LAYEREDSUBDETECTOR_H_

#include "LayerStack.h"

#include "DD4hep/Detector.h"

namespace DD4hep {
namespace Geometry {

class LayeredSubdetector: virtual public DetElement {
public:
	LayeredSubdetector(const DetElement& e);
	virtual ~LayeredSubdetector();

	inline bool isLayered() const {
		return true;
	}

	inline int getNumberOfLayers() const {
		return layerStack->getNumberOfLayers();
	}

	inline int getNumberOfSensors(int layerIndex, int moduleIndex = 1) const {
		return layerStack->getNumberOfSensors(layerIndex, moduleIndex);
	}

	inline DetElement getSensor(int layerIndex, int moduleIndex = 1, int sensorIndex = 1) const {
		return layerStack->getSensor(layerIndex, moduleIndex, sensorIndex);
	}

	inline double getLayerThickness(int layerIndex, int moduleIndex = 1) const {
		return layerStack->getThickness(layerIndex, moduleIndex);
	}

	inline double getInteractionLengths(int layerIndex, int moduleIndex = 1) const {
		return layerStack->getInteractionLengths(layerIndex, moduleIndex);
	}

	inline double getRadiationLengths(int layerIndex, int moduleIndex = 1) const {
		return layerStack->getRadiationLengths(layerIndex, moduleIndex);
	}

	inline double getMipEnergyLoss(int layerIndex, int moduleIndex = 1) const {
		return 0.;
	}

	inline double getTotalThickness(int moduleIndex = 1) const {
		return layerStack->getTotalThickness(moduleIndex);
	}

	inline double getTotalInteractionLengths(int moduleIndex = 1) const {
		return layerStack->getTotalInteractionLengths(moduleIndex);
	}

	inline double getTotalRadiationLengths(int moduleIndex = 1) const {
		return layerStack->getTotalRadiationLengths(moduleIndex);
	}

protected:
	LayerStack* layerStack;

private:
	void getExtension();

};

} /* namespace Geometry */
} /* namespace DD4hep */

#endif /* LAYEREDSUBDETECTOR_H_ */
