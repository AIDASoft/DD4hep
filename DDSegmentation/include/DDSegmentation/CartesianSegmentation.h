/*
 * CartesianSegmentation.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CARTESIANSEGMENTATION_H_
#define CARTESIANSEGMENTATION_H_

#include "DDSegmentation/Segmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianSegmentation: public Segmentation {
public:
	virtual ~CartesianSegmentation();
protected:
	CartesianSegmentation(const std::string& cellEncoding);
	CartesianSegmentation(const BitField64& decoder);

	double binToPosition(long64 bin, double cellSize, double offset) const;
	int positionToBin(double position, double cellSize, double offset) const;
};

} /* namespace Segmentation */
} /* namespace DD4hep */
#endif /* CARTESIANSEGMENTATION_H_ */
