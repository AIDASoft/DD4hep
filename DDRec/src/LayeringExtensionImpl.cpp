/*
 * LayeringExtensionImpl.cpp
 *
 *  Created on: Mar 6, 2014
 *      Author: cgrefe
 */

#include "DDRec/Extensions/LayeringExtensionImpl.h"

#include "DD4hep/LCDD.h"

#include "TGeoManager.h"

namespace DD4hep {
namespace DDRec {

using Geometry::DetElement;

using std::vector;
using std::map;

/// Default constructor
LayeringExtensionImpl::LayeringExtensionImpl() {
}

/// Copy constructor
//LayeringExtensionImpl::LayeringExtensionImpl(const LayeringExtensionImpl& e, const DetElement& d) {}

/// Destructor
LayeringExtensionImpl::~LayeringExtensionImpl() {
}

/// Access to the total number of layers
int LayeringExtensionImpl::numberOfLayers() const {
	return _layerMap.size();
}

/// Access to the total number of sensors in a given layer index
int LayeringExtensionImpl::numberOfSensors(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).sensors.size();
}

/// Access to the layer DetElement for the given index
DetElement LayeringExtensionImpl::layer(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).layer;
}

/// Access to the sensitive DetElements of a given layer index
const vector<DetElement>& LayeringExtensionImpl::sensors(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).sensors;
}

/// Access to the non-sensitive DetElements of a given layer index
const vector<DetElement>& LayeringExtensionImpl::absorbers(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).absorbers;
}

/// Access the total thickness of the sub detector
double LayeringExtensionImpl::totalThickness() const {
	map<int, LayerAttributes>::iterator it = _layerMap.begin();
	double thick_ness = 0.;
	while (it != _layerMap.end()) {
		LayerAttributes& attributes = it->second;
		if (not attributes.isCalculated) {
			attributes.calculate();
		}
		thick_ness += attributes.thickness;
		++it;
	}
	return thick_ness;
}

/// Access the total radiation length of the sub detector
double LayeringExtensionImpl::totalRadiationLength() const {
	map<int, LayerAttributes>::iterator it = _layerMap.begin();
	double radiation_length = 0.;
	while (it != _layerMap.end()) {
		LayerAttributes& attributes = it->second;
		if (not attributes.isCalculated) {
			attributes.calculate();
		}
		radiation_length += attributes.radiationLength;
		++it;
	}
	return radiation_length;
}

/// Access the total nuclear interaction length of the sub detector
double LayeringExtensionImpl::totalInteractionLength() const {
	map<int, LayerAttributes>::iterator it = _layerMap.begin();
	double interaction_length = 0.;
	while (it != _layerMap.end()) {
		LayerAttributes& attributes = it->second;
		if (not attributes.isCalculated) {
			attributes.calculate();
		}
		interaction_length += attributes.interactionLength;
		++it;
	}
	return interaction_length;
}

/// Access the total thickness of the layer with the given index
double LayeringExtensionImpl::thickness(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).thickness;
}

/// Access the total radiation length of the layer with the given index
double LayeringExtensionImpl::radiationLength(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).radiationLength;
}

/// Access the total nuclear interaction length of the layer with the given index
double LayeringExtensionImpl::interactionLength(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).interactionLength;
}

/// Access the total thickness of all non-sensitive elements of the layer with the given index
double LayeringExtensionImpl::absorberThickness(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).absorberThickness;
}

/// Access the total radiation length of all non-sensitive elements of the layer with the given index
double LayeringExtensionImpl::absorberRadiationLength(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).absorberRadiationLength;
}

/// Access the total nuclear interaction length of all non-sensitive elements of the layer with the given index
double LayeringExtensionImpl::absorberInteractionLength(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).absorberInteractionLength;
}

/// Access the total thickness of all sensitive elements of the layer with the given index
double LayeringExtensionImpl::sensorThickness(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).sensorThickness;
}

/// Access the total radiation length of all sensitive elements of the layer with the given index
double LayeringExtensionImpl::sensorRadiationLength(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).sensorRadiationLength;
}

/// Access the total nuclear interaction length of all sensitive elements of the layer with the given index
double LayeringExtensionImpl::sensorInteractionLength(int layerIndex) const {
	checkMap(layerIndex);
	return _layerMap.at(layerIndex).sensorInteractionLength;
}

/// Stores the layer information for the given layer index
void LayeringExtensionImpl::setLayer(int layerIndex, Geometry::DetElement layer_elt, const Geometry::Position& normal) {
	LayerAttributes& layerAttributes = _layerMap[layerIndex];
	layerAttributes.layer = layer_elt;
	layerAttributes.normal = normal.Unit();
	layerAttributes.isCalculated = false;
}

/// Helper method to check validity of layer entry
void LayeringExtensionImpl::checkMap(int layerIndex) const {
	map<int, LayerAttributes>::iterator it;
	it = _layerMap.find(layerIndex);
	if (it == _layerMap.end()) {
		// TODO throw exception
	}
	if (not it->second.isCalculated) {
		it->second.calculate();
	}
}

LayeringExtensionImpl::LayerAttributes::LayerAttributes() :
		thickness(0.), radiationLength(0.), interactionLength(0.), absorberThickness(0.), absorberRadiationLength(0.), absorberInteractionLength(
				0.), sensorThickness(0.), sensorRadiationLength(0.), sensorInteractionLength(0.), isCalculated(false) {
	_tgeoManager = Geometry::LCDD::getInstance().world().volume()->GetGeoManager();
}

void LayeringExtensionImpl::LayerAttributes::calculate() {
	// reset all values
	sensors.clear();
	absorbers.clear();
	thickness = 0.;
	radiationLength = 0.;
	interactionLength = 0.;
	absorberThickness = 0.;
	absorberRadiationLength = 0.;
	absorberInteractionLength = 0.;
	sensorThickness = 0.;
	sensorRadiationLength = 0.;
	sensorInteractionLength = 0.;
	// add all children recursively starting from top
	addElement(this->layer);
	isCalculated = true;
}

double LayeringExtensionImpl::LayerAttributes::addElement(const DetElement& det) {
	double daughterThickness = 0.;
	double thisThickness = 0.;

	const DetElement::Children& children = det.children();
	DetElement::Children::const_iterator it = children.begin();
	while (it != children.end()) {
		daughterThickness += addElement(it->second);
		++it;
	}

	Geometry::Volume volume = det.volume();
	if (volume.isValid() and volume.solid().isValid()) {
		Geometry::Solid solid = volume.solid();
		Geometry::Material material = volume.material();
		double origin[3] = { 0., 0., 0. };
		double direction[3] = { normal.x(), normal.y(), normal.z() };
		double reverse_direction[3] = { -normal.x(), -normal.y(), -normal.z() };
		thisThickness = solid->DistFromInside(origin, direction) + solid->DistFromInside(origin, reverse_direction);
		// if daughters had volumes with thicknesses subtract those
		double effectiveThickness = thisThickness - daughterThickness;
		double radLength = effectiveThickness / material.radLength();
		double intLength = effectiveThickness / material.intLength();
		thickness += effectiveThickness;
		interactionLength += intLength;
		radiationLength += radLength;
		if (volume.isSensitive()) {
			sensorThickness += thickness;
			sensorInteractionLength += intLength;
			sensorRadiationLength += radLength;
			sensors.push_back(det);
		} else {
			absorbers.push_back(det);
			absorberThickness += thickness;
			absorberInteractionLength += intLength;
			absorberRadiationLength += radLength;
		}
	}
	return thisThickness;
}

} /* namespace DDRec */
} /* namespace DD4hep */
