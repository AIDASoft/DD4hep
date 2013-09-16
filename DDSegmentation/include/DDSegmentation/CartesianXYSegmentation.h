/*
 * CartesianXYSegmentation.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CARTESIANXYSEGMENTATION_H_
#define CARTESIANXYSEGMENTATION_H_

#include "DDSegmentation/CartesianSegmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianXYSegmentation: public CartesianSegmentation {
public:
	CartesianXYSegmentation(const std::string& cellEncoding, double cellSizeX, double cellSizeY, double offsetX = 0.,
			double offsetY = 0.);
	CartesianXYSegmentation(const BitField64& decoder, double cellSizeX, double cellSizeY, double offsetX = 0.,
			double offsetY = 0.);
	virtual ~CartesianXYSegmentation();


	virtual std::vector<double> getLocalPosition(const long64& cellID) const;

	virtual long64 getCellID(double x, double y, double z) const;

	double getCellSizeX() const {
		return _cellSizeX;
	}

	double getCellSizeY() const {
		return _cellSizeY;
	}

	double getOffsetX() const {
		return _offsetX;
	}

	double getOffsetY() const {
		return _offsetY;
	}

protected:
	double _cellSizeX;
	double _offsetX;
	double _cellSizeY;
	double _offsetY;
};

} /* namespace Segmentation */
} /* namespace DD4hep */
#endif /* CARTESIANXYSEGMENTATION_H_ */
