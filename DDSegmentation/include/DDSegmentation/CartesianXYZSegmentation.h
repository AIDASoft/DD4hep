/*
 * CartesianXYZSegmentation.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CARTESIANXYZSEGMENTATION_H_
#define CARTESIANXYZSEGMENTATION_H_

#include "DDSegmentation/CartesianXYSegmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianXYZSegmentation: public CartesianXYSegmentation {
public:
	CartesianXYZSegmentation(const std::string& cellEncoding, double cellSizeX, double cellSizeY, double cellSizeZ,
			double offsetX = 0., double offsetY = 0., double offsetZ = 0.);
	CartesianXYZSegmentation(const BitField64& decoder, double cellSizeX, double cellSizeY, double cellSizeZ, double offsetX =
			0., double offsetY = 0., double offsetZ = 0.);
	virtual ~CartesianXYZSegmentation();

	virtual std::vector<double> getLocalPosition(const long64& cellID) const;

	virtual long64 getCellID(double x, double y, double z) const;

	double getCellSizeZ() const {
		return _cellSizeZ;
	}

	double getOffsetZ() const {
		return _offsetZ;
	}

protected:
	double _cellSizeZ;
	double _offsetZ;
};

} /* namespace Segmentation */
} /* namespace DD4hep */
#endif /* CARTESIANXYZSEGMENTATION_H_ */
