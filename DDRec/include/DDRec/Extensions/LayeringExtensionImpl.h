/*
 * LayeringExtensionImpl.h
 *
 * Simple implementation of the LayeringExtension interface.
 *
 *  Created on: Mar 6, 2014
 *      Author: Christian Grefe, CERN
 */

#ifndef DDRec_LAYERINGEXTENSIONIMPL_H_
#define DDRec_LAYERINGEXTENSIONIMPL_H_

// DDRec
#include "DDRec/Extensions/LayeringExtension.h"

// DD4hep
#include "DD4hep/Detector.h"

// C++
#include <map>
#include <vector>

class TGeoManager;

namespace DD4hep {
namespace DDRec {

class  [[gnu::deprecated(" unmaintained code ")]] LayeringExtensionImpl: public LayeringExtension {
public:
  /// Shortcut to use geometrical positions
  typedef Geometry::Position Position;
  
	/// Default constructor
	LayeringExtensionImpl();

	/// Copy constructor
  LayeringExtensionImpl(const LayeringExtensionImpl& /*e*/, const Geometry::DetElement& /*d*/) {};

	 /// Destructor
	 virtual ~LayeringExtensionImpl();

	 /// Access to the total number of layers
	 virtual int numberOfLayers() const;

	 /// Access to the total number of sensors in a given layer index
	 virtual int numberOfSensors(int layerIndex) const;

	 /// Access to the layer DetElement for the given index
	 virtual Geometry::DetElement layer(int layerIndex) const;

	/// Access to the sensitive DetElements of a given layer index
	virtual const std::vector<Geometry::DetElement>& sensors(int layerIndex) const;

	/// Access to the non-sensitive DetElements of a given layer index
	virtual const std::vector<Geometry::DetElement>& absorbers(int layerIndex) const;

	/// Access the total thickness of the sub detector
	virtual double totalThickness() const;

	/// Access the total radiation length of the sub detector
	virtual double totalRadiationLength() const;

	/// Access the total nuclear interaction length of the sub detector
	virtual double totalInteractionLength() const;

	/// Access the total thickness of the layer with the given index
	virtual double thickness(int layerIndex) const;

	/// Access the total radiation length of the layer with the given index
	virtual double radiationLength(int layerIndex) const;

	/// Access the total nuclear interaction length of the layer with the given index
	virtual double interactionLength(int layerIndex) const;

	/// Access the total thickness of all non-sensitive elements of the layer with the given index
	virtual double absorberThickness(int layerIndex) const;

	/// Access the total radiation length of all non-sensitive elements of the layer with the given index
	virtual double absorberRadiationLength(int layerIndex) const;

	/// Access the total nuclear interaction length of all non-sensitive elements of the layer with the given index
	virtual double absorberInteractionLength(int layerIndex) const;

	/// Access the total thickness of all sensitive elements of the layer with the given index
	virtual double sensorThickness(int layerIndex) const;

	/// Access the total radiation length of all sensitive elements of the layer with the given index
	virtual double sensorRadiationLength(int layerIndex) const;

	/// Access the total nuclear interaction length of all sensitive elements of the layer with the given index
	virtual double sensorInteractionLength(int layerIndex) const;

	/// Stores the layer information for the given layer index
	void setLayer(int layerIndex, Geometry::DetElement layer, const Position& normal);

protected:
	/// Helper class to store layer attributes
	struct LayerAttributes {
		LayerAttributes();
		Geometry::DetElement layer;
		Position normal;
		std::vector<Geometry::DetElement> sensors;
		std::vector<Geometry::DetElement> absorbers;
		double thickness;
		double radiationLength;
		double interactionLength;
		double absorberThickness;
		double absorberRadiationLength;
		double absorberInteractionLength;
		double sensorThickness;
		double sensorRadiationLength;
		double sensorInteractionLength;
		bool isCalculated;
		/// Helper method to calculate attributes from layer
		void calculate();
	private:
		double addElement(const Geometry::DetElement& det);
		TGeoManager* _tgeoManager;
	};

	mutable std::map<int, LayerAttributes> _layerMap; /// Map to store layer attributes

	/// Helper method to check validity of layer entry
	void checkMap(int layerIndex) const;
};

} /* namespace DDRec */
} /* namespace DD4hep */

#endif /* DDReconstruction_LAYERINGEXTENSIONIMPL_H_ */
