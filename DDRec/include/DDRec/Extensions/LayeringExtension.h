/*
 * LayeringExtension.h
 *
 * Abstract extension used by the LayeredSubdetector class.
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef rec_LAYERINGEXTENSION_H_
#define rec_LAYERINGEXTENSION_H_

#include "DD4hep/DetElement.h"
#include "DDRec/tempdefs.h"

namespace dd4hep {
namespace rec {

/**
 * Class describing the layer structure of a sub detector.
 * The information for one layer corresponds to a typical module
 * if, for example, a layer consists of multiple modules.
 */
class LayeringExtension {
public:
	/// Destructor
	virtual ~LayeringExtension() {
	}

	/// Access to the total number of layers
	virtual int numberOfLayers() const = 0;

	/// Access to the total number of sensors in a given layer index
	virtual int numberOfSensors(int layerIndex) const = 0;

	/// Access to the layer DetElement for the given index
	virtual DetElement layer(int layerIndex) const = 0;

	/// Access to the sensitive DetElements of a given layer index
	virtual const std::vector<DetElement>& sensors(int layerIndex) const = 0;

	/// Access to the non-sensitive DetElements of a given layer index
	virtual const std::vector<DetElement>& absorbers(int layerIndex) const = 0;

	/// Access the total thickness of the sub detector
	virtual double totalThickness() const = 0;

	/// Access the total radiation length of the sub detector
	virtual double totalRadiationLength() const = 0;

	/// Access the total nuclear interaction length of the sub detector
	virtual double totalInteractionLength() const = 0;

	/// Access the total thickness of the layer with the given index
	virtual double thickness(int layerIndex) const = 0;

	/// Access the total radiation length of the layer with the given index
	virtual double radiationLength(int layerIndex) const = 0;

	/// Access the total nuclear interaction length of the layer with the given index
	virtual double interactionLength(int layerIndex) const = 0;

	/// Access the total thickness of all non-sensitive elements of the layer with the given index
	virtual double absorberThickness(int layerIndex) const = 0;

	/// Access the total radiation length of all non-sensitive elements of the layer with the given index
	virtual double absorberRadiationLength(int layerIndex) const = 0;

	/// Access the total nuclear interaction length of all non-sensitive elements of the layer with the given index
	virtual double absorberInteractionLength(int layerIndex) const = 0;

	/// Access the total thickness of all sensitive elements of the layer with the given index
	virtual double sensorThickness(int layerIndex) const = 0;

	/// Access the total radiation length of all sensitive elements of the layer with the given index
	virtual double sensorRadiationLength(int layerIndex) const = 0;

	/// Access the total nuclear interaction length of all sensitive elements of the layer with the given index
	virtual double sensorInteractionLength(int layerIndex) const = 0;

protected:
	LayeringExtension() {
	}
};

} /* namespace rec */
} /* namespace dd4hep */
#endif /* rec_LAYERINGEXTENSION_H_ */
