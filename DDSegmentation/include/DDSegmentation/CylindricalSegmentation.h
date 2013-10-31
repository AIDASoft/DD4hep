/*
 * CylindricalSegmentation.h
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CYLINDRICALSEGMENTATION_H_
#define DDSegmentation_CYLINDRICALSEGMENTATION_H_

#include "DDSegmentation/Segmentation.h"

#include <map>

namespace DD4hep {
namespace DDSegmentation {

class CylindricalSegmentation: public Segmentation {
public:
	/// destructor
	virtual ~CylindricalSegmentation();

	/// determine the radius based on the cell ID
	std::string getRadius(const long64& cellID) const;
	/// access the field name used for layer
	std::string getFieldNameLayer() const {
		return _layerID;
	}
	/// set the field name used for the layer
	void setFieldNameLayer(const std::string& name) {
		_layerID = name;
	}
	/// set the radius of the given layer index
	void setLayerRadius(int layerIndex, double radius) {
		_layerRadiusMap[layerIndex] = radius;
	}
protected:
	/// default constructor using an arbitrary type
	template <typename TYPE> CylindricalSegmentation(TYPE cellEncoding, const std::string& layerField = "layer");
	/// default constructor using an existing decoder
	CylindricalSegmentation(BitField64* decoder);
	/// store layer radii
	std::map<int, double> _layerRadiusMap;
	std::string _layerID;
};


} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CYLINDRICALSEGMENTATION_H_ */
