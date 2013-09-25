/*
 * CartesianGrid.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CARTESIANGRID_H_
#define DDSegmentation_CARTESIANGRID_H_

#include "DDSegmentation/Segmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianGrid: public Segmentation {
public:
	/// destructor
	virtual ~CartesianGrid();
protected:
	/// default constructor using an arbitrary type
	template <typename TYPE> CartesianGrid(TYPE cellEncoding);
	/// default constructor using an existing decoder
	CartesianGrid(BitField64* decoder);

	/// helper method to convert a bin number to a 1D position
	double binToPosition(long64 bin, double cellSize, double ofset) const;
	/// helper method to convert a 1D position to a cell ID
	int positionToBin(double position, double cellSize, double offset) const;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRID_H_ */
