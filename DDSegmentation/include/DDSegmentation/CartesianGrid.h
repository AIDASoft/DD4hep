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
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRID_H_ */
