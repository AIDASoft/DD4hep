/*
 * LayeredSubdetector.h
 *
 * Basic reconstruction interface for layered subdetectors
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef rec_LAYEREDSUBDETECTOR_H_
#define rec_LAYEREDSUBDETECTOR_H_

#include "DDRec/API/Exceptions.h"
#include "DDRec/Extensions/LayeringExtension.h"

#include "DD4hep/DetElement.h"

namespace dd4hep {
namespace rec {

class [[gnu::deprecated(" unmaintained code ")]] LayeredSubdetector: public virtual DetElement, public LayeringExtension {
public:
	/// Default constructor
	LayeredSubdetector(const DetElement& det) :
			DetElement(det) {
		getLayeringExtension();
	}

	/// Destructor
	virtual ~LayeredSubdetector() {
		// does not own the extension!
	}

	int numberOfLayers() const {
		return _layering->numberOfLayers();
	}

	int numberOfSensors(int layerIndex = 0) const {
		return _layering->numberOfSensors(layerIndex);
	}

	DetElement layer(int layerIndex) const {
		return _layering->layer(layerIndex);
	}

	const std::vector<DetElement>& sensors(int layerIndex = 0) const {
		return _layering->sensors(layerIndex);
	}

	const std::vector<DetElement>& absorbers(int layerIndex = 0) const {
		return _layering->absorbers(layerIndex);
	}

	double totalThickness() const {
		return _layering->totalThickness();
	}

	double totalRadiationLength() const {
		return _layering->totalRadiationLength();
	}

	double totalInteractionLength() const {
		return _layering->totalInteractionLength();
	}

	double thickness(int layerIndex = 0) const {
		return _layering->thickness(layerIndex);
	}

	double radiationLength(int layerIndex = 0) const {
		return _layering->radiationLength(layerIndex);
	}

	double interactionLength(int layerIndex = 0) const {
		return _layering->interactionLength(layerIndex);
	}

	/// Access the total thickness of all non-sensitive elements of the layer with the given index
	double absorberThickness(int layerIndex) const {
		return _layering->absorberThickness(layerIndex);
	}

	/// Access the total radiation length of all non-sensitive elements of the layer with the given index
	double absorberRadiationLength(int layerIndex) const {
		return _layering->absorberRadiationLength(layerIndex);
	}

	/// Access the total nuclear interaction length of all non-sensitive elements of the layer with the given index
	double absorberInteractionLength(int layerIndex) const {
		return _layering->absorberInteractionLength(layerIndex);
	}

	/// Access the total thickness of all sensitive elements of the layer with the given index
	virtual double sensorThickness(int layerIndex) const {
		return _layering->sensorThickness(layerIndex);
	}

	/// Access the total radiation length of all sensitive elements of the layer with the given index
	double sensorRadiationLength(int layerIndex) const {
		return _layering->sensorRadiationLength(layerIndex);
	}

	/// Access the total nuclear interaction length of all sensitive elements of the layer with the given index
	double sensorInteractionLength(int layerIndex) const {
		return _layering->sensorInteractionLength(layerIndex);
	}


protected:
	LayeringExtension* _layering;

private:
	/// Helper method to retrieve the extension
	void getLayeringExtension() {
		_layering = this->isValid() ? this->extension<LayeringExtension>() : 0;
		if (not _layering) {
			throw invalid_detector_element("Found no extension of type \"LayeringExtension\"", DetElement(*this));
		}
	}
};

} /* namespace rec */
} /* namespace dd4hep */
#endif /* reconstruction_LAYEREDSUBDETECTOR_H_ */
