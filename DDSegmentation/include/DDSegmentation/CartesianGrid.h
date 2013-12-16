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
	/// Destructor
	virtual ~CartesianGrid();
protected:
	/// Default constructor used by derived classes passing the encoding string
	CartesianGrid(const std::string& cellEncoding = "");
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRID_H_ */
